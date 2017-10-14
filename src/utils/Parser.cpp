#include "utils/Parser.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

#include "core/Dispatcher.h"
#include "dispatchers/Aperiodic.h"
#include "dispatchers/Periodic.h"
#include "dispatchers/PeriodicJitter.h"

#include "configuration/Scratch.h"
#include "utils/TimeUtil.h"
#include "utils/Enumerations.h"
#include "utils/vectormath.h"
#include "utils/FileOperator.h"
#include "core/structdef.h"

#include "performance_counter/PerformanceCounters.h"



using namespace pugi;
using namespace std;

bool string2bool(string s){
	if (s == "False" || s == "false"){
		return false;
	}else if (s == "True" || s == "true"){
		return true;
	}else{
		cout << "Parser:: not recognized bool string!" << endl;
		exit(1);
	}
}

void ParserPAPITest(int argc, char** argv){
	PerformanceCounters::PAPI_test(argc, argv);
}


Parser::Parser(string _xml_path){
	xml_path = _xml_path;
}

int Parser::parseThermalModel(
	std::vector<std::vector<double> >& C, 
	std::vector<std::vector<double> >& G,
	std::vector<std::vector<double> >& K,
	double& ambientT,
	double& period){

	int ret = 0;

	// load the xml file
	xml_document doc;
	if( !doc.load_file(xml_path.data()) ){
		cout << getexepath() << endl;
		std::cout << "xml_path: " << xml_path.data() << endl;
		std::cout << "Could not find file...\n";
		return -1;
	}

	xml_node processor = doc.child("processor");
	xml_node model = processor.child("thermal_model");

	C.clear();
	G.clear();
	K.clear();

	C = loadMatrixFromFile<double>(model.child("parameterC").attribute("path").as_string());
	G = loadMatrixFromFile<double>(model.child("parameterG").attribute("path").as_string());
	K = loadMatrixFromFile<double>(model.child("parameterK").attribute("path").as_string());

	ambientT = model.child("AmbientT").attribute("value").as_double();
	period = parseTimeMircroSecond(model.child("period"))/1000000;

	return ret;
}
// This function parse the file pointed by xml_path, and then 
// save all necessary data required by the simulation in Scratch class.
int Parser::parseFile(){
	
	int ret = 0;

	// load the xml file
	xml_document doc;
	if( !doc.load_file(xml_path.data()) ){
		cout << getexepath() << endl;
		std::cout << "xml_path: " << xml_path.data() << endl;
		std::cout << "Could not find file...\n";
		return -1;
	}

	// get experiment name and duration
	xml_node sim_node      = doc.child("experiment");
	string name            = sim_node.attribute("name").value();
	xml_node TempSensors   = sim_node.child("temperature_sensors");

	bool useHardwareTempSensor = false;
	bool useSoftTempSensor = false;
	string softSensorCalculator;
	string parameterA, parameterB, parameterK;
	
	xml_node softSensorNode;
	xml_node hardSensorNode;

	for (xml_node sensor = TempSensors.first_child(); sensor; sensor = sensor.next_sibling())
	{
		string sensor_type = sensor.attribute("type").as_string();
		if (!useSoftTempSensor && sensor_type == "soft"){
			useSoftTempSensor = true;
			softSensorNode = sensor;
			softSensorCalculator = sensor.attribute("calculator").as_string() ;
			parameterA = sensor.attribute("parameterA").as_string() ;
			parameterB = sensor.attribute("parameterB").as_string() ;
			parameterK = sensor.attribute("parameterK").as_string() ;
			
		}else if (!useHardwareTempSensor && sensor_type == "hardware"){
			useHardwareTempSensor = true;
			hardSensorNode = sensor;
		}

	}
	
	

	// save the duration in microsecond unit
	unsigned long duration = parseTimeMircroSecond(sim_node.child("duration"));
	
	// get pipeline stage number
	xml_node processor     = sim_node.child("processor");
	int ncores             = (int) stoul(processor.attribute("core_number").value(), NULL, 0);

	// put all the necessary input parameters in scratch
	Scratch::initialize(ncores, duration, name, useHardwareTempSensor, useSoftTempSensor);

	string fixed_frequency = processor.attribute("fixed_frequency").as_string();
	string fixed_active = processor.attribute("fixed_active").as_string();

	Scratch::setFixedFrequency(string2bool(fixed_frequency));
	Scratch::setFixedActive(string2bool(fixed_active));


	// get parameters of all tasks
	xml_node task_node     = sim_node.child("tasks");
	//iterate through all of the children nodes
	int taskid = 0;
	for (xml_node task = task_node.first_child(); task; task = task.next_sibling()){
		
		TaskArgument data = parseTask(task, taskid);
		++taskid;
		Scratch::addTask(data._type, data._periodicity, data);
	}

	xml_node thermal_approach = sim_node.child("thermal_approach");
	xml_node kernel = thermal_approach.child("kernel");
	string kerneltype = kernel.attribute("type").as_string();
	if (kerneltype == "pboo" || kerneltype == "PBOO"){
		Scratch::setStaticApproach(true);
		vector<long int> tons_us = parseTimeVectorMicro<long int>(kernel.child("ton"));
		vector<long int> toffs_us = parseTimeVectorMicro<long int>(kernel.child("toff"));

		Scratch::setPBOOTons(TimeUtil::Micros(tons_us));
		Scratch::setPBOOToffs(TimeUtil::Micros(toffs_us));

	}
	
   // if save the results into files
	xml_node isSaveFile	   = sim_node.child("save_result");
	if (isSaveFile){
		string isSave 		=  isSaveFile.attribute("value").value();
		if ((isSave == "false") || (isSave == "False")){
			Scratch::setSavingFile(false);
		}else if ((isSave == "true") || (isSave == "True")){
			Scratch::setSavingFile(true);
		}else{
			cout << "Parser warning: parameter of saving result error! set to default TRUE value\n";
		}
	}

	
	if (useSoftTempSensor){
					
		string parameterA = softSensorNode.attribute("parameterA").as_string() ;
		string parameterB = softSensorNode.attribute("parameterB").as_string() ;
		string parameterK = softSensorNode.attribute("parameterK").as_string() ;

		Scratch::softSensorCalculator = softSensorNode.attribute("calculator").as_string() ;
		Scratch::softSensorA = loadMatrixFromFile<double>(parameterA);
		Scratch::softSensorB = loadMatrixFromFile<double>(parameterB);
		Scratch::softSensorK = loadVectorFromFile<double>(parameterK);

		if (softSensorNode.child("period")){
			Scratch::softSamplingInterval = parseTimeMircroSecond(softSensorNode.child("period"));
		}

		xml_node softSensors 	= sim_node.child("soft_temperature_sensor_counters");

		for (xml_node counter = softSensors.first_child(); counter; counter = counter.next_sibling()){

			string name = counter.attribute("name").as_string();

			double coef_a = counter.attribute("coefficient_a").as_double();
			double coef_b = counter.attribute("coefficient_b").as_double();
			Scratch::addSoftTempSensor(name, coef_a, coef_b);
		}
	}

	Scratch::print();
	return ret;
}

TaskArgument Parser::parseTask(pugi::xml_node task, int taskid){
	TaskArgument data = TaskArgument();

	string task_name = task.attribute("name").as_string();
	if(task_name.length() > 0 ){
		data.name = task_name;
	}else{
		ostringstream tmp;
		tmp << taskid;
		data.name = "default_task_name" + tmp.str();
	}

	data.taskId = taskid;

	string task_type = task.attribute("type").as_string();
	string load_type = task.attribute("load_type").as_string();
	string periodicity = task.attribute("periodicity").as_string();

	_task_type type;
	_task_periodicity pcity;
	_task_load_type loadtype;

	if (task_type == "singlecore"){
		type = singlecore;
	}else if (task_type == "pipelined"){
		type = pipelined;
	}else {
		cout << "parseTask: task type was not recognized" << endl;
		exit(1);
	}
	
	data._type = type;


	xml_node attachedCores = task.child("attached_core");
	int attached_core;
	if(attachedCores){
		attached_core = attachedCores.attribute("value").as_int();
		data.default_coreId = attached_core;
	}
	


	vector<long int> wcets = parseTimeVectorMicro<long int>(task.child("wcets")); 
	data.wcets = TimeUtil::Micros(wcets);
	data.wcets_us = TimeUtil::convert_us(data.wcets);

	if (load_type == "busy_wait"){
		loadtype = busywait;		
	}else if (load_type == "benchmark"){
		loadtype = benchmark;
		string benchmark_name = task.child("benchmark").attribute("name").as_string();
			// int benchmark_id = findBenchmarkId(benchmark_name);
		int benchmark_id = 1;
		data.benchmark_id = benchmark_id;
	}else if (load_type == "user_defined"){
		loadtype = userdefined;
		int loadId = task.child("defined_load").attribute("index").as_int();
		data.user_defined_load_id = loadId;

	}else {
		cout << "parseTask: task load type was not recognized" << endl;
		exit(1);
	}
	data._load_type = loadtype;




	if(periodicity == "periodic") {
		pcity = periodic;
		data.period = parseTime(task.child("period"));
	}
	else if(periodicity == "periodic_jitter") {
		pcity = periodic_jitter;
		data.period = parseTime(task.child("period"));
		data.jitter = parseTime(task.child("jitter"));
	}
	else if(periodicity == "aperiodic") {
		pcity = aperiodic;
		data.release_time = parseTime(task.child("release_time"));
	}
	else {
		cout << "parseTask: task periodicity was not recognized" << endl;
		exit(1);
	}
	data._periodicity = pcity;

	return data;

}

unsigned long Parser::parseTimeMircroSecond(xml_node n){
	struct timespec tmp = parseTime(n);
	return TimeUtil::convert_us(tmp);
}

struct timespec Parser::parseTime(xml_node n) {
	int time     = n.attribute("value").as_int();
	string units = n.attribute("unit").value();
	struct timespec ret;

	if(units == "sec") {
		ret = TimeUtil::Seconds(time);
	}
	else if(units == "ms") {
		ret = TimeUtil::Millis(time);
	}
	else if(units == "us") {
		ret = TimeUtil::Micros(time);
	}
	else {
		cout << "Parser error: could not recognize time unit!\n";
		exit(1);
	}

	return ret;
}




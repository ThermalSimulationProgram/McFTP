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

Parser::Parser(string _xml_path){
	xml_path = _xml_path;
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


	// save the duration in microsecond unit
	unsigned long duration = parseTimeMircroSecond(sim_node.child("duration"));
	
	// get pipeline stage number
	xml_node processor     = sim_node.child("processor");
	int ncores             = (int) stoul(processor.attribute("core_number").value(), NULL, 0);

	// put all the necessary input parameters in scratch
	Scratch::initialize(ncores, duration, name);

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


	xml_node attachedCores = task.child("attached_cores");
	vector<int> attached_cores;
	if(attachedCores){
		attached_cores = stringToVector<int>(attachedCores.attribute("value").as_string());
	}else{
		attached_cores = vector<int>(1, 0);
	}
	data.attached_cores = attached_cores;


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




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
	int taskid = 1;
	for (xml_node task = task_node.first_child(); task; task = task.next_sibling()){
		string task_type = task.attribute("type").as_string();
		string periodicity = task.attribute("periodicity").as_string();

		_task_type type;
		_task_periodicity pcity;
		task_data data;
		if (task_type == "busy_wait"){
			type = busywait;
		}else if (task_type == "benchmark"){
			type = benchmark;
			string benchmark_name = task.attribute("benchmark_name").as_string();
			data.benchmark = benchmark_name;
		}else if (task_type == "user_defined"){
			type = userdefined;
		}else {
			cout << "parseFile: task type was not recognized" << endl;
			exit(1);
		}

		 /**** CREATE DISPATCHER ****/
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
			cout << "parseFile: task periodicity was not recognized" << endl;
			exit(1);
		}
		string task_name = task.attribute("name").as_string();

		if(task_name.length() > 0 ){
			data.name = task_name;
		}else{
			ostringstream tmp;
			tmp << taskid;
			data.name = "task" + tmp.str();
		}

		xml_node attachedCores = task.child("attached_cores");
		vector<int> attached_cores;
		if(attachedCores){
			attached_cores = stringToVector<int>(attachedCores.attribute("value").as_string());
		}

		data.attached_cores = attached_cores;

		Scratch::addTask(type, pcity, data);
	}

	xml_node thermal_approach = sim_node.child("thermal_approach");
	xml_node kernel = thermal_approach.child("kernel");
	string kerneltype = kernel.attribute("type").as_string();
	if (kerneltype == "pboo"){
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




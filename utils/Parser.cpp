#include "Parser.h"

#include <iostream>
#include <string>
#include <algorithm>


#include "Scratch.h"
#include "TimeUtil.h"
#include "Enumerations.h"
#include "vectormath.h"
#include "FileOperator.h"



using namespace pugi;
using namespace std;

Parser::Parser(string _xml_path){
	xml_path = _xml_path;
}

// This function parse the file pointed by xml_path, and then 
// save all necessary data required by the simulation in Scratch class.
int Parser::parseFile(){
	return 0;
	/*int ret = 0;

	// load the xml file
	xml_document doc;
	if( !doc.load_file(xml_path.data()) ){
		std::cout << "Could not find file...\n";
		return -1;
	}

	// get simulation name and duration
	xml_node sim_node      = doc.child("simulation");
	string name            = sim_node.attribute("name").value();


	// save the duration in microsecond unit
	unsigned long duration = parseTimeMircroSecond(sim_node.child("duration"));
	
	// get pipeline stage number
	xml_node pipe_node     = sim_node.child("pipeline");
	int nstage             = (int) stoul(pipe_node.attribute("stagenumber").value(), NULL, 0);

	// get job release times
	vector<unsigned long> rl_release_times;
	xml_node event_node     = sim_node.child("events");
	string arrival_csv_path = event_node.child("csv_path").attribute("value").value();
	string unit             = event_node.child("csv_path").attribute("unit").value();
	
	if(arrival_csv_path.length() > 0){
		vector<double> initv = loadVectorFromFile<double> (arrival_csv_path);
		vector<double> v     = formatTimeMicros<double>(initv, unit);

		for (unsigned i = 0; i < v.size(); ++i){
			if( (i>0) && (v[i]< v[i-1])){			
				cout << "At lease on element in input time vector is less than its" <<
				" previous and is modified to its previous value \n";
				v[i] = v[i-1];
			} 
			rl_release_times.push_back(v[i]);
		}
	}
	
	// get task parameteres: period, jitter, distance, and wcets
	unsigned long period        = parseTimeMircroSecond(
		event_node.child("period"));
	unsigned long jitter        = parseTimeMircroSecond(
		event_node.child("jitter"));
	unsigned long distance      = parseTimeMircroSecond(
		event_node.child("distance"));
	unsigned long rltDeadline   = parseTimeMircroSecond(
		event_node.child("relative_deadline"));
	vector<unsigned long> wcets = parseTimeVectorMicro<unsigned long>(
		event_node.child("wcets"));
	if ((int)wcets.size() != nstage){
		cout << "Parser error: wcetvalues' size doesn't match pipeline stage number\n";
		return -1;
	}
	
	// get the execution time factor
	double exe_factor = event_node.child("exe_factor").attribute("value").as_double();

	// get scheduler attributes
	xml_node schedule_node = sim_node.child("scheduler");
	xml_node kernel_node   = schedule_node.child("kernel");
	string kernel_type     = kernel_node.attribute("type").value();
	enum _schedule_kernel type = GE;
	if (kernel_type == "APTM")
		type = APTM;
	else if (kernel_type == "BWS")
		type = BWS;
	else if (kernel_type == "PBOO")
		type = PBOO;
	else if (kernel_type == "GE")
		type = GE;
	else{
		cout << "Parser error: could not recognize kernel type!\n";
		return -1;
	}

	// put all the necessary input parameters in scratch
	Scratch::initialize(nstage, period, jitter, distance, 
		rltDeadline, wcets, rl_release_times, type, duration, name);

	Scratch::setExeFactor(exe_factor);

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

	xml_node benchmark = sim_node.child("benchmark");
	if(benchmark){
		string benchmark_name = benchmark.attribute("name").value();
		Scratch::setBenchmark(benchmark_name);
	}
	

	// handle additional parameters for the scheduler
	if(type == APTM){
		xml_node APeriodNode  = kernel_node.child("period");
		xml_node b_factorNode = kernel_node.child("b_factor");
		xml_node offline_node = kernel_node.child("offlinedata");

		if(!APeriodNode){
			cout << "Parser::parseFile(): adaption period is required for APTM kernel\n";
			return -1;
		}
		if(!offline_node){
			cout << "Parser::parseFile(): offline data is required for APTM kernel\n";
			return -1;
		}
		unsigned long adaptPeriod = parseTimeMircroSecond(APeriodNode);
		string prefix             = offline_node.child("prefix").attribute("path").value();
		thermalProp offlinedata   = getOfflineData(prefix, nstage);

		
		Scratch::setAdaptionPeriod(adaptPeriod);
		Scratch::setOfflineData(offlinedata);

		if(b_factorNode){
			double b_factor   	= b_factorNode.attribute("value").as_double();
			Scratch::setBFactor(b_factor);
		}
		
	}else if (type == BWS){
		unsigned long adaptPeriod = parseTimeMircroSecond(kernel_node.child("period"));
		Scratch::setAdaptionPeriod(adaptPeriod);
	}else if (type == PBOO){
		ptmspec ptm;
		vector<unsigned long> tons  = parseTimeVectorMicro<unsigned long>(kernel_node.child("ton"));
		vector<unsigned long> toffs = parseTimeVectorMicro<unsigned long>(kernel_node.child("toff"));
		ptm.tons                    = tons/1000;  
		ptm.toffs                   = toffs/1000;
		Scratch::setPTMValues(ptm);
	}else if (type == GE){
		ptmspec ptm;
		ptm.tons  = vector<double>(nstage,1000);  
		ptm.toffs = vector<double>(nstage,0);
		Scratch::setPTMValues(ptm);
	}
	// Scratch::print();
	return ret;*/
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




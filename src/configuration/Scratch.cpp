#include "configuration/Scratch.h"


#include "utils/utils.h"

#include <iostream>

using namespace std;


string 					Scratch::name;
int 					Scratch::nstage;
// unsigned long 			Scratch::period;
// unsigned long 			Scratch::jitter;
// unsigned long 			Scratch::distance;
// unsigned long 			Scratch::rltDeadline;
// double 					Scratch::rltDeadline_ms;
// vector<unsigned long> 	Scratch::wcets;
// vector<double> 			Scratch::dwcets;
// vector<unsigned long> 	Scratch::arrival_times;
// enum _schedule_kernel 	Scratch::kernel;
unsigned long 			Scratch::duration;
// double 					Scratch::exefactor;
// double 					Scratch::bfactor;
unsigned long 			Scratch::adaption_period;
// ptmspec 				Scratch::ptm;
// thermalProp 			Scratch::offlinedata;
sem_t 					Scratch::access_sem;
bool 					Scratch::isSave;
string 					Scratch::benchmark;
vector<_task_periodicity> Scratch::periodicities;
vector<_task_type> Scratch::task_types;
vector<task_data>  Scratch::all_task_data;


void Scratch::initialize(int _nstage,
	unsigned long _duration,
	string _name){

	nstage 			= _nstage;
	// period          = _period;
	// jitter          = _jitter;
	// distance 		= _distance;
	// rltDeadline 	= _rltDeadline;
	// rltDeadline_ms 	= (double)rltDeadline/1000;
	// wcets           = _wcets;
	// arrival_times   = _arrival_times;
	// kernel 			= _kernel;
	duration 		= _duration;
	name            = _name;
	// adaption_period = 1000;
	// exefactor 		= 1;
	// bfactor 		= 0.93;
	isSave 			= true;
	// for (unsigned i = 0; i < wcets.size(); ++i)
	// 	dwcets.push_back((double)wcets[i]/1000);
	benchmark 		= "default";

	sem_init(&access_sem, 0, 1);
}

void Scratch::print(){
	cout << "nstage \t\t\t= " << nstage << endl;
	// cout << "period \t\t\t= " << period << endl;
	// cout << "jitter \t\t\t= " << jitter << endl;
	// cout << "distance \t\t= " << distance << endl;
	// cout << "rltDeadline \t\t= " << rltDeadline << endl;
	// cout << "rltDeadline_ms \t\t= " << rltDeadline_ms << endl;
	// cout << "kernel \t\t\t= " << kernel << endl;
	cout << "duration \t\t= " << duration << endl;
	cout << "name \t\t\t= " << name << endl;
	cout << "adaption_period \t= " << adaption_period << endl;
	// cout << "exefactor \t\t= " << exefactor << endl;
	// cout << "bfactor \t\t= " << bfactor << endl;
	// displayvector(wcets, "wcets");
	// displayvector(ptm.tons, "tons");
	// displayvector(ptm.toffs, "toffs");
}

void Scratch::addTask(_task_type type, _task_periodicity p, task_data data){
	task_types.push_back(type);
	periodicities.push_back(p);
	all_task_data.push_back(data);
}

void Scratch::setBenchmark(const string& name){
	sem_wait(&access_sem);
	benchmark = name;
	sem_post(&access_sem);
}


void Scratch::setSavingFile(bool f){
	sem_wait(&access_sem);
	isSave = f;
	sem_post(&access_sem);
}

bool Scratch::isSaveFile(){
	sem_wait(&access_sem);
	bool ret = isSave;
	sem_post(&access_sem);
	return ret;
}

void Scratch::setName(string newname){
	sem_wait(&access_sem);
	name = newname;
	sem_post(&access_sem);
}

void Scratch::setAdaptionPeriod(unsigned long p){
	sem_wait(&access_sem);
	adaption_period = p;
	sem_post(&access_sem);
	
}


vector<_task_type>	Scratch::getAllTaskTypes(){
	return task_types;
}
vector<_task_periodicity> 	Scratch::getAllTaskPeriodicity(){
	// vector<_task_periodicity> ret;
	return periodicities;
}

vector<task_data>  Scratch::getTaskData(){
	return all_task_data;
}


std::string  Scratch::getApproachName(){
	return "test";
}

// void Scratch::setPTMValues(ptmspec _ptm){
// 	sem_wait(&access_sem);
// 	if (kernel != PBOO && kernel != GE){
// 		cout << "Scratch::setPTMValues:Set PTM parameters only for PBOO and GE kernel"<< endl;
// 		sem_post(&access_sem);
// 		return;
// 	}
	
// 	ptm = _ptm;
// 	sem_post(&access_sem);
	
// }

// void Scratch::setOfflineData(thermalProp data){
// 	sem_wait(&access_sem);
//  	if (kernel != APTM){
// 		cout << "Scratch::setOfflineData:Set offline data only for APTM kernel"<< endl;
// 		sem_post(&access_sem);
// 		return;
// 	}
// 	if ((int)data.allStageCurves.size()!= nstage){
// 		cout << "Scratch::setOfflineData:Invalid input offline data"<<endl;
// 		sem_post(&access_sem);
// 		return;
// 	}

// 	offlinedata = data;
// 	sem_post(&access_sem);
// }
// thermalProp Scratch::getOfflineData(){
// 	thermalProp ret;
// 	sem_wait(&access_sem);
// 	ret = offlinedata;
// 	sem_post(&access_sem);
// 	return ret;
// }


// void Scratch::setExeFactor(double f){
// 	if(f<0 || f>1){
// 		cout << "Scratch::setExeFactor:Input exefactor out of range" << endl;
// 		return;
// 	}
// 	sem_wait(&access_sem);
// 	exefactor = f;
// 	sem_post(&access_sem);

// }

// void Scratch::setBFactor(double f){
// 	if(f<0 || f>1){
// 		cout << "Scratch::setBFactor:Input bfactor out of range" << endl;
// 		return;
// 	}

// 	sem_wait(&access_sem);
// 	if (kernel != APTM){
// 		sem_post(&access_sem);
// 		cout << "Scratch::setBFactor:Set bfactor only for APTM kernel"<< endl;
// 		return;
// 	}
// 	bfactor = f;
// 	sem_post(&access_sem);

// }

string Scratch::getBenchmarkName(){
	sem_wait(&access_sem);
	string ret = benchmark;
	sem_post(&access_sem);
	return ret;

}

unsigned long Scratch::getAdaptionPeriod(){
	sem_wait(&access_sem);
	unsigned long ret = adaption_period;
	sem_post(&access_sem);
	return ret;

}


// ptmspec Scratch::getPTMValues(){
// 	sem_wait(&access_sem);
// 	if (kernel != PBOO && kernel != GE){
// 		cout << "Scratch::getPTMValues:Warning get PTM parameters only for PBOO and GE kernel";
// 	}
// 	ptmspec ret = ptm;
// 	sem_post(&access_sem);
// 	return ret;
// }


string Scratch::getName(){
	sem_wait(&access_sem);
	string ret = name;
	sem_post(&access_sem);
	return ret;
}
int Scratch::getNstage(){
	sem_wait(&access_sem);
	int ret = nstage;
	sem_post(&access_sem);
	return ret;
}
// unsigned long Scratch::getPeriod(){
// 	sem_wait(&access_sem);
// 	unsigned ret = period;
// 	sem_post(&access_sem);
// 	return ret;
// }
// unsigned long Scratch::getJitter(){
// 	sem_wait(&access_sem);
// 	unsigned long ret = jitter;
// 	sem_post(&access_sem);
// 	return ret;
// }
// unsigned long Scratch::getDistance(){
// 	sem_wait(&access_sem);
// 	unsigned long ret = distance;
// 	sem_post(&access_sem);
// 	return ret;
// }
// unsigned long Scratch::getRltDeadline(){
// 	sem_wait(&access_sem);
// 	unsigned long ret = rltDeadline;
// 	sem_post(&access_sem);
// 	return ret;
// }
// double 	Scratch::getRltDeadline_ms(){
// 	sem_wait(&access_sem);
// 	double ret = rltDeadline_ms;
// 	sem_post(&access_sem);
// 	return ret;
// }
// vector<unsigned long> Scratch::getWcets(){
// 	sem_wait(&access_sem);
// 	vector<unsigned long> ret = wcets;
// 	sem_post(&access_sem);
// 	return ret;
// }
// vector<double> 	Scratch::getDwcets(){
// 	sem_wait(&access_sem);
// 	vector<double> ret = dwcets;
// 	sem_post(&access_sem);
// 	return ret;
// }


// vector<unsigned long> Scratch::getArrivalTimes(){
// 	sem_wait(&access_sem);
// 	vector<unsigned long> ret = arrival_times;
// 	sem_post(&access_sem);
// 	return ret;
// }

// vector<double> Scratch::getArrivalTimes_ms(){
// 	sem_wait(&access_sem);
// 	vector<double> ret;
// 	for (unsigned i = 0; i < arrival_times.size(); ++i)
// 		ret.push_back((double)arrival_times[i]/1000);
	
// 	sem_post(&access_sem);
	
// 	return ret;
// }


// enum _schedule_kernel Scratch::getKernel(){
// 	sem_wait(&access_sem);
// 	enum _schedule_kernel ret = kernel;
// 	sem_post(&access_sem);
// 	return ret;
// }
unsigned long Scratch::getDuration(){
	sem_wait(&access_sem);
	unsigned long ret = duration;
	sem_post(&access_sem);
	return ret;
}

// double Scratch::getExeFactor(){
// 	sem_wait(&access_sem);
// 	double ret = exefactor;
// 	sem_post(&access_sem);
// 	return ret;
// }

// double Scratch::getBFactor(){
// 	sem_wait(&access_sem);
// 	double ret = bfactor;
// 	sem_post(&access_sem);
// 	return ret;
// }





#include "configuration/Scratch.h"


#include "utils/utils.h"

#include <iostream>
#include <cstdlib>

using namespace std;


string 						Scratch::name;
int 						Scratch::nstage;
unsigned long 				Scratch::duration;
unsigned long 				Scratch::adaption_period;
sem_t 						Scratch::access_sem;
bool 						Scratch::isSave;
string 						Scratch::benchmark;
vector<_task_periodicity> 	Scratch::periodicities;
vector<_task_type> 			Scratch::task_types;
vector<task_data>  			Scratch::all_task_data;

vector<struct timespec> 	Scratch::WCETS;

bool 					Scratch::fixedFrequency;
bool 					Scratch::fixedActive;
bool 					Scratch::is_staticApproach;
vector<struct timespec> 	Scratch::PBOO_tons;
vector<struct timespec> 	Scratch::PBOO_toffs;


void Scratch::initialize(int _nstage,
	unsigned long _duration,
	string _name){

	nstage 			= _nstage;
	duration 		= _duration;
	name            = _name;
	adaption_period = 50000;
	isSave 			= true;
	benchmark 		= "default";
	fixedFrequency  = false;
	fixedActive 	= false;
	is_staticApproach= true;


	WCETS = vector<struct timespec> (nstage, TimeUtil::Micros(50000));
	PBOO_tons = vector<struct timespec> (nstage, TimeUtil::Micros(100000));
	PBOO_toffs = vector<struct timespec> (nstage, TimeUtil::Micros(10000));

	sem_init(&access_sem, 0, 1);
}

void Scratch::print(){
	cout << "nstage \t\t\t= " << nstage << endl;
	cout << "duration \t\t= " << duration << endl;
	cout << "name \t\t\t= " << name << endl;
	cout << "adaption_period \t= " << adaption_period << endl;

	for (int i = 0; i < (int)all_task_data.size(); ++i)
	{	
		task_data_print(all_task_data[i]);
		displayvector(periodicities, "periodicities");
		displayvector(task_types, "task_types");
	}
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

void Scratch::setWCETs(vector<struct timespec> wcets){
	sem_wait(&access_sem);
	WCETS = wcets;
	sem_post(&access_sem);
}


void Scratch::setFixedFrequency(bool ff){
	sem_wait(&access_sem);
	fixedFrequency = ff;
	sem_post(&access_sem);	
}
void Scratch::setFixedActive(bool fa){
	sem_wait(&access_sem);
	fixedActive = fa;
	sem_post(&access_sem);
}
void Scratch::setStaticApproach(bool sa){
	sem_wait(&access_sem);
	is_staticApproach = sa;
	sem_post(&access_sem);
}
void Scratch::setPBOOTons(std::vector<struct timespec> tons){
	sem_wait(&access_sem);
	PBOO_tons = tons;
	sem_post(&access_sem);
}
void Scratch::setPBOOToffs(std::vector<struct timespec> toffs){
	sem_wait(&access_sem);
	PBOO_toffs = toffs;
	sem_post(&access_sem);
}



bool Scratch::isFixedFrequency(){
	sem_wait(&access_sem);
	bool ret = fixedFrequency;
	sem_post(&access_sem);
	return ret;
}
bool Scratch::isFixedActive(){
	sem_wait(&access_sem);
	bool ret = fixedActive;
	sem_post(&access_sem);
	return ret;
}
bool Scratch::isStaticApproach(){
	sem_wait(&access_sem);
	bool ret = is_staticApproach;
	sem_post(&access_sem);
	return ret;
}

std::vector<struct timespec> Scratch::getPBOOTons(){
	sem_wait(&access_sem);
	vector<struct timespec> ret = PBOO_tons;
	sem_post(&access_sem);
	return ret;
}
std::vector<struct timespec> Scratch::getPBOOToffs(){
	sem_wait(&access_sem);
	vector<struct timespec> ret = PBOO_toffs;
	sem_post(&access_sem);
	return ret;
}



vector<struct timespec> Scratch::getWCETs(){
	sem_wait(&access_sem);
	vector<struct timespec> ret = WCETS;
	sem_post(&access_sem);
	return ret;
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

task_data  Scratch::getTaskData(_task_type type){
	int id = 0;
	bool found = false;
	for (int i = 0; i < (int) task_types.size(); ++i)
	{
		if(task_types[i] == type){
			id = i;
			found = true;
			break;
		}
	}
	if(!found){
		cout << "Scratch::getTaskData: cannot found task data for given task type" << endl;
		exit(1);
	}
	return all_task_data[id];
}


std::string  Scratch::getApproachName(){
	return "test";
}


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



string Scratch::getName(){
	sem_wait(&access_sem);
	string ret = name;
	sem_post(&access_sem);
	return ret;
}
int Scratch::getNcores(){
	sem_wait(&access_sem);
	int ret = nstage;
	sem_post(&access_sem);
	return ret;
}

unsigned long Scratch::getDuration(){
	sem_wait(&access_sem);
	unsigned long ret = duration;
	sem_post(&access_sem);
	return ret;
}


void task_data_print(const task_data & data){
	cout << "task name: " << data.name << endl;
	cout << "task Id: " << data.taskId << endl;
	cout << "period: " << TimeUtil::convert_ms(data.period) << endl;
	cout << "jitter: " << TimeUtil::convert_ms(data.jitter) << endl;
	cout << "release_time: " << TimeUtil::convert_ms(data.release_time) << endl;
	
	cout << "benchmark: " << data.benchmark << endl;
	displayvector(data.attached_cores, "attached_cores");

	displayvector(TimeUtil::convert_us(data.wcets), "wcets");
}



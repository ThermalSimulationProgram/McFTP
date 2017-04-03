#include "Job.h"

#include <stdlib.h>
#include <iostream>

using namespace std;

Job::Job(unsigned _nstage, unsigned _id, unsigned long _rltDeadline, 
	unsigned long _rltReleaseTime){
	nstage      = _nstage;
	id          = _id;
	cstage      = 0;
	abet        = 0;
	releaseTime = -1;
	absDeadline = -1;
	deadline 	= _rltDeadline;
	rltReleaseTime = _rltReleaseTime;
	// cout << "Job::Job: " << rltReleaseTime << endl;
	rltDeadline = rltReleaseTime + deadline;
	sem_init(&state_sem, 0, 1);
}

int Job::setRCET(vector<unsigned long> wcets, float exe_factor){
	if (wcets.size() != nstage){
		cerr << "Job::setRCET: input wcets doesn't match stage number\n";
		exit(1);
	}
	
	if (exe_factor>1 || exe_factor <=0){
		cerr << "Job::setRCET: input exe_factor out of range\n";
		exit(1);
	}

	sem_wait(&state_sem);
	rcet.clear();
	loads.clear();
	
	for (unsigned i = 0; i < wcets.size(); ++i){
		float min    = ((float)wcets[i]) * exe_factor;
		float max    = (float)wcets[i];
		float lambda = ((double) rand()/ (RAND_MAX));
		unsigned long r;
		r            =  (unsigned long) (min  * lambda + max * (1-lambda) + 0.5);
		rcet.push_back(r);
		loads.push_back(r);
	}
	sem_post(&state_sem);
	return 0;
}

int Job::execute(unsigned long length){
	if(length==0)
		return 0;
	
	if (isFinished())
		return -1;
	
	sem_wait(&state_sem);
	if (length < loads[cstage]){
		loads[cstage] = loads[cstage] - length;
		abet          = abet + length;
		sem_post(&state_sem);
		return 0;
	}
	else {
		loads[cstage] = 0;
		abet          = rcet[cstage];
		sem_post(&state_sem);
		return 1;
	}
}

unsigned long Job::getABET(){
	unsigned long ret;
	sem_wait(&state_sem);
	ret = abet;
	sem_post(&state_sem);
	return ret;
}
bool Job::isFinished(){
	bool ret;
	sem_wait(&state_sem);
	ret = (loads[cstage] == 0);
	sem_post(&state_sem);
	return ret;
}

bool Job::isFinalFinished(){
	bool ret;
	sem_wait(&state_sem);
	ret = (nstage==cstage+1 && loads.back()==0);
	sem_post(&state_sem);
	return ret;
}

int Job::joinStage(unsigned stage){
	
	if(stage >= nstage)
		return -1;
	
	sem_wait(&state_sem);
	if (stage > 0 && loads[stage-1]>0){
		sem_post(&state_sem);
		return -2;
	}

	abet   = 0;
	cstage = stage;
	sem_post(&state_sem);
	return 0;
}

unsigned Job::getId(){
	sem_wait(&state_sem);
	unsigned ret = id;
	sem_post(&state_sem);
	return ret;
	
}

unsigned long Job::getLoad(){
	sem_wait(&state_sem);
	unsigned long ret = loads[cstage-1];
	sem_post(&state_sem);
	return ret;
	
}


void Job::release(unsigned long releasetime){
	sem_wait(&state_sem);
	releaseTime = releasetime;
	absDeadline = releasetime + deadline;
	sem_post(&state_sem);
}

unsigned long Job::getAbsDeadline(){
	sem_wait(&state_sem);
	unsigned long ret = absDeadline;
	sem_post(&state_sem);
	return ret;
}


unsigned long Job::getRltDeadline(){
	sem_wait(&state_sem);
	unsigned long ret = rltDeadline;
	sem_post(&state_sem);
	return ret;
}


unsigned long Job::getRltReleaseTime(){
	sem_wait(&state_sem);
	unsigned long ret = rltReleaseTime;
	// cout << "Job::getRltReleaseTime: " << ret << endl;
	sem_post(&state_sem);
	return ret;
}
#include "core/TaskLoad.h"

#include <iostream>

TaskLoad::TaskLoad(){
	sem_init(&suspend_sem, 0, 0);
	sem_init(&resume_sem, 0, 0);
	sem_init(&stop_sem, 0, 0);

	wcet_us = 0;
	
	sleepEnd.tv_sec = 0;
    sleepEnd.tv_nsec = 0;

    TASKLOADCOUNTER = 0;
    TASKLOADSTOPCOUNTER = 0;
}

TaskLoad::~TaskLoad(){}

void TaskLoad::LoadsInterface(unsigned long _wcet_us){
	initCheckCounter();
	runLoads(_wcet_us);
}


void TaskLoad::runLoads(unsigned long _wcet_us){

	std::cout << "TaskLoad::runLoads: This should not print!\n";

}

void TaskLoad::suspend(const struct timespec& _sleepEndTime){
	sleepEnd = _sleepEndTime;
	sem_post(&suspend_sem);
}

void TaskLoad::resume(){
	sem_post(&resume_sem);
}


void TaskLoad::stop(){
	sem_post(&stop_sem);
}
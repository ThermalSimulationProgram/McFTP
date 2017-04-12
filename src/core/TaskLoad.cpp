#include "core/TaskLoad.h"

#include <iostream>

TaskLoad::TaskLoad(){
	sem_init(&suspend_sem, 0, 0);
	sem_init(&resume_sem, 0, 0);
	sem_init(&stop_sem, 0, 0);

	wcet_us = 0;
	
	sleepLength.tv_sec = 0;
    sleepLength.tv_nsec = 0;
}

TaskLoad::~TaskLoad(){}

void TaskLoad::runLoads(unsigned long wcet_us){

	std::cout << "TaskLoad::runLoads: This should not print!\n";

}

void TaskLoad::suspend(const struct timespec& length){
	sleepLength = length;
	sem_post(&suspend_sem);
}

void TaskLoad::resume(){
	sem_post(&resume_sem);
}


void TaskLoad::stop(){
	sem_post(&stop_sem);
}
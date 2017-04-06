#include "core/Task.h"

#include <iostream>


// #include "core/Worker.h"


long Task::jobCounter = 0;

using namespace std;

/***************************************
 *        CLASS DEFINITION             * 
 ***************************************/

/*********** CONSTRUCTOR ***********/

Task::Task(_task_type load) {
  task_load = load;
  jobId = jobCounter;
  jobCounter++;
  worker = NULL;

  sem_init(&suspend_sem, 0, 0);
  sem_init(&resume_sem, 0, 0);
}

/*********** MEMBER FUNCTIONS ***********/

void Task::setWorker(Worker* w){
	worker = w;
}

///This function performs one task
void Task::fire() {
  cout << "Task::fire - This should not print!\n";
}

int Task::getId(){
	return jobId;
}


_task_type Task::getType(){
	return task_load;
}

void Task::suspend(const struct timespec& length){
	sleepLength = length;
	sem_post(&suspend_sem);

}

void Task::resume(){
	sem_post(&resume_sem);
}



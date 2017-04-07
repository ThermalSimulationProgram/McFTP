#include "configuration/JobQueue.h"

#include <iostream>
#include <stdlib.h>

#include "core/Task.h"

using namespace std;
#define _DEBUG 0
#define _INFO 0

/***************************************
 *        CLASS DEFINITION             * 
 ***************************************/

/*********** CONSTRUCTOR ***********/
///Contructor needs nothing
JobQueue::JobQueue() {

  	sem_init(&queue_sem, 0, 1); //mutex semaphore
}

JobQueue::JobQueue(const JobQueue& j){
	sem_init(&queue_sem, 0, 1);
}

///destructor
JobQueue::~JobQueue(){
	
}


/*********** PUBLIC MEMBER FUNCTIONS ***********/


///This function is a public wrapper to the protectedInsertRunnable function
void JobQueue::insertJob(Task* newTask) {
	

	if(newTask == NULL) {
		cout << "JobQueue::insertJob() - newTask is null!\n";
		return;
	}
	sem_wait(&queue_sem);
	taskPointers.push_back(newTask);
	sem_post(&queue_sem); 
}

///This function reads the tail of the queue (non-destructive read), 
///and returns the pointer to the task
Task* JobQueue::peek_back() {
	if(getSize() == 0)
		return NULL;

	return taskPointers[getSize()-1];
}

///This function reads the head of the queue (non-destructive read) 
///and returns a pointer to the task
Task* JobQueue::peek_front() {
	if(getSize() == 0)
		return NULL;

	return taskPointers[0];
}

///This function reads the head of the queue, erases it from the JobQueue, 
///and returns the head pointer
Task* JobQueue::pop_front() {
	Task* ret;
	if(getSize() == 0)
		return NULL;
	sem_wait(&queue_sem);
	ret = taskPointers[0];
	taskPointers.pop_front();
	sem_post(&queue_sem);
	return ret;
}

/*********** GETTER FUNCTIONS ***********/

///This function returns the size of the JobQueue
int JobQueue::getSize() {
	return (int)taskPointers.size();
}


void JobQueue::print(){

	if(getSize()>0) {
		for (int i = 0; i < getSize(); ++i)
		{
			cout << "JobQueue::print: task with id: "<< taskPointers[i]->getId() << endl;
		}
	}
  //if head is NULL, there is nothing to do
	else {
		cout << "JobQueue::print: empty queue" << endl;
	}

}
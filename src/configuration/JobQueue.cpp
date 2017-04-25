#include "configuration/JobQueue.h"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <stdlib.h>

#include "core/Task.h"


#define _DEBUG 0
#define _INFO 0


using namespace std;

/*********** CONSTRUCTOR ***********/
///Contructor needs nothing
JobQueue::JobQueue(){
	//mutex semaphore
	sem_init(&queue_sem, 0, 1); 
}


JobQueue::JobQueue(const JobQueue& j){
	sem_init(&queue_sem, 0, 1);
	taskPointers.clear();
	std::list<Task*>::const_iterator it = j.taskPointers.begin();
	for (; it != j.taskPointers.end(); it++)
	{
		taskPointers.push_back(*it);
	}

}


///destructor
JobQueue::~JobQueue(){}

/*********** PROTECTED MEMBER FUNCTIONS ***********/
std::list<Task*>::iterator JobQueue::protectedGetIteratorAt(int position){
	std::list<Task*>::iterator it = taskPointers.begin();
	if (position > (int) taskPointers.size() ){
		position = (int)taskPointers.size();
	}
	std::advance(it, position);
	return it;
}

/*********** PUBLIC MEMBER FUNCTIONS ***********/
bool JobQueue::deleteJob(Task* _task){
	bool ret = false;
	sem_wait(&queue_sem);
	std::list<Task*>::iterator it = std::find(taskPointers.begin(), 
		taskPointers.end(), _task);
	if (it != taskPointers.end()){
		taskPointers.erase(it);
		ret = true;
	}
	
	sem_post(&queue_sem);
	return ret;
}



bool JobQueue::deleteJobAt(int position){
	if (position < 0 || position >= (int) taskPointers.size()){
		return false;
	}
	sem_wait(&queue_sem);
	std::list<Task*>::iterator it = protectedGetIteratorAt(position);
	taskPointers.erase(it);
	sem_post(&queue_sem);
	return true;
}


///This function is a public wrapper to the protectedInsertRunnable function
void JobQueue::insertJob(Task* newTask){
	if(newTask == NULL){
		cout << "JobQueue::insertJob() - newTask is null!\n";
		return;
	}
	sem_wait(&queue_sem);
	taskPointers.push_back(newTask);
	sem_post(&queue_sem);
}


void JobQueue::insertJobAt(int position, Task* newTask){
	if (position < 0){
		return;
	}
	if(newTask == NULL) {
		cout << "JobQueue::insertJobAt() - newTask is null!\n";
		return;
	}

	sem_wait(&queue_sem);
	std::list<Task*>::iterator it = protectedGetIteratorAt(position);
	taskPointers.insert(it, newTask);
	sem_post(&queue_sem);
}


int JobQueue::findJob(Task* _task){
	int ret;
	sem_wait(&queue_sem);
	std::list<Task*>::iterator it = std::find(taskPointers.begin(), 
		taskPointers.end(), _task);
	ret = (int) std::distance(taskPointers.begin(), it);
	if (ret >= (int)taskPointers.size()){
		ret = -1;
	}
	sem_post(&queue_sem);
	return ret;
}

std::vector<Task*> JobQueue::peekAllTasks(){
	vector<Task*> ret;
	sem_wait(&queue_sem);
	ret.reserve(taskPointers.size());
	for (std::list<Task*>::iterator it = taskPointers.begin(); 
	 		it != taskPointers.end() ; it++)
	{
		ret.push_back(*it);
	}

	sem_post(&queue_sem);
	return ret;
}

Task* JobQueue::peek_at(int position){
	Task* ret = NULL;
	sem_wait(&queue_sem);
	if (position >= 0 && position < (int)taskPointers.size()){
		std::list<Task*>::iterator it = protectedGetIteratorAt(position);
		ret = *it;
	}
	sem_post(&queue_sem);

	return ret;

}

Task* JobQueue::pop_at(int position){
	Task* ret = NULL;
	sem_wait(&queue_sem);
	if (position >= 0 && position < (int)taskPointers.size()){
		std::list<Task*>::iterator it = protectedGetIteratorAt(position);
		ret = *it;
		taskPointers.erase(it);
	}
	sem_post(&queue_sem);

	return ret;

}


///This function reads the tail of the queue (non-destructive read), 
///and returns the pointer to the task
Task* JobQueue::peek_back() {
	Task* ret;
	sem_wait(&queue_sem);
	if (taskPointers.size() == 0){
		ret = NULL;
	}else{
		ret = taskPointers.back();
	}
	sem_post(&queue_sem);

	return ret;
}

///This function reads the head of the queue (non-destructive read) 
///and returns a pointer to the task
Task* JobQueue::peek_front() {
	Task* ret;
	sem_wait(&queue_sem);
	if (taskPointers.size() == 0){
		ret = NULL;
	}else{
		ret = taskPointers.front();
	}
	sem_post(&queue_sem);

	return ret;
}

///This function reads the head of the queue, erases it from the JobQueue, 
///and returns the head pointer
Task* JobQueue::pop_front() {
	Task* ret;
	sem_wait(&queue_sem);
	if (taskPointers.size() == 0){
		ret = NULL;
	}else{
		ret = taskPointers.front();
		taskPointers.pop_front();
	}
	sem_post(&queue_sem);
	return ret;
}

/*********** GETTER FUNCTIONS ***********/

///This function returns the size of the JobQueue
int JobQueue::getSize() {
	int ret;
	sem_wait(&queue_sem);
	ret = (int)taskPointers.size();
	sem_post(&queue_sem);
	return ret;
}


void JobQueue::print(){
	// cout << "=========BEGIN OF PRINT JOBQUEUE============"<< endl;
	sem_wait(&queue_sem);
	if( taskPointers.size() > 0) {
		for (std::list<Task*>::iterator it = taskPointers.begin(); it != taskPointers.end(); it++)
		{
			cout << "[Task: "<< (*it)->getTaskId() << " Job: " <<  (*it)->getId() << "] " ;
		}
	}
  // there is nothing to do
	else {
		cout << "JobQueue::print: empty queue" << endl;
	}
	sem_post(&queue_sem);
	// cout << "=========END OF PRINT JOBQUEUE============"<< endl;
}
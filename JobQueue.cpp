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
	size = 0;
	head = NULL;
	tail = NULL;
  	sem_init(&queue_sem, 0, 1); //mutex semaphore
  }

  JobQueue::JobQueue(const JobQueue& j){
  	sem_init(&queue_sem, 0, 1);
  	size = 0;
  	head = NULL;
  	tail = NULL;

  	Node* p = j.head;
  	while (p != NULL){
  		insertJob(p.task);
  		p = p.next;
  	}
  }

///destructor
  JobQueue::~JobQueue(){
	// if the link list is not empty
  	if(head != NULL){
  		Node* current = head;
  		Node* next;

  		while (current != NULL){
  			next = current->next;
  			delete(current);
  			current = next;
  		}
  	}
  }

bool findJobInfo(Task* _task, Node* theNode, int* position){
	bool ret;
	sem_wait(&queue_sem);
	//if head is NULL, there is nothing to do
	if (head == NULL){
		ret = false;
	}else{
		//If task is the head, delete the head
		if(head->task = _task) {
			theNode = head;
			ret = true;
		}else{
			//iterate to find task and delete it
			Node *aux = head->next, *prev = head;
			ret = false; 
			while(aux != NULL) {
				if(aux->task = _task) {
					theNode = aux;
					ret = true ;
					break;
				}
				prev = aux;
				aux = aux->next;
			}
		}
	}

	sem_post(&queue_sem);
	return ret;
}



/*********** PUBLIC MEMBER FUNCTIONS ***********/

bool JobQueue::deleteJob(Task* _task) {
	bool ret;
	sem_wait(&queue_sem);

	//if head is NULL, there is nothing to do
	if (head == NULL){
		ret = false;
	}else{
		//If task is the head, delete the head
		if(head->task = _task) {
			Node *aux = head;
			head = head->next;
			delete(aux);

			size--;
			ret = true;
		}else{
		//iterate to find task and delete it
			Node *aux = head->next, *prev = head;
			ret = false; 
			while(aux != NULL) {
				if(aux->task = _task) {
					if(aux == tail) {
						tail = prev;
					}
					prev->next = aux->next;
					delete(aux);
					size--;
					ret = true ;
				}
				prev = aux;
				aux = aux->next;
			}
		}
	}

	sem_post(&queue_sem);
	return ret;
}


///This function is a public wrapper to the protectedInsertRunnable function
 void JobQueue::insertJob(Task* newTask) {


  	if(newTask == NULL) {
  		cout << "JobQueue::insertJob() - newTask is null!\n";
  		return;
  	}
  	sem_wait(&queue_sem);

	//increase the size counter
  	size++;
  	#if _INFO==1
  	cout << "JobQueue::insertJob() - size is now: " << size << endl;
  	#endif

  	//Base case, the list was empty. The task is now head and tail of queue
  	if (head == NULL) {
  		head = (Node*) malloc(sizeof(Node));
  		head->task = newTask;
  		tail = head;
  		tail->next = NULL;

    	#if _DEBUG==1
  		cout << "New Head1: " << newTask->getId() << endl;
    	#endif
  	}else{
  		//create new node
  		Node* newNode = (Node*) malloc(sizeof(Node));
  		newNode->task = newTask;
  		newNode->next = NULL;

    	//link the old tail to the new node
  		tail->next = newNode;
    	//move the tail
  		tail = newNode;

    	#if _DEBUG==1
  		cout << "New tail: " << newTask->getId() << endl;
    	#endif
  	}
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
  	// create the new node
	Node* newNode = (Node* ) malloc(sizeof(Node));
	newNode->task = newTask;
	newNode->next = NULL;

	sem_wait(&queue_sem);
	if (size == 0){
		head = newNode;
		tail = head;
	}else{
			// insert at head
		if (position == 0){
			// link to the old head
			newNode->next = head;
			head = newNode;
		}else{
			// traverse the linked list
			Node* current = head, *prev = NULL;
			int counter = 1;

			while(current != NULL && counter != position){
				prev = current;
				current = current->next;
				counter++;
			}
			// if reach the tail
			if (prev == tail){
				tail = newNode;
			}

			prev.next = newNode;
			newNode.next = current;
		}

	}
	size++;
	sem_post(&queue_sem);
}


int findJob(Task* task){

}

///This function reads the tail of the queue (non-destructive read), 
///and returns the pointer to the task
Task* JobQueue::peek_back() {
  	if(getSize() == 0)
  		return NULL;

  	return tail.task;
  }

///This function reads the head of the queue (non-destructive read) 
///and returns a pointer to the task
Task* JobQueue::peek_front() {
  	if(getSize() == 0)
  		return NULL;

  	return head.task;
  }

///This function reads the head of the queue, erases it from the JobQueue, 
///and returns the head pointer
Task* JobQueue::pop_front() {
  	Task* ret;
  	if(getSize() == 0)
  		return NULL;
  	sem_wait(&queue_sem);
  	ret = head.task;
  	deleteJob(ret);
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
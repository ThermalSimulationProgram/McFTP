#include "configuration/JobQueue.h"

#include <iostream>
#include <stdlib.h>

#include "core/Task.h"

using namespace std;

/***************************************
 *        CLASS DEFINITION             * 
 ***************************************/

/*********** CONSTRUCTOR ***********/
///Contructor needs nothing
JobQueue::JobQueue() {
	head = NULL;
	tail = NULL;
	size = 0;

  sem_init(&queue_sem, 0, 1); //mutex semaphore
}

///destructor delete the link list
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

/*********** PROTECTED MEMBER FUNCTIONS ***********/

///This function removes the queue's pointer to the Task with the specified type and id. 
///returns true if the task was found, false otherwise
bool JobQueue::protectedDeleteTask(_task_type type, unsigned int id) {
	if(head!=NULL) {
    //If task is the head, delete the head
		if(head->r->getId()==id && head->r->getType()==type) {
			Node *aux = head;
			head = head->next;
			delete(aux);

			size--;
			return true;
		}
	}
  //if head is NULL, there is nothing to do
	else {
		return false;
	}

	Node *aux = head->next, *prev=head;

  //iterate to find task and delete it
	while(aux!=NULL) {
		if(aux->r->getId() == id && aux->r->getType()==type) {
			if(aux == tail) {
				tail = prev;
			}
			prev->next = aux->next;
			delete(aux);
			size--;
			return true ;
		}
		prev = aux;
		aux = aux->next;
	}

	return false;  
}

///This function inserts the new task in the queue 
bool JobQueue::protectedInsertTask(Task* newTask) {
    //increase the size counter
	size++;

  #if _INFO==1
	cout << "JobQueue::protectedInsertRunnable() - size is now: " << size << endl;
  #endif

  //Base case, the list was empty. The task is now head and tail of queue
	if (head == NULL) {
		head = (Node*) malloc(sizeof(Node));
		head->r = newTask;
		tail = head;
		tail->next = NULL;

    #if _DEBUG==1
		cout << "New Head1: " << newTask->getId() << endl;
    #endif
		return true;
	}else{
  	//create new node
		Node* newNode = (Node*) malloc(sizeof(Node));
		newNode->r = newTask;
		newNode->next = NULL;

    //link the old tail to the new node
		tail->next = newNode;
    //move the tail
		tail = newNode;

    #if _DEBUG==1
		cout << "New tail: " << newTask->getId() << endl;
    #endif
		return false;
	}

}

/*********** PUBLIC MEMBER FUNCTIONS ***********/

///This function is a public wrapper to the protectedDeleteRunnable function
bool JobQueue::deleteTask(_task_type type, unsigned int id) {
	bool ret;

	sem_wait(&queue_sem);
	ret = protectedDeleteRunnable(type, id);
	sem_post(&queue_sem);

	return ret;
}

///This function is a public wrapper to the protectedInsertRunnable function
bool JobQueue::insertRunnable(Task* newTask) {
	bool ret;

	if(newTask == NULL) {
		cout << "JobQueue::insertRunnable() - newTask is null!\n";
		return false;
	}

	sem_wait(&queue_sem);
    //Clear the queue of any nodes with this task           
	protectedDeleteTask(newTask->getType(), newTask->getId());
    //Insert into the queue
	ret = protectedInsertTask(newTask);
    //cout << size << endl;
	sem_post(&queue_sem);
	return ret;  
}

///This function reads the tail of the queue (non-destructive read), 
///and returns the pointer to the task
Task* JobQueue::peek_back() {
	if(tail==NULL)
		return NULL;

	return tail->r;
}

///This function reads the head of the queue (non-destructive read) 
///and returns a pointer to the task
Task* JobQueue::peek_front() {
	if(head==NULL)
		return NULL;

	return head->r;
}

///This function reads the head of the queue, erases it from the JobQueue, 
///and returns the head pointer
Task* JobQueue::pop_front() {
	if(head==NULL)
		return NULL;

	Task* aux = head->r;

	deleteTask(aux->getType(), aux->getId());

	return aux;
}

/*********** GETTER FUNCTIONS ***********/

///This function returns the size of the JobQueue
int JobQueue::getSize() {
	return size;
}
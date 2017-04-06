#ifndef JOBQUEUE_H
#define JOBQUEUE_H




#include <semaphore.h>

#include "utils/Enumerations.h"


class Task;

/***************************************
 *        CLASS DECLARATION            * 
 ***************************************/

///This structure is the basis for the linked list of JObQueue. 
///It holds a pointer to the Task, and a pointer to the next Node.
typedef struct _Node {
	Task *r;
	struct _Node *next;
} Node;

class JobQueue {

protected:

 /*********** VARIABLES ***********/

 	///This variable holds the size of the queue
	int size;

  	///The head of the queue points the current job. 
	Node* head;

  	///The tail of the queue points to the last job on the active queue.
	Node* tail;

  	///This semaphore protects access to the linked list (for insertion and removal)
	sem_t queue_sem;

public:

  	/*********** CONSTRUCTOR ***********/

  	///Contructor needs nothing
	JobQueue();
	///Destructor
	~JobQueue();

	JobQueue(const JobQueue& j);

protected:

  /*********** PROTECTED MEMBER FUNCTIONS ***********/

  	///This function removes the queue's pointer to the Task with the specified type and id. 
 	///returns true if the task was found, false otherwise
	bool protectedDeleteJob(_task_type type, int id);

  	///This function inserts the new task in the queue
	bool protectedInsertJob(Task* newTask);

public:

  /*********** PUBLIC MEMBER FUNCTIONS ***********/

  	///This function is a public wrapper to the protectedDeleteJob function
	bool deleteJob(_task_type type, int id);

  	///This function is a public wrapper to the protectedInsertJob function
	bool insertJob(Task* newTask);

  	///This function reads the head of the queue (non-destructive read) 
	///and returns a pointer to the task
	Task* peek_front();

  
	///This function reads the tail of the queue (non-destructive read), 
	///and returns the pointer to the task
	Task* peek_back();

  	///This function reads the head of the queue, erases it from the JobQueue, 
	///and returns the head pointer
	Task* pop_front();

  /*********** GETTER FUNCTIONS ***********/

  ///This function returns the size of the JobQueue
	int getSize();
};

#endif


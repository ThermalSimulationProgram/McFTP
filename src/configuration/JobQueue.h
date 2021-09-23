#ifndef JOBQUEUE_H
#define JOBQUEUE_H

#include <semaphore.h>
#include <vector>
#include <list>

#include "utils/Enumerations.h"


class Task;

/***************************************
*        CLASS DECLARATION            *
***************************************/

class JobQueue {
protected:

   /*********** VARIABLES ***********/

   // This linked list stores pointers to the jobs.
   std::list <Task *> taskPointers;

   ///This semaphore protects access to the linked list (for insertion and removal)
   sem_t queue_sem;

public:

   /*********** CONSTRUCTOR ***********/
   ///Contructor needs nothing
   JobQueue();

   ///Destructor
   ~JobQueue();

   /// Copy constructor
   JobQueue(const JobQueue& j);

protected:
   // get the iterator of the job at given position
   std::list <Task *>::iterator protectedGetIteratorAt(int position);

public:

   /*********** PUBLIC MEMBER FUNCTIONS ***********/

   ///This function removes the queue's pointer to the job with the specified task pointer.
   ///returns true if the task was found, false otherwise
   bool deleteJob(Task *_task);

   ///This function removes the queue's pointer to the job stored at given position.
   ///The position of a node equals the distance between itself and the begin node.
   bool deleteJobAt(int position);

   ///This function inserts new job in the queue at the tail
   void insertJob(Task *newTask);

   ///This function inserts new job in the queue at given position
   void insertJobAt(int position, Task *newTask);

   ///This function find the position pointed by task, returns the distance to
   // the begin node if found, -1 otherwise.
   int findJob(Task *task);

   // This function copies all the task pointers and saves them in a vector
   std::vector <Task *> peekAllTasks();

   ///This function reads the given position of the queue (non-destructive read),
   ///and returns the pointer to the task
   Task *peek_at(int position);

   ///This function reads the given position of the queue, erases it from the JobQueue,
   ///and returns the pointer to the task
   Task *pop_at(int position);

   ///This function reads the head of the queue (non-destructive read)
   ///and returns a pointer to the task
   Task *peek_front();

   ///This function reads the tail of the queue (non-destructive read),
   ///and returns the pointer to the task
   Task *peek_back();

   ///This function reads the head of the queue, erases it from the JobQueue,
   ///and returns the head pointer
   Task *pop_front();

   /*********** GETTER FUNCTIONS ***********/

   ///This function returns the size of the JobQueue
   int getSize();

   /// This function prints the queue
   void print();
};

#endif

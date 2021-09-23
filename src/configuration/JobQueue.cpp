#include "configuration/JobQueue.h"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <stdlib.h>

#include "core/Task.h"

#define _DEBUG    0
#define _INFO     0


using namespace std;

/*********** CONSTRUCTOR ***********/
///Contructor needs nothing
JobQueue::JobQueue()
{
   //mutex semaphore
   sem_init(&queue_sem, 0, 1);
}

/// Copy constructor
JobQueue::JobQueue(const JobQueue& j)
{
   // reset the semaphore
   sem_init(&queue_sem, 0, 1);
   // reset the linked list
   taskPointers.clear();
   // copy all the task pointers
   std::list <Task *>::const_iterator it = j.taskPointers.begin();

   for (; it != j.taskPointers.end(); it++)
   {
      taskPointers.push_back(*it);
   }
}

///destructor
JobQueue::~JobQueue()
{
}

/*********** PROTECTED MEMBER FUNCTIONS ***********/
// get the iterator of the job at given position
std::list <Task *>::iterator JobQueue::protectedGetIteratorAt(int position)
{
   // start from the begin
   std::list <Task *>::iterator it = taskPointers.begin();

   // truncate the inputed position if it is larger than the size
   if (position > (int)taskPointers.size())
   {
      position = (int)taskPointers.size();
   }
   // advance the iterator
   std::advance(it, position);
   return(it);
}

/*********** PUBLIC MEMBER FUNCTIONS ***********/
///This function removes the queue's pointer to the job with the specified task pointer.
///returns true if the task was found, false otherwise
bool JobQueue::deleteJob(Task *_task)
{
   bool ret = false;

   // lock the list
   sem_wait(&queue_sem);
   std::list <Task *>::iterator it = std::find(taskPointers.begin(),
                                               taskPointers.end(), _task);

   // if find the _task, erase it
   if (it != taskPointers.end())
   {
      taskPointers.erase(it);
      ret = true;
   }
   // unlock
   sem_post(&queue_sem);
   return(ret);
}

///This function removes the queue's pointer to the job stored at given position.
///The position of a node equals the distance between itself and the begin node.
bool JobQueue::deleteJobAt(int position)
{
   // invalid input
   if (position < 0 || position >= (int)taskPointers.size())
   {
      return(false);
   }
   sem_wait(&queue_sem);
   // get the iterator
   std::list <Task *>::iterator it = protectedGetIteratorAt(position);

   taskPointers.erase(it);
   sem_post(&queue_sem);
   return(true);
}

///This function inserts new job in the queue at the tail
void JobQueue::insertJob(Task *newTask)
{
   if (newTask == NULL)
   {
                #if _DEBUG == 1
      cout << "JobQueue::insertJob() - newTask is null!\n";
                #endif
      return;
   }
   sem_wait(&queue_sem);
   taskPointers.push_back(newTask);
   sem_post(&queue_sem);
}

///This function inserts new job in the queue at given position
void JobQueue::insertJobAt(int position, Task *newTask)
{
   if (position < 0)
   {
      return;
   }
   if (newTask == NULL)
   {
                #if _DEBUG == 1
      cout << "JobQueue::insertJobAt() - newTask is null!\n";
                #endif

      return;
   }

   sem_wait(&queue_sem);
   std::list <Task *>::iterator it = protectedGetIteratorAt(position);
   taskPointers.insert(it, newTask);
   sem_post(&queue_sem);
}

///This function find the position pointed by task, returns the distance to
// the begin node if found, -1 otherwise.
int JobQueue::findJob(Task *_task)
{
   int ret;

   sem_wait(&queue_sem);
   std::list <Task *>::iterator it = std::find(taskPointers.begin(),
                                               taskPointers.end(), _task);

   ret = (int)std::distance(taskPointers.begin(), it);
   // not found
   if (ret >= (int)taskPointers.size())
   {
      ret = -1;
   }
   sem_post(&queue_sem);
   return(ret);
}

// This function copies all the task pointers and saves them in a vector
std::vector <Task *> JobQueue::peekAllTasks()
{
   vector <Task *> ret;

   sem_wait(&queue_sem);
   ret.reserve(taskPointers.size());
   // iterate all the items
   for (std::list <Task *>::iterator it = taskPointers.begin();
        it != taskPointers.end(); it++)
   {
      ret.push_back(*it);
   }

   sem_post(&queue_sem);
   return(ret);
}

///This function reads the given position of the queue (non-destructive read),
///and returns the pointer to the task
Task *JobQueue::peek_at(int position)
{
   Task *ret = NULL;

   sem_wait(&queue_sem);
   // sanity check
   if (position >= 0 && position < (int)taskPointers.size())
   {
      std::list <Task *>::iterator it = protectedGetIteratorAt(position);
      ret = *it;
   }
   sem_post(&queue_sem);

   return(ret);
}

///This function reads the given position of the queue, erases it from the JobQueue,
///and returns the pointer to the task
Task *JobQueue::pop_at(int position)
{
   Task *ret = NULL;

   sem_wait(&queue_sem);
   // sanity check
   if (position >= 0 && position < (int)taskPointers.size())
   {
      std::list <Task *>::iterator it = protectedGetIteratorAt(position);
      ret = *it;
      taskPointers.erase(it);
   }
   sem_post(&queue_sem);

   return(ret);
}

///This function reads the head of the queue (non-destructive read)
///and returns a pointer to the task
Task *JobQueue::peek_back()
{
   Task *ret;

   sem_wait(&queue_sem);
   if (taskPointers.size() == 0)
   {
      ret = NULL;
   }
   else
   {
      ret = taskPointers.back();
   }
   sem_post(&queue_sem);

   return(ret);
}

///This function reads the head of the queue (non-destructive read)
///and returns a pointer to the task
Task *JobQueue::peek_front()
{
   Task *ret;

   sem_wait(&queue_sem);
   if (taskPointers.size() == 0)
   {
      ret = NULL;
   }
   else
   {
      ret = taskPointers.front();
   }
   sem_post(&queue_sem);

   return(ret);
}

///This function reads the head of the queue, erases it from the JobQueue,
///and returns the head pointer
Task *JobQueue::pop_front()
{
   Task *ret;

   sem_wait(&queue_sem);
   if (taskPointers.size() == 0)
   {
      ret = NULL;
   }
   else
   {
      ret = taskPointers.front();
      taskPointers.pop_front();
   }
   sem_post(&queue_sem);
   return(ret);
}

/*********** GETTER FUNCTIONS ***********/

///This function returns the size of the JobQueue
int JobQueue::getSize()
{
   int ret;

   sem_wait(&queue_sem);
   ret = (int)taskPointers.size();
   sem_post(&queue_sem);
   return(ret);
}

void JobQueue::print()
{
   // cout << "=========BEGIN OF PRINT JOBQUEUE============"<< endl;
   sem_wait(&queue_sem);
   if (taskPointers.size() > 0)
   {
      for (std::list <Task *>::iterator it = taskPointers.begin(); it != taskPointers.end(); it++)
      {
         cout << "[Task: " << (*it)->getTaskId() << " Job: " << (*it)->getId() << "] ";
      }
   }
   // there is nothing to do
   else
   {
      cout << "JobQueue::print: empty queue" << endl;
   }
   sem_post(&queue_sem);
   // cout << "=========END OF PRINT JOBQUEUE============"<< endl;
}

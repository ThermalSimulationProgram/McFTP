#ifndef _TASK_H
#define _TASK_H


#include <time.h>
#include <string>
#include <semaphore.h>

#include "utils/Enumerations.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"
#include "results/Statistics.h"
#include "core/Worker.h"

/***************************************
 *        CLASS DECLARATION            * 
 ***************************************/
 



class Task {
private:
	static long jobCounter;
 
protected:
 
  /*********** VARIABLES ***********/
  
  ///Varible describing the task's type of load (busywait, benchmarks, userdefined)
  _task_type task_load;

  // std::string name;
  int taskId;

  Worker* worker;

  // int taskId;

  int jobId;

  sem_t suspend_sem;

  sem_t resume_sem;

  struct timespec sleepLength;

  bool finished;

public:
 
  /*********** CONSTRUCTOR ***********/
  
  ///Constructor needs its load type
  Task(const _task_type & load, int _taskId);

  virtual ~Task();

  /*********** MEMBER FUNCTIONS ***********/

  void setWorker(Worker* w);

  ///This function performs one task 
  virtual void fire();

  void suspend(const struct timespec& length);

  void resume();

  int getId();

  inline void setSuspendPoint(){
    if (sem_trywait(&suspend_sem) == 0)//successfully read a suspend singal, block the executing immediately
    {
      struct timespec now = TimeUtil::getTime();
      struct timespec sleepEnd = now + sleepLength;
      int resumeVal;

      // make sure the resume semaphore is cleared
      sem_getvalue(&resume_sem, &resumeVal);
      for (int i = 0; i < resumeVal; ++i)
      {
         sem_trywait(&resume_sem);
      }
      _thread_type thread_type = _worker;
      Statistics::addTrace(thread_type, worker->getId(), sleep_start);
      // two exit conditions: reach the sleepEnd time, or recieves a resume_sem signal
      sem_timedwait(&resume_sem, &sleepEnd);
      Statistics::addTrace(thread_type, worker->getId(), sleep_end);
    }

  };

  _task_type getType();

  int getTaskId();

  bool isFinished();



};
#endif

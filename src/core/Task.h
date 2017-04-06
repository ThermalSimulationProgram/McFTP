#ifndef _TASK_H
#define _TASK_H


#include <time.h>
#include <semaphore.h>

#include "utils/Enumerations.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"

/***************************************
 *        CLASS DECLARATION            * 
 ***************************************/
 

class Worker;

class Task {
private:
	static long jobCounter;
 
protected:
 
  /*********** VARIABLES ***********/
  
  ///Varible describing the task's type of load (busywait, benchmarks, userdefined)
  _task_type task_load;

  Worker* worker;

  int jobId;

  sem_t suspend_sem;

  sem_t resume_sem;

  struct timespec sleepLength;

public:
 
  /*********** CONSTRUCTOR ***********/
  
  ///Constructor needs its load type
  Task(_task_type load);

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

      // two exit conditions: reach the sleepEnd time, or recieves a resume_sem signal
      sem_timedwait(&resume_sem, &sleepEnd);
    }

  };

  _task_type getType();



};
#endif

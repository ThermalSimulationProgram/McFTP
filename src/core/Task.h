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
#include "core/TaskLoad.h"

/***************************************
 *        CLASS DECLARATION            * 
 ***************************************/
 



class Task {
private:
	static long jobCounter;
 
protected:
 
  /*********** VARIABLES ***********/
  
  ///Varible describing the task's type of load (busywait, benchmarks, userdefined)
  _task_type task_type;
  _task_load_type task_load_type;

  //The index of this kind of task
  int taskId;

  // the worker which is currently running this task
  Worker* worker;

  int jobId;

  TaskLoad* loads;

  bool finished;

public:
 
  /*********** CONSTRUCTOR ***********/
  
  ///Constructor needs its load type
  Task(_task_type type, _task_load_type  load, int loadId, int _taskId);

  virtual ~Task();

  /*********** MEMBER FUNCTIONS ***********/

  void setWorker(Worker* w);

  ///This function performs one task 
  virtual void fire();

  void stop();

  void setLoad(TaskLoad* l);

  void suspend(const struct timespec& length);

  void resume();

  int getId();

  _task_type getType();

  int getTaskId();

  bool isFinished();

  virtual unsigned long getWCET(int id) ;



};
#endif

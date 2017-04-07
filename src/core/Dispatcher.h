#ifndef _DISPATCHER_H
#define _DISPATCHER_H

#include <time.h>
#include <vector>

#include "pthread/Thread.h"
#include "utils/Enumerations.h"
#include "core/Task.h"

class CMI;

/***************************************
 *        CLASS DECLARATION            * 
 ***************************************/

class Dispatcher : public Thread {

 protected:

  /*********** VARIABLES ***********/

  ///Periodicity of the task to be dispatched
  _task_periodicity periodicity;

  ///When this is set, the dispatcher will sleep for the specified time before beginning to dispatch
  struct timespec offset;

  ///Pointer to the CMI associated to the dispatcher
  CMI *cmi;

  std::vector<Task*> allTasks;

  const _task_type TASK_TYPE;
    
 public:

  /*********** CONSTRUCTOR ***********/

  ///Constructor needs  a disp_id and the type of the task associated with it
  Dispatcher (unsigned int id, _task_type task_type);

  ~Dispatcher();

  /*********** INHERITED FUNCTIONS ***********/

  /**** FROM THREAD ****/

  ///This join function takes into account the dispatcher's unblocking mechanism
  void join();
  
  ///This is the pthread's wrapper function, calls dispatch -> which has the flagged loop.
  void wrapper();

  /*********** MEMBER FUNCTIONS ***********/

  ///This function assignes DISP_PR to the thread
  void activate();  

  ///This function was a flagged loop that activates the Worker according to the task periodicity
  virtual void dispatch();

  /*********** SETTER FUNCTIONS ***********/

  ///This function sets the dispatcher's offset
  void setOffset(struct timespec o);

  ///This function sets the tasks's periodicity
  void setPeriodicity(_task_periodicity t);

  ///This function sets the associated CMI
  void setCMI(CMI *c);

  Task* createNewTask();
};

#endif

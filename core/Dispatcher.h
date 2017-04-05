#ifndef _DISPATCHER_H
#define _DISPATCHER_H

#include "Thread.h"
#include "Enumerations.h"

#include <time.h>

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
    
 public:
 
  ///Pointer to the CMI associated to the dispatcher
  CMI *cmi;

  /*********** CONSTRUCTOR ***********/

  ///Contructor needs a disp_id
  Dispatcher (unsigned int id);

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
};

#endif

#include "core/Dispatcher.h"

#include <time.h>
#include <iostream>

#include "core/CMI.h"
#include "pthread/Priorities.h"



#define _INFO 0

using namespace std;

/***************************************
 *        CLASS DEFINITION             * 
 ***************************************/

/*********** CONSTRUCTOR ***********/

///Constructor needs  a disp_id and the type of the task associated with it
Dispatcher::Dispatcher(unsigned int _id, _task_type task_type): 
Thread(_id), TASK_TYPE(task_type) {
  #if _INFO == 1
  cout << "++New Dispatcher - " << _id << "\n";
  #endif

  thread_type = dispatcher;

  //By default periodic
  periodicity = periodic;

  //Offset is initially 0
  offset.tv_sec = 0;
  offset.tv_nsec = 0;

  cmi = NULL;
}

/*********** INHERITED FUNCTIONS ***********/

/**** FROM THREAD ****/

///This join function takes into account the dispatcher's unblocking mechanism
void Dispatcher::join() {
 
  join2();
}

///This is the pthread's wrapper function
void Dispatcher::wrapper() {
  struct timespec rem;

  #if _INFO == 1
  cout << "Disp: " << id << " waiting for initialization\n";
  #endif

  //Wait until the CMI is initialized
  while( !CMI::isInitialized() );
  
  #if _INFO == 1
  cout << "Disp: " << id << " begining execution \n";
  #endif

  //if offset != 0, sleep before dispatching
  if(offset.tv_nsec != 0 || offset.tv_sec !=0) {
    nanosleep(&offset, &rem);
  }

  dispatch();

  #if _INFO == 1
  cout << "Dispatcher " << id << " exiting wrapper...\n";
  #endif
}

/*********** MEMBER FUNCTIONS ***********/

///This function sets the dispatcher's priority to DISP_PR
void Dispatcher::activate() {
  setPriority(Priorities::get_disp_pr());
}

///This virtual function should be implemented by the subclasses
void Dispatcher::dispatch() {
  //empty
  cout << "Dispatcher::dispatch - This should not print!\n";
}

/*********** SETTER FUNCTIONS ***********/

///This function sets the dispatcher's offset
void Dispatcher::setOffset(struct timespec o) {
  offset = o;
}

///This function sets the dispatcher's periodicity
void Dispatcher::setPeriodicity(_task_periodicity t) {
  periodicity = t;
}

///This function sets the worker
void Dispatcher::setCMI(CMI *c) {
  cmi = c;
}

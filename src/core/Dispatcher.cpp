#include "core/Dispatcher.h"

#include <time.h>
#include <iostream>


#include "core/CMI.h"
#include "core/Task.h"
#include "configuration/Scratch.h"
#include "pthread/Priorities.h"
#include "tasks/Pipelined.h"
#include "tasks/SingleCore.h"
#include "utils/TimeUtil.h"
#include "utils/vectormath.h"


#define _INFO 0

using namespace std;

/***************************************
 *        CLASS DEFINITION             * 
 ***************************************/

/*********** CONSTRUCTOR ***********/

///Constructor needs  a disp_id and the type of the task associated with it
Dispatcher::Dispatcher(unsigned int _id): 
Thread(_id){
  // #if _INFO == 1
  // cout << "++New Dispatcher - " << _id << "\n";
  // #endif

  thread_type = _dispatcher;

  //By default periodic
  periodicity = periodic;

  TASK_TYPE = singlecore;

  //Offset is initially 0
  offset.tv_sec = 0;
  offset.tv_nsec = 0;


  cmi = NULL;
  allTasks.reserve(10000);

  // ncores = taskdata.attached_cores.size();


}

Dispatcher::~Dispatcher(){
  for (int i = 0; i < (int) allTasks.size(); ++i)
  {
     delete allTasks[i];
  }
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
  sem_wait(&CMI::init_sem);
  // while( !CMI::isInitialized() );
  
  #if _INFO == 1
  cout << "Disp: " << id << " begining execution \n";
  #endif

  // while( !CMI::isRunning() );
  sem_wait(&CMI::running_sem);

  //if offset != 0, sleep before dispatching
  if(offset.tv_nsec != 0 || offset.tv_sec !=0) {
    nanosleep(&offset, &rem);
  }


  Dispatcher* disp = (Dispatcher*)this;
  disp->dispatch();

  #if _INFO == 1
  cout << "Dispatcher " << id << " exiting wrapper...\n";
  #endif
}

/*********** MEMBER FUNCTIONS ***********/

///This function sets the dispatcher's priority to DISP_PR
void Dispatcher::activate() {
  setPriority(Priorities::get_dispatcher_pr());
}

Task* Dispatcher::createNewTask(){
  Task * t = NULL;

  switch (taskdata._type){
    case pipelined:{
      Pipelined* newTask = new Pipelined(taskdata.wcets_us, 
        taskdata._load_type, taskdata.taskId);
      t = (Task*) newTask;
      break;
    }

    case singlecore:{
      SingleCore* newTask = new SingleCore(taskdata.wcets_us[0],
        taskdata._load_type, taskdata.taskId);
      t = (Task*) newTask;
      break;
    }

  }
  allTasks.push_back(t);
  return t;
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


void Dispatcher::setTaskLoadType(_task_load_type type){
  TASK_LOAD_TYPE = type;
}

void Dispatcher::setTaskType(_task_type type){
  TASK_TYPE = type;
}


///This function sets the worker
void Dispatcher::setCMI(CMI *c) {
  cmi = c;
}


void Dispatcher::setTaskData(TaskArgument d){
  taskdata = d;
  setPeriodicity(taskdata._periodicity);
  setTaskLoadType(taskdata._load_type);
  setTaskType(taskdata._type);
}
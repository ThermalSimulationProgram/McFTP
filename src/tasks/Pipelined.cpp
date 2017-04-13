#include "tasks/Pipelined.h"

#include <iostream>
#include <cstdlib>

#include "core/Dispatcher.h"
// #include "core/CMI.h"
#include "core/Worker.h"
#include "utils/Operators.h"
#include "utils/TimeUtil.h"
#include "utils/Enumerations.h"
#include "utils/vectormath.h"
#include "utils/utils.h"


using namespace std;

/***************************************
 *        CLASS DEFINITION             * 
 ***************************************/

/*********** CONSTRUCTOR ***********/

///The constructor requires a pointer to the simulation, its own dispatcher, and the WCET
Pipelined::Pipelined(const vector<unsigned long>& _wcet, _task_load_type  load,
int loadId, int taskid): Task(pipelined, load, loadId, taskid) {
  
  // coreFinished = vector<bool> (_wcet.size(), false);
  
  wcet_us = _wcet;
  nextCoreId = 0;
}

Pipelined::~Pipelined(){

}
/*********** INHERITED FUNCTIONS ***********/

/**** FROM TASK ****/

void Pipelined::fire() {
  
  int current_core_id = worker->getId();
  
  loads->runLoads(wcet_us[current_core_id]);

  if (current_core_id == (int)wcet_us.size()-1){
    finished = true;
    nextCoreId = -1;
  }else{
    nextCoreId++;
  }
}


int Pipelined::getNextCoreId(){
  return nextCoreId;
}

unsigned long Pipelined::getWCET(int id){
  return wcet_us[id];
}

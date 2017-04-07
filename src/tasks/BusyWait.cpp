#include "tasks/BusyWait.h"

#include <iostream>
#include <cstdlib>

#include "core/Dispatcher.h"
#include "core/CMI.h"
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
BusyWait::BusyWait(vector<struct timespec> wcet, vector<int> _coreIds) : Task(busywait) {
  if (wcet.size() != _coreIds.size()){
    cout << "BusyWait::BusyWait: incorrect inputs" << endl;
    exit(1);
  }

  WCET = wcet;
  coreIds = _coreIds;
  coreFinished = vector<bool> (wcet.size(), false);
  nextCoreId = 0;
}

BusyWait::~BusyWait(){

}
/*********** INHERITED FUNCTIONS ***********/

/**** FROM TASK ****/

void BusyWait::fire() {
  int current_core_id = worker->getId();
  if(current_core_id != nextCoreId ){
    cout << current_core_id << endl;
    // displayvector(coreIds, "coreIds");
    cout << "BusyWait::fire: I am being run by a wrong core!" << endl;
    return;
  }

  unsigned int count=0, wcet_us = TimeUtil::convert_us(WCET[current_core_id]);
  
  

  //BUSY WAIT LOOP
  do {
     setSuspendPoint(); // set a suspend point here. When recieves a suspend signal, pause execution here

     unsigned int start = 0, tmp_count = 0;
     start = TimeUtil::convert_us(TimeUtil::getTime());
     while(tmp_count < 100){
      tmp_count = TimeUtil::convert_us(TimeUtil::getTime()) - start;
     }

      count += tmp_count;
  } while ( count < wcet_us && CMI::isRunning()); 

  coreFinished[current_core_id] = true;

  _thread_type thread_type = _worker;
  Statistics::addTrace(thread_type, worker->getId(), task_end);

  if (vectorAll(coreFinished)){
    finished = true;
    nextCoreId = -1;
  }else{
    nextCoreId++;
  }
}


int BusyWait::getNextCoreId(){
  return nextCoreId;
}

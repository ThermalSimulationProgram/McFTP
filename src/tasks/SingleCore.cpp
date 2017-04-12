#include "tasks/SingleCore.h"

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


SingleCore::SingleCore(unsigned long _wcet_us, _task_load_type  load, int loadId, int taskid):
Task(singlecore, load, loadId, taskid){
	wcet_us = _wcet_us;

}


void SingleCore::fire(){
	 loads->runLoads(wcet_us);
	 finished = true;
}
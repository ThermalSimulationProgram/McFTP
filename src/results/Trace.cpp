#include "results/Trace.h"

#include <sstream>

#include "results/Statistics.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"
#include "utils/Semaphores.h"


using namespace std;

/***************************************
*        CLASS DEFINITION             *
***************************************/

/*********** MEMBER FUNCTIONS ***********/

///This function sets the attributes of the object
void Trace::setTrace(struct timespec ts, _thread_type tt, unsigned int ti, _task_action ta)
{
   timestamp   = ts;
   thread_type = tt;
   thread_id   = ti;
   task_action = ta;
}

///This function converts the trace to string for file output
string Trace::toString()
{
   stringstream out;

   out << TimeUtil::convert_ms(timestamp - TimeUtil::offset) << "," << thread_id << "," << task_action;

   return(out.str());
}

/*********** GETTER FUNCTIONS ***********/

///This function return the traced action
_task_action Trace::getAction()
{
   return(task_action);
}

///This function returns the thread ID
unsigned int Trace::getId()
{
   return(thread_id);
}

///This function returns the timestamp
struct timespec Trace::getTimestamp()
{
   return(timestamp);
}

///This function returns the thread type
_thread_type Trace::getType()
{
   return(thread_type);
}

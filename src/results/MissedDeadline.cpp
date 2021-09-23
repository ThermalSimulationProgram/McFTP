#include "results/MissedDeadline.h"


#include <iostream>
#include <sstream>

#include "utils/TimeUtil.h"

using namespace std;

/***************************************
*        CLASS DEFINITION             *
***************************************/

/*********** MEMBER FUNCTIONS ***********/

///This function sets the attributes of the object
void MissedDeadline::setMissedDeadline(unsigned int ti, unsigned long at, unsigned long dl, unsigned long ft)
{
   thread_id   = ti;
   arrivalTime = at;
   deadline    = dl;
   finishTime  = ft;
}

///This function converts the trace to string for file output
string MissedDeadline::toString()
{
   stringstream out;

   out << thread_id << "," << arrivalTime << "," << deadline << "," << finishTime;

   return(out.str());
}

/*********** GETTER FUNCTIONS ***********/

///This function returns the job's arrival time
unsigned long MissedDeadline::getArrivalTime()
{
   return(arrivalTime);
}

///This function returns the job's deadline
unsigned long MissedDeadline::getDeadline()
{
   return(deadline);
}

///This function returns the thread ID
unsigned int MissedDeadline::getId()
{
   return(thread_id);
}

///This function returns the jobs finish time
unsigned long MissedDeadline::getFinishTime()
{
   return(finishTime);
}

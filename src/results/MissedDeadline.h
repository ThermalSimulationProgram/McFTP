#ifndef _MISSEDDEADLINE_H
#define _MISSEDDEADLINE_H

#include <string>
#include <vector>

#include "utils/Enumerations.h"


/***************************************
*        CLASS DECLARATION            *
***************************************/

class MissedDeadline {
private:

   /*********** VARIABLES ***********/

   //Variables will be traced in the following order:

   ///This variable stores the id of the thread
   unsigned int thread_id;

   ///This variable stores the job's arrival time
   unsigned long arrivalTime;

   ///This variable stores the job's deadline
   unsigned long deadline;

   ///This variable stores the job's finish time
   unsigned long finishTime;

public:

   /*********** MEMBER FUNCTIONS ***********/

   ///This function sets the attributes of the object
   void setMissedDeadline(unsigned int ti, unsigned long at, unsigned long dl, unsigned long ft);

   ///This function converts the trace to string for file output
   std::string toString();

   /*********** GETTER FUNCTIONS ***********/

   ///This function returns the job's arrival time
   unsigned long getArrivalTime();

   ///This function returns the job's deadline
   unsigned long getDeadline();

   ///This function returns the thread ID
   unsigned int getId();

   ///This function returns the job's finish time
   unsigned long getFinishTime();
};

#endif

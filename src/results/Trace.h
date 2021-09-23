#ifndef _TRACE_H
#define _TRACE_H

#include <string>

#include "utils/Enumerations.h"

/***************************************
*        CLASS DECLARATION            *
***************************************/

class Trace {
private:

   /*********** VARIABLES ***********/
   //Variables will be traced in the following order:

   ///This variable will store the trace's timestamp
   struct timespec timestamp;

   ///This variable stores the type of thread the trace belongs to
   _thread_type thread_type;

   ///This variable store the id of the thread the trace belongs to
   unsigned int thread_id;

   ///This variable stores the action that is being traced
   _task_action task_action;

public:

   /*********** MEMBER FUNCTIONS ***********/

   ///This function sets the attributes of the object
   void setTrace(struct timespec ts, _thread_type tt, unsigned int ti, _task_action ta);

   ///This function converts the trace to string for file output
   std::string toString();

   /*********** GETTER FUNCTIONS ***********/

   ///This function return the traced action
   _task_action getAction();

   ///This function returns the thread ID
   unsigned int getId();

   ///This function returns the timestamp
   struct timespec getTimestamp();

   ///This function returns the thread type
   _thread_type getType();
};

#endif

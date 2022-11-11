#ifndef BUSYWAIT_H
#define BUSYWAIT_H



#include "core/TaskLoad.h"
#include "taskloads/MWC.h"



class BusyWait : public TaskLoad {
private:

   unsigned long count;

   MWC RND;

public:
   BusyWait();

   virtual ~BusyWait();

   // void setWCET();

   bool runLoads(unsigned long _wcet_us);
};



#endif

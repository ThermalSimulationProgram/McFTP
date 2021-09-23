#ifndef USERDEFINEDLOAD_H
#define USERDEFINEDLOAD_H


#include "core/TaskLoad.h"

class UserDefinedLoad : public TaskLoad {
private:
   unsigned long count;

   int loadId;

   void *arg;

public:
   UserDefinedLoad(int _loadId);
   ~UserDefinedLoad();

   bool runLoads(unsigned long _wcet_us);

   bool coolTask();
};



#endif

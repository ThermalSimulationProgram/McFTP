#ifndef POWERMANAGER_H
#define POWERMANAGER_H

#include <vector>
#include <string>
#include <stdio.h>
#include <semaphore.h>

#include "configuration/StateTable.h"
#include "configuration/HyperStateTable.h"
#include "pthread/Thread.h"

class Worker;

class PowerManager : public Thread {
private:
   struct timespec nextActionTime;

   std::vector <HyperStateTable> tables;

   std::vector <Worker *> workers;

   int nextCoreId;

   std::vector <std::string> freqInterface;

   sem_t interrupt_sem;

   sem_t statetable_sem;

   bool isFixedFrequency;

   bool isFixedActive;

   // std::vector<FILE*> file_d;

public:

   PowerManager(unsigned, std::vector <Worker *> workers);

   ///gives the PowerManager thread the ACTIVE_PR priority
   void activate();

   ///This join function takes into account the PowerManager's unblocking mechanism
   void join();

   ///The created thread in the object starts execution by invoking this function
   void wrapper();

   void setStateTables(const std::vector <StateTable>&);

   void setStateTable(const StateTable& t);

   void changePower(int id, double f, struct timespec);

   void setFrequency(int id, int f);
};



#endif

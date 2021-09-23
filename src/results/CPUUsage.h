#ifndef CPUUSAGE_H
#define CPUUSAGE_H


#include <string>
#include <vector>

#define  NUM_CPU_STATES    10

enum CPUStates
{
   S_USER = 0,
   S_NICE,
   S_SYSTEM,
   S_IDLE,
   S_IOWAIT,
   S_IRQ,
   S_SOFTIRQ,
   S_STEAL,
   S_GUEST,
   S_GUEST_NICE
};


typedef struct CPUData
{
   std::string cpu;
   size_t      times[NUM_CPU_STATES];
} CPUData;


class CPUUsage {
private:
   std::vector <CPUData> entries1;
   std::vector <CPUData> entries2;

   bool isStartLogging;
   bool isFinishLogging;

public:
   CPUUsage();

   void startLoggingCPU();
   void endLoggingCPU();

   float getUsage();
   float getUsage(int);

   void printInfo();
};


#endif

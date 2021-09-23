#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <complex>

using namespace std;

#include "utils/matrix_exponential.hpp"
#include "utils/test_matrix_exponential.hpp"
#include "utils/c8lib.hpp"
#include "utils/r8lib.hpp"


#include "CMI.h"
//#include "performance_counter/PerformanceCounters.h"
#include "utils/Parser.h"
#include "utils/utils.h"
#include "utils/FileOperator.h"
#include "UnitTest/UnitTest.h"


void displayAllQueues(std::vector <std::vector <Task *> > queues)
{
   for (int i = 0; i < (int)queues.size(); ++i)
   {
      for (int j = 0; j < (int)queues[i].size(); ++j)
      {
         cout << "Task: " << queues[i][j]->getTaskId() <<
            " Job: " << queues[i][j]->getId() << " *** ";
      }
      cout << endl << "----------------------------------------------" << endl;
   }
}

void test_online_approach1(CMI *cmi, const DynamicInfo& p, std::vector <StateTable>& c)
{
   unsigned long t = cmi->getRelativeTimeUsec();

   if (t > 3000000)
   {
      if (p.coreinfos[0].onGoJobId > 0)
      {
         cmi->displayAllQueues();
         cout << "task migration " << endl;
         cmi->taskMigrate(0, 2);
         cmi->displayAllQueues();
      }
   }
}

void test_online_approach2(CMI *cmi, const DynamicInfo& p, std::vector <StateTable>& c)
{
   if (p.tasksInQueue[0].size() > 10)
   {
      cmi->displayAllQueues();
      cout << "task move" << endl;
      cmi->moveJobToAnotherQueue(0, 2, 1, 2);
      cmi->moveJobToAnotherQueue(0, 4, 2, 3);
      cmi->displayAllQueues();
   }
}

void test_online_approach3(CMI *cmi, const DynamicInfo& p, std::vector <StateTable>& c)
{
   if (p.tasksInQueue[0].size() > 10)
   {
      cmi->displayAllQueues();
      cout << "task advance" << endl;
      cmi->advanceJobInQueue(0, 8, 9);
      cmi->advanceJobInQueue(2, 4, 2);
      cmi->displayAllQueues();
   }
}

int  test_online_task_allocator(CMI *cmi, int _taskId)
{
   return(_taskId);
}

int main(int argc, char **argv)
{
   /******************* PARSE INPUT ARGUMENTS *******************/

   string file;

   int isAppendSaveFile = 0;

   if (argc > 1)
   {
      for (int i = 1; i <= argc; i++)
      {
         if (argv[i] == NULL)
         {
            continue;
         }

         if (argv[i] == string("-a"))
         {
            i++;
            isAppendSaveFile = atoi(argv[i]);
         }
         else
         {
            file = string(argv[i]);
            if (file.find(".xml") == string::npos)
            {
               file = file + ".xml";
            }
         }
      }
   }
   else
   {
      file = "example.xml";
   }

   // create the experiment
   CMI *cmi = new CMI(file, isAppendSaveFile);


   /******************* SETTINGS ABOUT THE EXPERIMENT *******************/
   // Modify below functions to customize the experiment

   /* Set dynamic thermal approach*/
   // cmi->setOnlineThermalApproach(test_online_approach1);

   /* Set the period of online thermal approach*/
   // cmi->setOnlineThermalApproachPeriod(200000);

   /* Statically set the cores that tasks should be executed*/
   // vector<int> ids = cmi->getAllTaskIds();
   // for (int i = 0; i < (int)ids.size(); ++i)
   // {
   //    cmi->setTaskRunningCore(ids[i], 0);
   // }

   /* Or define an online task allocator to replace the static task allocator*/
   cmi->setOnlineTaskAllocator(test_online_task_allocator);



   /******************* START THE EXPERIMENT *******************/
   cmi->initializeComponents();

   cmi->startRunning();

   delete cmi;

   return(0);
}

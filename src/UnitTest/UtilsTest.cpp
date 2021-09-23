#include "UnitTest/UnitTest.h"

#include <assert.h>

#include "utils/utils.h"
#include "core/Worker.h"
#include "core/Task.h"
#include "utils/Enumerations.h"
#include "configuration/HyperStateTable.h"
#include "configuration/JobQueue.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"

using namespace std;

unsigned long getDiff(struct timespec begin, struct timespec end)
{
   return(TimeUtil::convert_ms(end - begin));
}

void testStringUtils()
{
   string          test = " 1 ,5, 7";
   vector <double> t    = stringToVector <double>(test);

   appStringToVector(test, t);
   dumpMatrix(t);
}

void testHyperStateTable()
{
   struct timespec sleep = TimeUtil::Micros(50000);
   struct timespec rem;
   HyperStateTable h = HyperStateTable();
   StateTable      s = StateTable(3);

   s.pushState(0, 2000);
   s.pushState(3, 5000);
   s.pushState(2, 9000);
   s.pushState(1, 10000);
   s.pushState(4, 6000);

   struct timespec begin = TimeUtil::getTime();

   h.setStateTable(s, begin);
   struct timespec length;

   for (int i = 0; i < 16; ++i)
   {
      cout << getDiff(begin, h.peekNextTime());
      cout << " --- " << h.getState(TimeUtil::getTime(), length) << endl;

      nanosleep(&sleep, &rem);
   }
}

void testJobQueue()
{
   int n = 20;

   vector <Task *> tasks;
   JobQueue        j1 = JobQueue();

   for (int i = 0; i < n; ++i)
   {
      Task *aux = new Task(singlecore, busywait, 1, 9, i, TimeUtil::Micros(100000));
      tasks.push_back(aux);
      j1.insertJob(aux);
   }

   vector <Task *> task2 = j1.peekAllTasks();


   assert((int)j1.getSize() == n);

   for (int i = 0; i < n; ++i)
   {
      int v = j1.findJob(tasks[i]);
      assert(v == i);
      assert(task2[i] == tasks[i]);
   }


   j1.deleteJob(tasks[0]);
   for (int i = 1; i < n; ++i)
   {
      int v = j1.findJob(tasks[i]);
      assert(v == i - 1);
   }

   j1.deleteJob(tasks[11]);

   for (int i = 1; i < 11; ++i)
   {
      int v = j1.findJob(tasks[i]);
      assert(v == i - 1);
   }

   for (int i = 12; i < n; ++i)
   {
      int v = j1.findJob(tasks[i]);
      assert(v == i - 2);
   }

   j1.deleteJob(tasks[19]);

   assert((int)j1.getSize() == n - 3);



   Task *aux = new Task(singlecore, busywait, 1, 9, n + 1, TimeUtil::Micros(100000));

   cout << j1.getSize() << endl;

   bool r = j1.deleteJob(aux);

   j1.print();

   j1.deleteJobAt(17);

   j1.print();

   cout << r << " " << j1.getSize() << endl;

   j1.insertJobAt(0, aux);

   Task *aux2 = new Task(singlecore, busywait, 1, 9, n + 2, TimeUtil::Micros(100000));

   j1.insertJobAt(32, aux2);



   delete aux;
   delete aux2;


   for (int i = 0; i < n; ++i)
   {
      delete tasks[i];
   }
}

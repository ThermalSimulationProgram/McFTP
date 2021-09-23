#include "pthread/Thread.h"

#include <iostream>
#include <unistd.h>

#include "pthread/Priorities.h"
#include "results/Statistics.h"
#include "utils/Semaphores.h"
#include "utils/TimeUtil.h"

#define _INFO    0

using namespace std;

/***************************************
*        CLASS DEFINITION             *
***************************************/

/*********** CONSTRUCTOR ***********/
///Note that there should never be a simple Thread object because its wrapper
///function is pure virtual. (It should be called from a subclass)

///A simple Constructor which doesn't create the thread, because the static_wrapper
///contains the pure virtual method wrapper() which may be invoked when a derived class
///object is constructed. Therefore, creating an actual thread is done in method trigger().

Thread::Thread(unsigned int _id)
{
   id          = _id;
   joined      = false;
   initialized = false;

   n_cpus = sysconf(_SC_NPROCESSORS_ONLN);
}

///needs nothing to create thread (with inactive priotity)
///Note that this function should not be overloaded!
void Thread::trigger()
{
  #if _INFO == 1
   cout << "Thread " << id << " created\n";
  #endif

   pthread_attr_init(&thread_attr);

   //creating the pthread
   if (pthread_create(&thread, &thread_attr, static_wrapper, this))
   {
      cout << "Error creating thread\n";
      exit(-1);
   }

   //Setting its priorityy
   setPriority(Priorities::get_inactive_pr());
}

Thread::~Thread()
{
   // cout << "Thread Object " << id << " is being destructed\n";
}

/*********** MEMBER FUNCTIONS ***********/

///This function blocks the calling thread until this thread exits
void Thread::join()
{
   if (joined)
   {
      return;
   }
   pthread_join(thread, NULL);
   joined = true;
}

///Copy of the join() method -> in case join() is redefined by a subclass
void Thread::join2()
{
   if (joined)
   {
      return;
   }
   pthread_join(thread, NULL);
   joined = true;
}

///The runnable thread points to this static function. This function recieves a Thread object, and calls the wrapper function in that object
void *Thread::static_wrapper(void *This)
{
   Thread *t = ((Thread *)This);

   t->wrapper();
   // Semaphores::print_sem.wait_sem();
   // cout << "THread::static_wrapper: finished wrapper, id " << t->id << endl;
   // Semaphores::print_sem.post_sem();
   struct timespec ts;

   clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);

   // Semaphores::print_sem.wait_sem();
   // cout << "Thread "  << t->id << " clock time is:" << TimeUtil::convert_us(ts) << endl;
   // Semaphores::print_sem.post_sem();
   //Save the runtime statistic

   Statistics::addRuntime(t->thread_type, t->id, ts);
   // Semaphores::print_sem.wait_sem();
   // cout << "THread::static_wrapper: finished saving time, id " << t->id << endl;
   // Semaphores::print_sem.post_sem();

   pthread_exit(NULL);
   return(NULL);
}

/*********** GETTER AND SETTER FUNCTIONS ***********/

///This function return the thread id
unsigned int Thread::getId()
{
   return(id);
}

///This function sets the priority of the thread
void Thread::setPriority(int priority)
{
   pthread_getschedparam(thread, &policy, &thread_param);
   thread_param.sched_priority = priority;
   pthread_setschedparam(thread, SCHED_FIFO, &thread_param);
}

void Thread::setCPU(int cpu)
{
   if (cpu < 0 || cpu >= n_cpus)
   {
      cpu = n_cpus - 1;
   }

   //Set CPU affinity
   CPU_ZERO(&set);

   for (int i = 0; i < n_cpus; i++)
   {
      //Set only the chosen cpu
      if (i == cpu)
      {
         CPU_SET(i, &set);
      }
      //By default, clear CPU from the set
      else
      {
         CPU_CLR(i, &set);
      }
   }

   if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &set))
   {
      cout << "Failed to set the CPU affinitity of this thread" << endl;
   }
}

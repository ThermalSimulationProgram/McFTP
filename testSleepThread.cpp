#include <pthread.h>
#include <iostream>
// #include <sched.h>
#include <unistd.h>
#include <semaphore.h>
#include <vector>

#include "utils/TimeUtil.h"
#include "utils/Operators.h"

using namespace std;
#define _INFO 1
///This enum describes what thread types can produce traces
// enum _thread_type 
// {
//   idle,
//   thermal_approach,
//   dispatcher,
//   server,
//   worker
// };

class Thread {

 protected:

  /*********** VARIABLES ***********/

  ///Thread id - a unique value among all threads in a simulation
  unsigned int id;

  bool initialized;
  ///Auxiliary variable to determine if thread has been joined
  bool joined;

  ///This is an auxiliary variable to set the thread's priority
  int policy;

  ///The actual pthread variable
  pthread_t thread;

  ///Auxiliary variable to set the thread's attributes
  pthread_attr_t thread_attr;

  ///Auxiliary variable to the set the thread's parameters
  struct sched_param thread_param;

  vector<sem_t> all_sems;
  

  ///Type indicates wether the thread is a scheduler, worker, dispatcher, server, or idle
  _thread_type thread_type;


///number of cpu cores in the machine
  int n_cpus;

  ///Auxiliary variable for the cpu affinity
  cpu_set_t set;

 public:

  /*********** CONSTRUCTOR ***********/

  ///Constructor needs nothing to create thread (with inactive priotity);
  explicit Thread(unsigned int _id);
  virtual ~Thread();

  ///This function creates a real thread, should be called imediately after construction
  void trigger();

  /*********** MEMBER FUNCTIONS ***********/
  
  ///Calling this function will block the calling thread until this thread exits
  virtual void join();

  ///Copy of the join() method -> in case join() is redefined by a subclass
  void join2();

  ///Pure virtual function
  ///This function is called by the static wrapper. In the Worker object, it should point 
  ///to fire(), and in the Scheduler object, it should point to schedule()
  virtual void wrapper() = 0;

  /*********** GETTER AND SETTER FUNCTIONS ***********/

  ///This function returns the thread id
  unsigned int getId();

  ///This function sets the priority of the thread
  void setPriority(int);

  ///This function sets the CPU where this thread runs
  void setCPU(int);

  

 private:
 
  /*********** MEMBER FUNCTIONS ***********/
 
  ///The runnable thread points to this static function. This function recieves a Runnable object, and calls
  ///the wrapper function in that object
  static void * static_wrapper(void * This);

};


class Idle : public Thread {

 private:

  /*********** VARIABLES ***********/

 public:

  /*********** CONSTRUCTOR ***********/

  ///Constructor needs simulation pointer
  Idle();

  /*********** INHERITED FUNCTIONS ***********/
  
  /**** FROM THREAD ****/
  
  ///This function is inherited from Thread, and will call the run() function
  void wrapper();

  /*********** MEMBER FUNCTIONS ***********/

  ///This function contains the flagged loop.
  void run();
  
};

class Worker : public Thread {

 private:

  /*********** VARIABLES ***********/

 public:

  /*********** CONSTRUCTOR ***********/

  ///Constructor needs simulation pointer
  Worker();

  /*********** INHERITED FUNCTIONS ***********/
  
  /**** FROM THREAD ****/
  
  ///This function is inherited from Thread, and will call the run() function
  void wrapper();

  /*********** MEMBER FUNCTIONS ***********/

  ///This function contains the flagged loop.
  void run();
  
};



int main (){

	// auxiliary variable to set main thread priority
	struct sched_param param;

	param.sched_priority = 8; //highest priority
	if ((sched_setscheduler(0, SCHED_FIFO, &param) != 0)) {
		cout << "Run with root\n";
		pthread_exit(0);
	}

	

	Idle s = Idle();

	// Worker w = Worker();
	cout << "all constructed\n";

	s.trigger();
	s.setPriority(2);
	s.setCPU(0);
	// w.trigger();
	
	// w.setCPU(0);
	// w.setPriority(1);

	// w.join2();
	s.join2();

}



Thread::Thread(unsigned int _id) {

  id = _id;
  joined = false;
  initialized = false;
  n_cpus = 6;

  for (int i = 0; i < n_cpus; ++i)
  {
     sem_t aux;
     sem_init(&aux, 0, 1);
     all_sems.push_back(aux);
  }
}

///needs nothing to create thread (with inactive priotity)
///Note that this function should not be overloaded!
void Thread::trigger()
{
  #if _INFO==1
  cout << " is creating Thread " << id << "\n";
  #endif

  for (int i = 0; i < n_cpus; ++i)
  {
     cout << "waiting " << i << " semaphore" << endl;
     sem_wait(&all_sems[i]);
     cout << "get " << i << " semaphore" << endl;
     sem_post(&all_sems[i]);
     cout << "posting " << i << " semaphore" << endl;
  }


  pthread_attr_init(&thread_attr);

  //creating the pthread
  if (pthread_create(&thread, &thread_attr, static_wrapper, this)) {
    cout << "Error creating thread\n";
    exit(-1);
  }
  
  #if _INFO==1
  cout << "Thread " << id << " created\n";
  #endif

  //Setting its priorityy
  // setPriority(1);
}

Thread::~Thread() {
  // cout << "Thread Object " << id << " is being destructed\n";
}

/*********** MEMBER FUNCTIONS ***********/

///This function blocks the calling thread until this thread exits
void Thread::join() {
  if(joined) {
    return;
  }
  pthread_join(thread, NULL);
  joined = true;
}

///Copy of the join() method -> in case join() is redefined by a subclass
void Thread::join2() {
  if(joined) {
    return;
  }
  pthread_join(thread, NULL);
  joined = true;
}

///The runnable thread points to this static function. This function recieves a Thread object, and calls the wrapper function in that object
void * Thread::static_wrapper(void * This)
{
  Thread *t = ((Thread*)This);
  t->wrapper(); 
  // Semaphores::print_sem.wait_sem();
  // cout << "THread::static_wrapper: finished wrapper, id " << t->id << endl;
  // Semaphores::print_sem.post_sem();
  // struct timespec ts;

  // clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
 //Save the runtime statistic
  
  // Statistics::addRuntime(t->thread_type, t->id, ts);
  // Semaphores::print_sem.wait_sem();
  // cout << "THread::static_wrapper: finished saving time, id " << t->id << endl;
  // Semaphores::print_sem.post_sem();
  
  pthread_exit(NULL);
  return NULL;
}


/*********** GETTER AND SETTER FUNCTIONS ***********/

///This function return the thread id
unsigned int Thread::getId() {
  return id;
}

///This function sets the priority of the thread
void Thread::setPriority(int priority) {
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
  CPU_ZERO (&set);
  
  for(int i=0; i<n_cpus; i++) 
  {
    //Set only the chosen cpu
    if(i == cpu) 
    {
      CPU_SET (i, &set);
    }
    //By default, clear CPU from the set
    else 
    {
      CPU_CLR (i, &set);
    }
  }

  if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &set))
  {
      cout << "Failed to set the CPU affinitity of this thread" << endl;
  }

}


Idle::Idle() : Thread(0) {
  #if _INFO == 1
  cout << "++New Idle\n";
  #endif

  thread_type = idle;

  // setPriority(1);
}

/*********** INHERITED FUNCTIONS ***********/

/**** FROM THREAD ****/

///This inherited function will only call run()
void Idle::wrapper() {


  #if _INFO == 1
  cout << "Idle thread beginning execution\n";
  #endif 

  run();
}

/*********** MEMBER FUNCTIONS ***********/

///This function contains the sleep function.
void Idle::run() {

  cout << "Starting Idle thread\n";
  

  struct timespec rem;
  struct timespec now = TimeUtil::getTime();
  struct timespec sleepEnd = now + TimeUtil::Micros(10*1000000);

  // nanosleep(&sleepEnd, &rem);
  while (1){}

  
  cout << "Exiting Idle thread\n";
 

}


Worker::Worker() : Thread(2) {
  #if _INFO == 1
  cout << "++New Worker\n";
  #endif

  thread_type = worker;

  // setPriority(1);
}

/*********** INHERITED FUNCTIONS ***********/

/**** FROM THREAD ****/

///This inherited function will only call run()
void Worker::wrapper() {


  #if _INFO == 1
  cout << "Worker thread beginning execution\n";
  #endif 

  run();
}

void stress_cpu_fibonacci()
{
	// const uint64_t fn_res = 0xa94fad42221f2702ULL;
	long f1 = 0, f2 = 1, fn;
	int i = 0;
	do {
		fn = f1 + f2;
		f1 = f2;
		f2 = fn;
		i++;
		} while (i < 200);
}


/*********** MEMBER FUNCTIONS ***********/

///This function contains the sleep function.
void Worker::run() {

 
  cout << "Starting Worker thread\n";
 

  int i = 0;
  int block = 100000;
  int counter = block;

  while (i < 9999999){
  	stress_cpu_fibonacci();
  	i++;
  	if(i>=counter){
  		cout << i << endl;
  		counter += block;
  	}
  }
 

}




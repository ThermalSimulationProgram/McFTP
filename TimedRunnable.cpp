#include "TimedRunnable.h"

#include <iostream>
#include <errno.h>
#include <unistd.h>

#include "Semaphores.h"
#include "CMI.h"
#include "TimeUtil.h"

using namespace std;


TimedRunnable::TimedRunnable(unsigned _id) : Thread(_id){
	abs_times_ready = false;

	///wrapper_sem has its value always being 0
	sem_init(&wrapper_sem, 0, 0);

	///time_sem initialize  0
	sem_init(&time_sem, 0, 0);
}

TimedRunnable::~TimedRunnable(){
	
}

///This is the pthread's wrapper function
void TimedRunnable::wrapper(){
	
  	Semaphores::print_sem.wait_sem();
	cout << "TimedRunnable::wrapper: This message should not be printed\n";
	Semaphores::print_sem.post_sem();
  
}

void TimedRunnable::timedRun(){
	
	int ret, err_no;
	///wait for the absolute times are setted
	while(!abs_times_ready && (CMI::isSimulating()) ){};

	TimedRunnable * t = (TimedRunnable*) this;

	while (CMI::isSimulating()){
		sem_wait(&time_sem);
		ret = sem_timedwait(&wrapper_sem, &abs_time);

		err_no = errno;
		if(ret == -1){

			if(err_no == ETIMEDOUT) {
      				// cout << "TimedRunnable::wrapper: time out\n";
			}
			else if (err_no==EINVAL){
				cout << "TimedRunnable::wrapper: EINVAL ERROR\n";
			}
			else if (err_no==EAGAIN){
				cout << "TimedRunnable::wrapper: EAGAIN ERROR\n";
			}
			else {
				cout << "TimedRunnable::wrapper: semaphore error (" << errno << ") - "  << "\n";
			}
		}
          //If the call received a signal, it is being deactivated
		else {
			cout << "TimedRunnable::wrapper: received a signal, be deactivated" << endl;
		}

		t->timedJob(i);
	}
}

///This function is just an interface to set vector abs_times. The actuall 
///abs_times should be calculated in derived classes
void TimedRunnable::setAbsTime(struct timespec _t){
	abs_time = _t;
	abs_times_ready = true;
	sem_post(&time_sem);
}



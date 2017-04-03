#include "TimedRunnable.h"

#include <iostream>
#include <errno.h>
#include <unistd.h>

#include "Semaphores.h"
#include "Pipeline.h"
#include "TimeUtil.h"

using namespace std;


TimedRunnable::TimedRunnable(unsigned _id) : Thread(_id){
	abs_times_ready = false;
	///wrapper_sem has its value always being 0
	sem_init(&wrapper_sem, 0, 0);
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
	while(!abs_times_ready && (Pipeline::isSimulating()) ){};

	TimedRunnable * t = (TimedRunnable*) this;
	for (unsigned i = 0; i < abs_times.size(); ++i){
		if (Pipeline::isSimulating()){		
			///wait until the abosulte time.
			///the semaphore wrapper_sem has its value always being 0
			//if timeslot was exhausted, pass on to the next time slot	
			ret = sem_timedwait(&wrapper_sem, &abs_times[i]);
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
		else
			break;
	}
}

///This fucntion calculates the abs_times vector according to given
/// relative times and time offset.
void TimedRunnable::convertRelativeTimesToAbs(const 
	vector<unsigned long>& rl_times, unsigned long offset){
	unsigned long t;
	abs_times.clear();
	abs_times.reserve(rl_times.size());
	for (unsigned i = 0; i < rl_times.size(); ++i){
		t = offset + rl_times[i];
		abs_times.push_back(TimeUtil::Micros(t));
	}
	abs_times_ready = true;
}


///This function is just an interface to set vector abs_times. The actuall 
///abs_times should be calculated in derived classes
void TimedRunnable::setAbsTimes(const vector<struct timespec>& _t){
	abs_times = _t;
	abs_times_ready = true;
}
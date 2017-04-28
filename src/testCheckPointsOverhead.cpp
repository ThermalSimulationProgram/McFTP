#include <semaphore.h>
#include <atomic>
#include <vector>
#include <iostream>

#include "utils/TimeUtil.h"
#include "utils/FileOperator.h"


#define setCheckBlockBegin() \
++TASKLOADCOUNTER;                  \
if(sem_trywait(&test_sem) == 0){ 	\
	TASKLOADSTOPCOUNTER = TASKLOADSTOPCOUNTER>TASKLOADCOUNTER? TASKLOADSTOPCOUNTER: TASKLOADCOUNTER; \
	return 1;							\
}                                      \
if (TASKLOADCOUNTER >= TASKLOADSTOPCOUNTER){ \




#define setCheckBlockEnd() \
} 



sem_t suspend_sem;


sem_t resume_sem;


sem_t test_sem;

unsigned long reallength = 0;



inline void setSuspendPoint(){
    if (sem_trywait(&suspend_sem) == 0)//successfully read a suspend singal, go to sleep immediately
    {
    		// struct timespec now = TimeUtil::getTime();
    		// struct timespec sleepEnd = now + sleepLength;
    	int resumeVal;

      		// make sure the resume semaphore is cleared
    	sem_getvalue(&resume_sem, &resumeVal);
    	for (int i = 0; i < resumeVal; ++i)
    	{
    		sem_trywait(&resume_sem);
    	}

      		// two exit conditions: reach the sleepEnd time, or recieves a resume_sem signal
    	sem_wait(&resume_sem);
    }

};


inline unsigned long busy_wait(unsigned long wcet_us){
	unsigned long timein = TimeUtil::convert_us(TimeUtil::getTime());
	unsigned long length = 0;
	do{
		length = TimeUtil::convert_us(TimeUtil::getTime()) - timein;
	}while(length < wcet_us);
	return length;
}


// void stress_cpu(unsigned long nloop)
// {

// 	setSuspendPoint();
// 	setCheckBlockBegin();


// 	reallength += busy_wait(nloop);
// 	setCheckBlockEnd();

// }



using namespace std;
int main(int argc, char** argv){
	
	sem_init(&suspend_sem, 0, 0);
	sem_init(&resume_sem, 0, 0);
	sem_init(&test_sem, 0, 0);

	
int TASKLOADCOUNTER = 0;
int TASKLOADSTOPCOUNTER = 0;


	int slice = 20;
	if (argc > 1){
		for(int i=1; i<=argc; i++){
			if(argv[i] == NULL)
				continue;

			if (argv[i] == string("-s")){
				i++;
				slice = atoi(argv[i]);
				break;
			}
		}
	}



	int total_time = 1000*1000*10;


	int loop_num2 = total_time / slice;
	int time2 = 0;

	unsigned long timein = TimeUtil::convert_us(TimeUtil::getTime());
	for (int i = 0; i < loop_num2; ++i)
	{	
		setSuspendPoint();
		setCheckBlockBegin();


		time2 += busy_wait((unsigned long)slice);
		setCheckBlockEnd();
	}


	unsigned long time1 = TimeUtil::convert_us(TimeUtil::getTime()) - timein;


	cout << "time1: " << time1 ;

	cout << "  time2: " << time2 ;
	cout << "  overhead percent: " << (double)(100*(time1 - time2))/time1 << endl;

	vector<double> data{(double)slice, (double)time1, (double)time2, (double)(100*(time1 - time2))/time2};

	appendToFile("check_points_overhead.csv", data);

	

	
	

}
 #include <iostream>
#include <semaphore.h>



unsigned long TASKLOADSTOPCOUNTER = 0;

unsigned long TASKLOADCOUNTER = 0;

#define setStopPoint() \
++TASKLOADCOUNTER;                  \
switch ((int ) (TASKLOADCOUNTER >= TASKLOADSTOPCOUNTER)){ \
	case 1:                   \
 


#define setCheckPointEnd() \
    case 0: ;           }         \

using namespace std;	
int main (){

	TASKLOADSTOPCOUNTER = 0;
	setStopPoint();

	cout << "after label 1" << endl;

	setCheckPointEnd();

	setStopPoint();

	cout << "after label 2" << endl;

	setCheckPointEnd();


	setStopPoint();

	cout << "after label 3" << endl;

	setCheckPointEnd();





}




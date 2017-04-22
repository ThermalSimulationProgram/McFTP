#include <iostream>
// #include <fstream>
// #include <stdlib.h>
// #include <pthread.h>
// #include <cmath>
// #include <time.h>
// #include <unistd.h>
#include <string>
// #include <semaphore.h>
// #include <jni.h>
#include <vector>
// #include <exception>
// #include <algorithm>

// #include "Thread.h"
// #include "TimeUtil.h"
// #include "Operators.h"
// #include "Enumerations.h"
// #include "Task.h"
// #include "TimeSim.h"
// #include "Dispatcher.h"
// #include "Scheduler.h"
#include "core/CMI.h"
// #include "Worker.h"
// #include "Parser.h"
// #include "Scratch.h"
// #include "rtc.h"
// #include "temperature.h"
// #include "vectormath.h"
// #include "APTMKernel.h"


// #include "utils.h"
// #include "FileOperator.h"

#include "UnitTest/UnitTest.h"
using namespace std;




#define _TESTRTC  0


void runSimulation(int argc, char** argv);




int main(int argc, char** argv){
	
	
// vector<unsigned long> wcets = {14200, 9000, 3600, 5700};

//   getCoolingCurve(4, wcets);

 // testrtc();
runSimulation( argc, argv);
// testJobQueue();
	// testHyperStateTable();
	// offlineSimulation();
	// testStringUtils();
	// testFileOperator();

	//testParser("example.xml");
}




void runSimulation(int argc, char** argv){


	string *file;
	int isAppendSaveFile = 0;
	if (argc > 1){

		for(int i=1; i<=argc; i++){
			if(argv[i] == NULL)
			    continue;

			if (argv[i] == string("-a")){
				i++;
				isAppendSaveFile = atoi(argv[i]);
			}else{
				file = new string(argv[i]);
				if(file->find(".xml") == string::npos) {
					*file = *file + ".xml";
				}
			}
		}
	}else{
		file = new string("example.xml");
	}

	CMI *p = new CMI(*file);
	// cout << "begin initialize Pipeline\n";
	p->startRunning();

	// double temp = p->simulate();
	// cout << temp << endl;
	delete p;
	delete file;

}










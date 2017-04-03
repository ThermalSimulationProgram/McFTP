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
#include "Pipeline.h"
// #include "Worker.h"
// #include "Parser.h"
// #include "Scratch.h"
#include "warming.h"
// #include "rtc.h"
// #include "temperature.h"
// #include "vectormath.h"
// #include "APTMKernel.h"


// #include "utils.h"
// #include "FileOperator.h"

#include "UnitTesting.h"
using namespace std;




#define _TESTRTC  0


void runSimulation(int argc, char** argv);
void offlineSimulation();




int main(int argc, char** argv){
	
	
// vector<unsigned long> wcets = {14200, 9000, 3600, 5700};

//   getCoolingCurve(4, wcets);

 // testrtc();
runSimulation( argc, argv);

	// offlineSimulation();
	// testStringUtils();
	// testFileOperator();

	//testParser("example.xml");
}










void offlineSimulation(){
	unsigned ncore = 4;
	vector<unsigned long> wcets = {14200, 9000, 3600, 5700};
	getWarmingCurve(ncore);
	getCoolingCurve(ncore, wcets);

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

	Pipeline *p = new Pipeline(*file, isAppendSaveFile);
	// cout << "begin initialize Pipeline\n";
	p->initialize();

	double temp = p->simulate();
	// cout << temp << endl;
	delete p;
	delete file;

}










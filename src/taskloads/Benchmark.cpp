#include "taskloads/Benchmark.h"

#include <cmath>
#include <stdlib.h>
#include <iostream>
#include <cstring>

#include "utils/TimeUtil.h"



using namespace std;

// Constructor uses the default benchmark, sqrt(rand())
Benchmark::Benchmark():Benchmark("default"){
	// -1 means the default benchmark
	benchmarkId = -1;
}

// Constructor needs the name of the benchmark
Benchmark::Benchmark(const std::string& benchmarkName):TaskLoad(), 
RND(), cpuStressor(){
	// get the number of total benchmarks
	int totalNumber = cpuStressor.getBenchmarkNumber();

	// find the benchmark id 
	bool found = false;	
	if (benchmarkName != ""){
		for (int i = 1; i <= totalNumber; ++i){
			if(benchmarkName == cpuStressor.getBenchmarkName(i)){
				benchmarkId = i;
				found = true;
				break;
			}
		}
	}

	// set as the default benchmark if not found
	if (!found){
		benchmarkId = -1;
	}
}

bool Benchmark::runLoads(unsigned long wcet_us){
	// get start time
	unsigned long timeIn = TimeUtil::convert_us(TimeUtil::getTime());
	// record the real executed time
	unsigned long realLength = 0;

	unsigned long timeEnd;

	if (benchmarkId < 1){
		do{
			setSuspendPoint(); // set a suspend point here. When recieves a suspend signal, pause execution here
			setCheckBlockBegin();

			for (int i = 0; i < 3000; ++i){
				sqrt(RND.mwc32());
			}

			timeEnd = TimeUtil::convert_us(TimeUtil::getTime());
			realLength += timeEnd - timeIn;
			timeIn = timeEnd;

			setCheckBlockEnd();
		}while(realLength < wcet_us);

	}else{
		do{
			setSuspendPoint(); // set a suspend point here. When recieves a suspend signal, pause execution here
			setCheckBlockBegin();

			cpuStressor.stressWithBenchmark(benchmarkId);
			timeEnd = TimeUtil::convert_us(TimeUtil::getTime());
			realLength += timeEnd - timeIn;
			timeIn = timeEnd;

			setCheckBlockEnd();
		}while(realLength < wcet_us);

	}

	return true;

}
// unsigned long Benchmark::consume_us_benchmarks(unsigned long length){

// 	if (benchmarkId < 1){
// 		// default
// 		return consume_us_rand(length);
// 	}else{
// 		unsigned long timeIn = TimeUtil::convert_us(TimeUtil::getTime());
// 		unsigned long realLength;
// 		do{
// 			cpuStressor.stressWithBenchmark(benchmarkId);
// 			realLength = TimeUtil::convert_us(TimeUtil::getTime()) - timeIn;

// 		}while(realLength < length);

// 		return realLength;
// 	}	
// }


// unsigned long Benchmark::consume_us_rand(unsigned long length){
// 	unsigned long timeIn = TimeUtil::convert_us(TimeUtil::getTime());
// 	unsigned long realLength;

// 	do{
// 		for (int i = 0; i < 3000; ++i){
// 			sqrt(RND.mwc32());
// 		}
// 		realLength = TimeUtil::convert_us(TimeUtil::getTime()) - timeIn;

// 	}while(realLength < length);

// 	return realLength;
// }


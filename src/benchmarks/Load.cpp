#include "Load.h"

#include <cmath>
#include <stdlib.h>
#include <iostream>
#include <cstring>

#include "utils/TimeUtil.h"



using namespace std;


Load::Load():RND(),cpu_stressor(){
	methodid = -1;
}

Load::Load(const std::string& methodname): RND(),cpu_stressor(){
	int totalMethodNumber = cpu_stressor.getMethodNumber();
	bool found = false;
	// cout << "Load::Load: totalMethodNumber is: " << totalMethodNumber << endl;
	// cout << "Load::Load: methond name is: " << methodname << endl;

	if (methodname != ""){
		for (int i = 1; i <= totalMethodNumber; ++i)
		{
			if(methodname == cpu_stressor.getMethodName(i)){
				methodid = i;
				found = true;
				break;
			}
		}
	}


	if (!found){
		methodid = -1;
	}

	// cout << "Load::Load: method id is: " << methodid << endl;


}

unsigned long Load::consume_us_benchmarks(unsigned long length){
	unsigned long timein = TimeUtil::convert_us(TimeUtil::getTime());
	unsigned long realLength = 0;
	if (methodid < 1){
		do{
			for (int i = 0; i < 3000; ++i){
				sqrt(RND.mwc32());
			}
			realLength = TimeUtil::convert_us(TimeUtil::getTime()) - timein;
		}while(realLength < length);

	}else{
		do{
			// cpu_stressor.stressOnce();
			cpu_stressor.stressWithMethod(methodid);
			realLength = TimeUtil::convert_us(TimeUtil::getTime()) - timein;
		}while(realLength < length);
	}
	
	return realLength;
}


unsigned long Load::consume_us_rand(unsigned long length){
	unsigned long timein = TimeUtil::convert_us(TimeUtil::getTime());
	unsigned long realLength;
	MWC RND = MWC();
	
	do{
		for (int i = 0; i < 3000; ++i)
		{
			sqrt(RND.mwc32());
		}
		realLength = TimeUtil::convert_us(TimeUtil::getTime()) - timein;
	}while(realLength < length);
	return realLength;


}


void waste_time(unsigned _duration){
  double result = 0.0;
  int offset = rand();
  long duration = _duration * 10;
  for (long i = 0; i < duration; ++i)
    result = result + sin(i+offset)*tan(i+offset);
 
}

void Load::consume_us(unsigned long us){
	if (us == 0){
		return;
	}
	unsigned long start = 0, end = 0;
	start = TimeUtil::convert_us(TimeUtil::getTime());
	do{
		Load::waste_time_unit(10);
		end = TimeUtil::convert_us(TimeUtil::getTime());
	}while((end - start)<=us);
}


void Load::consume_ms(unsigned long ms){
	Load::consume_us(1000*ms);
}

void Load::waste_time_unit(unsigned long _duration){
	int offset = rand();
	double result = 0.0;
	for (unsigned long i = 0; i < _duration; ++i){
		result = result + sin(i+offset)*tan(i+offset);
	}

}


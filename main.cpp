#include <iostream>
#include <string>
#include <vector>

#include "core/CMI.h"
#include "UnitTest/UnitTest.h"

using namespace std;


void test_online_approach (CMI* cmi, const DynamicInfo& p, std::vector<StateTable>& c){

}


void runSimulation(int argc, char** argv);



int main(int argc, char** argv){
	
	

runSimulation( argc, argv);
// testJobQueue();

}




void runSimulation(int argc, char** argv){


	string file;
	int isAppendSaveFile = 0;
	if (argc > 1){

		for(int i=1; i<=argc; i++){
			if(argv[i] == NULL)
			    continue;

			if (argv[i] == string("-a")){
				i++;
				isAppendSaveFile = atoi(argv[i]);
			}else{
				file = string(argv[i]);
				if(file.find(".xml") == string::npos) {
					file = file + ".xml";
				}
			}
		}
	}else{
		file = "example.xml";
	}

	CMI *cmi = new CMI(file);

	cmi->setOnlineThermalApproach(test_online_approach);
	vector<int> ids = cmi->getAllTaskIds();
	for (int i = 0; i < (int)ids.size(); ++i)
	{
		cmi->setTaskRunningCore(ids[i], 0);
	}
	// cout << "begin initialize Pipeline\n";
	cmi->startRunning();

	// double temp = p->simulate();
	// cout << temp << endl;
	delete cmi;

}










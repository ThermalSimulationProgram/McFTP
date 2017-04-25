#include <iostream>
#include <string>
#include <vector>

#include "core/CMI.h"
#include "UnitTest/UnitTest.h"

using namespace std;

void displayAllQueues(std::vector<std::vector<Task*> > queues){
	for (int i = 0; i < (int)queues.size(); ++i)
	{
		for (int j = 0; j <(int) queues[i].size(); ++j)
		{
			cout << "Task: " << queues[i][j]->getTaskId() <<
			" Job: " << queues[i][j]->getId() << " *** ";
		}
		cout << endl << "----------------------------------------------" << endl;
	}
}

void test_online_approach1 (CMI* cmi, const DynamicInfo& p, std::vector<StateTable>& c){
	// if (p.tasksInQueue[0].size() > 10){
	// 	cmi->displayAllQueues();
	// 	cout << "task move"<<endl;
	// 	cmi->moveTaskToAnotherQueue(0, 2, 1, 2);
	// 	cmi->moveTaskToAnotherQueue(0, 4, 2, 3);
	// 	cmi->displayAllQueues();
	// }

	// if (p.tasksInQueue[0].size() > 10){
	// 	cmi->displayAllQueues();
	// 	cout << "task advance"<<endl;
	// 	cmi->advanceTaskInQueue(0, 8, 9);
	// 	cmi->advanceTaskInQueue(2, 4, 2);
	// 	cmi->displayAllQueues();
	// }
	unsigned long t = cmi->getRelativeTimeUsec();
	if (t > 3000000){
		if (p.coreinfos[0].onGoJobId > 0){
			cmi->displayAllQueues();
			cout << "task migration "<<endl;
			cmi->taskMigrate(0, 2);
			cmi->displayAllQueues();
		}
	}
}

void test_online_approach2 (CMI* cmi, const DynamicInfo& p, std::vector<StateTable>& c){
	if (p.tasksInQueue[0].size() > 10){
		cmi->displayAllQueues();
		cout << "task move"<<endl;
		cmi->moveTaskToAnotherQueue(0, 2, 1, 2);
		cmi->moveTaskToAnotherQueue(0, 4, 2, 3);
		cmi->displayAllQueues();
	}
}

void test_online_approach3 (CMI* cmi, const DynamicInfo& p, std::vector<StateTable>& c){
	if (p.tasksInQueue[0].size() > 10){
		cmi->displayAllQueues();
		cout << "task advance"<<endl;
		cmi->advanceTaskInQueue(0, 8, 9);
		cmi->advanceTaskInQueue(2, 4, 2);
		cmi->displayAllQueues();
	}	
}


int  test_online_task_allocator (CMI* cmi, int _taskId){
	// unsigned long t = cmi->getRelativeTimeUsec();
	// int ncores = cmi->getCoreNumber();
	// if (t < 1000*1000){
	// 	return _taskId;
	// }else{
	// 	return 1;
	// }
	return _taskId;
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

	cmi->setOnlineThermalApproach(test_online_approach1);
	vector<int> ids = cmi->getAllTaskIds();
	for (int i = 0; i < (int)ids.size(); ++i)
	{
		cmi->setTaskRunningCore(ids[i], 0);
	}

	cmi->setOnlineThermalApproachPeriod(200000);
	cmi->setOnlineTaskAllocator(test_online_task_allocator);
	// cout << "begin initialize Pipeline\n";
	cmi->startRunning();

	// double temp = p->simulate();
	// cout << temp << endl;
	delete cmi;

}










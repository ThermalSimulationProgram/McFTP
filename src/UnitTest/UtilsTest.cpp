#include "UnitTest/UnitTest.h"

#include "utils/utils.h"
#include "core/Worker.h"
#include "core/Task.h"
#include "utils/Enumerations.h"
#include "configuration/HyperStateTable.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"

using namespace std;

unsigned long getDiff(struct timespec begin, struct timespec end){
	return (TimeUtil::convert_ms(end - begin));
}

void testStringUtils(){
	string test = " 1 ,5, 7";
	vector<double> t = stringToVector<double>(test);
	appStringToVector(test, t);
	dumpMatrix(t);
}

void testHyperStateTable(){
	
	struct timespec sleep = TimeUtil::Micros(50000);
	struct timespec rem;
	HyperStateTable h = HyperStateTable();
	StateTable s = StateTable(3);
	s.pushState(0, 2000);
	s.pushState(3, 5000);
	s.pushState(2, 9000);
	s.pushState(1, 10000);
	s.pushState(4, 6000);

	struct timespec begin = TimeUtil::getTime();
	h.setStateTable(s, begin);
	struct timespec length;
	for (int i = 0; i < 16; ++i)
	{
		cout << getDiff(begin, h.peekNextTime());
		cout << " --- " << h.getState(TimeUtil::getTime(), length) << endl;

		nanosleep(&sleep, &rem);
	}
	




}

void testJobQueue(){
	int n = 20;

	vector<Task*> tasks;
	JobQueue* j1 = new JobQueue();

	for (int i = 0; i < n; ++i)
	{
		Task* aux = new Task(busywait, 9);
		tasks.push_back(aux);
		j1->insertJob(aux);

	}



	JobQueue j2 = JobQueue(*j1);
	delete j1;

	j2.print();


	for (int i = 0; i < n; ++i)
	{
		delete tasks[i];
	}

}
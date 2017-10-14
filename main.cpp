#include <iostream>
#include <string>
#include <vector>
# include <cstdlib>
# include <cmath>
# include <complex>

using namespace std;

# include "utils/matrix_exponential.hpp"
# include "utils/test_matrix_exponential.hpp"
# include "utils/c8lib.hpp"
# include "utils/r8lib.hpp"


#include "CMI.h"
//#include "performance_counter/PerformanceCounters.h"
#include "utils/Parser.h"
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
		cmi->moveJobToAnotherQueue(0, 2, 1, 2);
		cmi->moveJobToAnotherQueue(0, 4, 2, 3);
		cmi->displayAllQueues();
	}
}

void test_online_approach3 (CMI* cmi, const DynamicInfo& p, std::vector<StateTable>& c){
	if (p.tasksInQueue[0].size() > 10){
		cmi->displayAllQueues();
		cout << "task advance"<<endl;
		cmi->advanceJobInQueue(0, 8, 9);
		cmi->advanceJobInQueue(2, 4, 2);
		cmi->displayAllQueues();
	}	
}


int  test_online_task_allocator (CMI* cmi, int _taskId){
	return _taskId;
}


void runExperiment(int argc, char** argv);


int main(int argc, char** argv){
	//PerformanceCounters::printCounterInfo();
	//ParserPAPITest(argc, argv);
	runExperiment(argc, argv);
	
}




void runExperiment(int argc, char** argv){
/******************* PARSE INPUT ARGUMENTS *******************/

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

	// create the experiment
	CMI *cmi = new CMI(file, isAppendSaveFile);


	/******************* SETTINGS ABOUT THE EXPERIMENT *******************/
	// Modify below functions to customize the experiment

	/* Set dynamic thermal approach*/
	// cmi->setOnlineThermalApproach(test_online_approach1);
	
	/* Set the period of online thermal approach*/
	// cmi->setOnlineThermalApproachPeriod(200000);

	/* Statically set the cores that tasks should be executed*/
	// vector<int> ids = cmi->getAllTaskIds();
	// for (int i = 0; i < (int)ids.size(); ++i)
	// {
	// 	cmi->setTaskRunningCore(ids[i], 0);
	// }

	/* Or define an online task allocator to replace the static task allocator*/
	cmi->setOnlineTaskAllocator(test_online_task_allocator);



	/******************* START THE EXPERIMENT *******************/
	cmi->initializeComponents();
	cmi->startRunning();

	delete cmi;

}




int main ( );
void matrix_exponential_test01 ( );
void matrix_exponential_test02 ( );

//****************************************************************************80

int main ( )

//****************************************************************************80
//
//  Purpose:
//
//    MAIN is the main program for MATRIX_EXPONENTIAL_PRB.
//
//  Discussion:
//
//    MATRIX_EXPONENTIAL_PRB tests the MATRIX_EXPONENTIAL library.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    05 March 2013
//
//  Author:
//
//    John Burkardt
//
{
  timestamp ( );
  cout << "\n";
  cout << "MATRIX_EXPONENTIAL_TEST:\n";
  cout << "  C++ version\n";
  cout << "  Test the MATRIX_EXPONENTIAL library.\n";
  cout << "  The C8LIB and R8LIB libraries are needed.\n";
  cout << "  This test needs the TEST_MATRIX_EXPONENTIAL library.\n";

  matrix_exponential_test01 ( );
  matrix_exponential_test02 ( );
//
//  Terminate.
//
  cout << "\n";
  cout << "MATRIX_EXPONENTIAL_TEST:\n";
  cout << "  Normal end of execution.\n";
  cout << "\n";
  timestamp ( );

  return 0;
}
//****************************************************************************80

void matrix_exponential_test01 ( )

//****************************************************************************80
//
//  Purpose:
//
//    MATRIX_EXPONENTIAL_TEST01 compares real matrix exponential algorithms.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    02 December 2011
//
//  Author:
//
//    John Burkardt
//
{
  double *a;
  double *a_exp;
  int n;
  int test;
  int test_num;

  cout << "\n";
  cout << "MATRIX_EXPONENTIAL_TEST01:\n";
  cout << "  EXPM is MATLAB's matrix exponential function\n";
  cout << "  R8MAT_EXPM1 is an equivalent to EXPM\n";
  cout << "  R8MAT_EXPM2 uses a Taylor series approach\n";
  cout << "  R8MAT_EXPM3 relies on an eigenvalue calculation.\n";

  test_num = r8mat_exp_test_num ( );

  for ( test = 1; test <= test_num; test++ )
  {
    cout << "\n";
    cout << "  Test #" << test << "\n";

    r8mat_exp_story ( test );

    n = r8mat_exp_n ( test );

    cout << "  Matrix order N = " << n << "\n";

    a = r8mat_exp_a ( test, n );

    r8mat_print ( n, n, a, "  Matrix:" );

    a_exp = r8mat_expm1 ( n, a );
    r8mat_print ( n, n, a_exp, "  R8MAT_EXPM1(A):" );
    delete [] a_exp;

    a_exp = r8mat_expm2 ( n, a );
    r8mat_print ( n, n, a_exp, "  R8MAT_EXPM2(A):" );
    delete [] a_exp;
//
//    a_exp = r8mat_expm3 ( n, a );
//    r8mat_print ( n, n, a_exp, "  R8MAT_EXPM3(A):" );
//    delete [] a_exp;
//
    a_exp = r8mat_exp_expa ( test, n );
    r8mat_print ( n, n, a_exp, "  Exact Exponential:" );
    delete [] a_exp;

    delete [] a;
  }

  return;
}
//****************************************************************************80

void matrix_exponential_test02 ( )

//****************************************************************************80
//
//  Purpose:
//
//    MATRIX_EXPONENTIAL_TEST02 compares complex matrix exponential algorithms.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    05 March 2013
//
//  Author:
//
//    John Burkardt
//
{
  complex <double> *a;
  complex <double> *a_exp;
  int n;
  int test;
  int test_num;

  cout << "\n";
  cout << "MATRIX_EXPONENTIAL_TEST02:\n";
  cout << "  EXPM is MATLAB's matrix exponential function\n";
  cout << "  C8MAT_EXPM1 is an equivalent to EXPM\n";
  cout << "  C8MAT_EXPM2 uses a Taylor series approach\n";
  cout << "  C8MAT_EXPM3 relies on an eigenvalue calculation.\n";

  test_num = c8mat_exp_test_num ( );

  for ( test = 1; test <= test_num; test++ )
  {
    cout << "\n";
    cout << "  Test #" << test << "\n";

    c8mat_exp_story ( test );

    n = c8mat_exp_n ( test );

    cout << "  Matrix order N = " << n << "\n";

    a = c8mat_exp_a ( test, n );

    c8mat_print ( n, n, a, "  Matrix:" );

    a_exp = c8mat_expm1 ( n, a );
    c8mat_print ( n, n, a_exp, "  C8MAT_EXPM1(A):" );
    delete [] a_exp;

//  a_exp = c8mat_expm2 ( n, a );
//  c8mat_print ( n, n, a_exp, "  C8MAT_EXPM2(A):" );
//  delete [] a_exp;
//
//    a_exp = c8mat_expm3 ( n, a );
//    c8mat_print ( n, n, a_exp, "  C8MAT_EXPM3(A):" );
//    delete [] a_exp;
//
    a_exp = c8mat_exp_expa ( test, n );
    c8mat_print ( n, n, a_exp, "  Exact Exponential:" );
    delete [] a_exp;

    delete [] a;
  }

  return;
}







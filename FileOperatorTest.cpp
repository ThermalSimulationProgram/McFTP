#include "UnitTesting.h"


#include "FileOperator.h"

using namespace std;

void testFileOperator(){


	vector<vector<double>> t = loadMatrixFromFile<double>("./csv_data/Dell9020ThermalProp_warmingdata1.csv");
	dumpMatrix(t);
	vector<double> t2 = loadVectorFromFile<double>("example_result.csv");
	dumpMatrix(t2);

	double t3 = loadDoubleFromFile("testttest.csv");

	cout << t3<<endl;


	saveToNewFile("testttest.csv", t);
	appendToFile("testttest.csv", t);
	appendToFile("testttest.csv", t2);
	
}

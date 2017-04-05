#include "UnitTest/UnitTesting.h"

#include "utils/utils.h"

using namespace std;

void testStringUtils(){
	string test = " 1 ,5, 7";
	vector<double> t = stringToVector<double>(test);
	appStringToVector(test, t);
	dumpMatrix(t);
}
#include "utils/utils.h"

#include <algorithm>

using namespace std;
string formatString(const string& base){
	string ret = base;
	std::replace(ret.begin(), ret.end(), '{', ' ');
	std::replace(ret.begin(), ret.end(), '}', ' ');
	std::replace(ret.begin(), ret.end(), ',', ' ');
	std::replace(ret.begin(), ret.end(), '(', ' ');
	std::replace(ret.begin(), ret.end(), ')', ' ');
	std::replace(ret.begin(), ret.end(), '[', ' ');
	std::replace(ret.begin(), ret.end(), ']', ' ');

	return ret;
}

// istringstream stringToiss(const string& base){
// 	std::string fstring = formatString(base);

// 	std::istringstream iss(fstring);
// 	return iss;
// }

void dumpLinesSeperator(){
	std::cout << "***********************************************" << std::endl;
}
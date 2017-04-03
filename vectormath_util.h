#ifndef _VECTORMATHUTIL_H
#define _VECTORMATHUTIL_H 

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

// using namespace std;

#define EPSILON (0.00000001)

 std::vector<int> integerVector(int start, int end);
 std::vector<int> integerVector(int start, int end, int interval);

 bool vectorAny(const std::vector<bool> &);
 bool vectorAll(const std::vector<bool> &);


#endif
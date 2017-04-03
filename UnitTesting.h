#ifndef _UNITTESTING_H
#define _UNITTESTING_H 

#include <vector>
#include <string>



void testFileOperator();


void testAssignToff(double upBound, 
	const std::vector<int> &index, const std::vector<double>& tau0, 
	const std::vector<std::vector<double>>& breakToffs,
	const std::vector<std::vector<double>> &slopes, 
	const std::vector<int>& numValidData);

void testrtc();

void testStringUtils();

void testCalcAPTM(std::vector<double> &tons, std::vector<double>& toffs, unsigned i);

void testParser(std::string xmlfile);

#endif
#ifndef _UNITTESTING_H
#define _UNITTESTING_H

#include <vector>
#include <string>



void testFileOperator();

void testJobQueue();

void testHyperStateTable();

void testStringUtils();

void testCalcAPTM(std::vector < double >&tons, std::vector < double >& toffs, unsigned i);

void testParser(std::string xmlfile);

#endif

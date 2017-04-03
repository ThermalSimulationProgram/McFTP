#ifndef _WARMING_H
#define _WARMING_H

#include <string>
#include <vector>


void getWarmingCurve(unsigned);

void getCoolingCurve(unsigned, std::vector<unsigned long> wcets);

void getOneStageCurve(const std::vector<unsigned long> &, const std::vector<unsigned long> &, 
	const unsigned, const std::string, const std::string);





#endif
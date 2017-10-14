#ifndef _PARSER_H
#define _PARSER_H

#include <string>
#include <vector>
#include <iostream>

#include "core/TaskArgument.h"

#include "utils/pugixml.hpp"

#include "utils/utils.h"





class Parser{
private:
	std::string xml_path;
public:
	explicit Parser(std::string);

	// This function parse the file pointed by xml_path, and then 
	// save all necessary data required by the simulation in Scratch class.
	int parseFile();

	int parseThermalModel(
		std::vector<std::vector<double> >& INVC, 
		std::vector<std::vector<double> >& G,
		std::vector<std::vector<double> >& K,
		double& ambientT,
		double& period);

	TaskArgument parseTask(pugi::xml_node task, int taskid);

	struct timespec parseTime(pugi::xml_node n);
	unsigned long parseTimeMircroSecond(pugi::xml_node n);

	
};

void ParserPAPITest(int argc, char** argv);

template<typename T> T formatTimeMicros(double v, const std::string& unit){
	T r;
	if (unit == "sec")
		r = (T) (v*1000000);
	else if (unit == "ms")
		r = (T) (v*1000);
	else if (unit == "us")
		r = (T) (v);
	else {
		std::cout << "parseTimeVectorMicro: Parser error: could not recognize time unit!\n";
		exit(1);
	}
	return r;
}

template<typename T> std::vector<T> formatTimeMicros(const std::vector<double>& v, 
const std::string& unit){
	std::vector<T> ret;
	for (int i = 0; i < (int) v.size(); ++i)
		ret.push_back(formatTimeMicros<T>(v[i], unit));

	return ret;
}


template<typename T> std::vector<T> parseTimeVectorMicro(pugi::xml_node n){
	std::vector<double> initvalues = stringToVector<double>(n.attribute("value").value());

	std::string unit = n.attribute("unit").value();
	std::vector<T> ret = formatTimeMicros<T>(initvalues, unit);

	return ret;	
}




#endif

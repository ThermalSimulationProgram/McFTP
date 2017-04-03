#ifndef _UTILS_H 
#define _UTILS_H 


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

// using namespace std;

std::string formatString(const std::string& base);
// std::istringstream stringToiss(const std::string& base);


// This function converts a string to a double vector. The  string should be 
// in format: {number1, number2, ..., numbern} or {number1 number2 ... numbern}
template<typename T> 
std::vector<T> stringToVector(std::string const& pointLine)
{
	std::istringstream iss (formatString(pointLine));

	return std::vector<T>{ 
		std::istream_iterator<T>(iss),
		std::istream_iterator<T>()
	};
}

template<typename T> 
void appStringToVector(std::string const& pointLine, std::vector<T>& data)
{
	std::istringstream iss (formatString(pointLine));

	data.insert(data.end(), std::istream_iterator<T>(iss), 
		std::istream_iterator<T>());
}

void dumpLinesSeperator();


template<typename T> 
void dumpVector(std::vector<T> a){
	for (int i = 0; i < int(a.size()); ++i){
		std::cout << a[i];
		if (i < int(a.size())-1){
			std::cout << "\t";
		}
	}
}


template<typename T> 
void dumpMatrix(std::vector<std::vector<T>> a){
	std::cout << "[";
	for (int i = 0; i < int(a.size()); ++i){
		dumpVector(a[i]);
		if (i < int(a.size())-1){
			std::cout << std::endl ;
		}
	}
	std::cout << "]" << std::endl;
	dumpLinesSeperator();
}

template<typename T> 
void dumpMatrix(const std::vector<T>& a){
	std::cout << "[";
	dumpVector(a);
	std::cout << "]" << std::endl ;
	dumpLinesSeperator();
}

template<typename T> 
void displayvector(const std::vector<T>& a, const std::string& name){
	std::cout << name << ": \n";
	dumpMatrix(a);
}

template<typename T> 
void displayvector(const std::vector<std::vector<T>>& a, const std::string& name){
	std::cout << name << ": \n";
	dumpMatrix(a);
}

template<typename T>
std::string vectorTostring(const std::vector<T>& data){
	std::stringstream out;
	for(int i = 0; i < (int)data.size(); i++) {
		if ( i != (int)data.size()-1 )
			out << (double)data[i] << ", " ;
		else
			out << (double)data[i];
	}
	return out.str();
}

template<typename T>
std::vector<std::string> matrixTostring(const std::vector<std::vector<T>>& data){
	std::vector<std::string> ret;

	for (int i = 0; i < (int)data.size(); ++i){
		ret.push_back(vectorTostring(data[i]));
	}

	return ret;
}


#endif
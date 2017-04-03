#ifndef _WARMINGCURVE_H
#define _WARMINGCURVE_H 

#include <vector>

class linearSegs{
public:
	std::vector<double> toffs;
	std::vector<double> slopes;
	std::vector<double> temps;

	linearSegs();

	void setData(std::vector<double>,
		std::vector<double>, std::vector<double>);


	double getValue(double) const;
};


class warmingCurves{
public:
	std::vector<linearSegs> curves;
	double step;
	unsigned maxSize;

	explicit warmingCurves(unsigned);

	void insert(linearSegs);

	 linearSegs getCurve(double) const;





};





#endif
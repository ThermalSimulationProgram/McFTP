#include "warmingCurve.h"

#include <iostream>
using namespace std;


linearSegs::linearSegs(){}

void linearSegs::setData(vector<double> _toffs,
		vector<double> _slopes, vector<double> _temps){
	toffs = _toffs;
	slopes = _slopes;
	temps = _temps;
}

double linearSegs::getValue(double x) const {
	double prepoint = 0;
	for (unsigned i = 0; i < toffs.size(); ++i)
	{
		if (x> prepoint && x<= toffs[i]){
			return temps[i] + (x - toffs[i])*slopes[i];
		}
		prepoint = toffs[i];
	}
	return temps.back();
}

warmingCurves::warmingCurves(unsigned _step){
	if(_step==0){
		cout << _step << endl;
		cerr<<"Invalid step\n";
		exit(1);
	}
	step = (double)_step/100;
	maxSize = 100/step;
}

void warmingCurves::insert(linearSegs _s){
	if(curves.size()>=maxSize)
		return;
	curves.push_back(_s);
}

linearSegs  warmingCurves::getCurve(double _slope) const{
	unsigned id = (unsigned)((_slope-0.01)/step + 0.5);
	if (id >= curves.size())
		id = curves.size()-1;

	return curves[id];
}
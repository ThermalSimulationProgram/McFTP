#ifndef _RTC_H
#define _RTC_H

#include "rtcjniapi.h"


#include <string>
#include <vector>


typedef struct bucket
{
	int bucket1;
	int bucket2;
	double stepWidth1;
	double stepWidth2;
	jobject upperBound1;
	jobject upperBound2;
} bucket;

class rtc{

private:



public:
	static void initialize(); 

/************************** THIRDPARTY METHODS ************************/
	static bucket  	initbucket(long, double, long);
	static jobject 	historyAwareAlphaByLeakyBucket_BSW(bucket, double, int);
	static double  	minbdf_BSF(jobject, double, double);
	static double  	minbdf_BSF(const std::vector<double>&, double, double);
	static double  	minbdf_BSF2(jobject, double, double);
	static double  	minspeedbdfEDG(jobject, double);
	static double  	minspeedbdfEDG(const std::vector<double>&, double);
	static double  	minspeedbdfEDG2(jobject, double);
	static std::vector<jobject> staticHistoryAwareArrialCurves(std::vector<double>, 
		std::vector<double>, long, double, long);

	static std::vector<double> segementsData(jobject, double);
	static bool 	eqZero(const std::vector<double>&);

/************************** HIGH LEVEL METHODS ***********************/
	static jobject 	affine(jobject, double, double);
	static jobject 	plus(jobject, jobject);
	static jobject 	plus(jobject, double);
	static jobject 	minus(jobject, jobject);
	static jobject 	minus(jobject, double);
	static bool    	eq(jobject, jobject);
	static jobject 	min(jobject, jobject);
	static jobject 	min(jobject, double);
	static jobject 	max(jobject, jobject);
	static jobject 	max(jobject, double);
	static jobject 	approxs(jobject, double, int, bool);
	static jobject 	maxconv(jobject, jobject);
	static jobject 	maxconv(jobject, double);


/********************** LOW LEVEL METHODS ********************************/
	static jobject 	Curve(double data[][3] , unsigned);
	static jobject 	Curve(std::vector<double>);
	static jobject 	Curve(std::vector<double>, double, long, double);
	static jobject 	Curve(std::vector<double>, std::vector<double>, 
		double, double, long, double);
	static jobject 	createPJDCurve(long, double, double, bool);

	///////Curve Methods
	static long 	period(jobject);
	static bool 	hasPeriodicPart(jobject);
	static void 	scaleX(jobject, double);
	static void 	scaleY(jobject, double);
	static void 	move(jobject, double, double);
	static void 	setName(jobject, const char *);
	static std::string 	name(jobject);
	static bool 	equals(jobject, jobject);
	static void 	simplify(jobject);
	static double 	y0epsilon(jobject);
	static double 	pdx(jobject);
	static double 	pdy(jobject);
	static double 	px0(jobject);
	static jobject 	clone(jobject);
	static jobject 	aperiodicSegments(jobject);
	static std::string 	toString(jobject);
	static jobject  segmentsLT(jobject, double);


	//////CurveMath methods
	static double 	maxVDist(jobject, jobject);
	static jobject 	maxPlusConv(jobject, jobject);
	static jobject 	maxPlusConv0(jobject);
	static jobject 	add(jobject, jobject);
	static jobject 	sub(jobject, jobject);
	static jobject 	math_max(jobject, jobject);
	static jobject 	math_min(jobject, jobject);


	static double 	s(jobject);
	static double 	x(jobject);

	static jobject 	lastSegment(jobject);
	static void 	trimLT(jobject, double);
	static std::string   segLTtoString(jobject);
};


//util functions
int gcd(int a, int b);
int lcm(int a, int b);



#endif 
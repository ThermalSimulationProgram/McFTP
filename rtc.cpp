#include "rtc.h"

#include "Statistics.h"
// #include "Parser.h"
#include "utils.h"


#include <iostream>
#include <limits>
#include <algorithm>

#define EPSILON (0.000001)


using namespace std;

void rtc::initialize(){
	rtcjni::initialize();
}

/************************** THIRDPARTY METHODS ************************/
bucket rtc::initbucket(long period, double jitter, long delay){
	int approxRange = 10;
	// string callfun = "rtc::initbucket";
	bucket result;
	if ((delay>0 ) && (delay>period-jitter)){
		result.bucket1      = 1;
		result.bucket2      = ceil(jitter/period);
		result.stepWidth1   = (double)delay;
		result.stepWidth2   = (double)period;
		
		jobject upperBound1 = createPJDCurve(delay, 0, 0, true );
		jobject upperBound2 = createPJDCurve(period, jitter, 0, true );
		
		result.upperBound1  = approxs(upperBound1, approxRange*delay, 0, true );
		result.upperBound2  = approxs(upperBound2, approxRange*period, 0, true );
	}
	else{
		result.bucket1      = ceil(jitter/period);
		result.bucket2      = 0;
		result.stepWidth1   = (double)period;
		result.stepWidth2   = (double)0;
		
		jobject upperBound1 = createPJDCurve(period, jitter, 0, true );
		vector<double> temp = {0.0, 0.0, 0.0};
		jobject upperBound2 = Curve(temp );
		
		result.upperBound1  = approxs(upperBound1, approxRange*period, 0, true );
		result.upperBound2  = upperBound2;
	}
	return result;
}

jobject rtc::historyAwareAlphaByLeakyBucket_BSW(bucket b, double curTime,
 int eventArrivals){
 	// string callfun = "rtc::historyAwareAlphaByLeakyBucket_BSW";
 	if (eventArrivals < 0){
 		cerr << "Negative eventArrivals<---"<<endl;
 		exit(RTC_EINPUT);
 	}
	int myfactorJ = floor(curTime/b.stepWidth1);
	int myBurstJ = b.bucket1 - eventArrivals + myfactorJ;
	if (myBurstJ > b.bucket1)
		myBurstJ = b.bucket1;

	int aux1   = ceil(curTime/b.stepWidth1);
	int myk    =  aux1 > 1 ? aux1 : 1;
	double tmp = (double) myk * b.stepWidth1;
	double myoffsetJ;
	if(abs(curTime - tmp)< EPSILON)
		myoffsetJ = 0;
	else
		myoffsetJ = tmp - curTime;

	jobject alpha = affine(b.upperBound1, 1, (-1)*(b.stepWidth1-myoffsetJ) );

	if(myBurstJ != b.bucket1)
		alpha = minus(alpha, (b.bucket1 - myBurstJ) );

	if (b.bucket2 != 0){
		myfactorJ = floor(curTime/b.stepWidth2);
		myBurstJ  = b.bucket2 - eventArrivals + myfactorJ;
		if (myBurstJ > b.bucket2)
			myBurstJ = b.bucket2;

		myk = 1;
		while(myk*b.stepWidth2 < curTime){
			myk++;
		}
		int aux2 = ceil(curTime/b.stepWidth2);
		myk = aux2 > 1? aux2 : 1;
		tmp = myk * b.stepWidth2;

		if(abs(curTime - tmp)< EPSILON)
			myoffsetJ = 0;
		else
			myoffsetJ = tmp - curTime;

		jobject alpha2 = affine(b.upperBound2, 1, myoffsetJ-b.stepWidth2 );
		if(myBurstJ != b.bucket2)
			alpha2 = minus(alpha2, (b.bucket2 - myBurstJ) );

		alpha = min(alpha, alpha2 );
	}

	return alpha;
}


vector<jobject> rtc::staticHistoryAwareArrialCurves(vector<double> rl_job_arrivals,
	vector<double> rl_adaption_times,	long period, 
	double jitter, long delay){

	bucket arrive_bucket = rtc::initbucket(period, jitter, delay);
	vector<jobject> haAlpha;

	for (unsigned i = 0; i < rl_adaption_times.size(); ++i){
		double curTime = rl_adaption_times[i];
		int eventArrivals = 0;
		for (unsigned j = 0; j < rl_job_arrivals.size(); ++j){
			if ((rl_job_arrivals[j] < curTime) || 
				(abs(rl_job_arrivals[j] - curTime)<0.00001))
				eventArrivals++;
			else
				break;
		}

		jobject temp = rtc::historyAwareAlphaByLeakyBucket_BSW(arrive_bucket, curTime,
			eventArrivals);
		haAlpha.push_back(temp);
	}
	return haAlpha;
}



double rtc::minbdf_BSF(jobject beta, double deadline, double k){
	// string callfun = "minbdf_BSF";
	if( k<=0 || deadline <= 0){
		cout << "k: "<< k << "  deadline: " << deadline << endl;
		cerr<<"parameter K and deadline should be positive! <-- minbdf_BSF" ;
		exit(RTC_EINPUT);
	}
	double MAX = deadline;
	double MIN = 0;
	double epsilon = 2;
	bool stop = false;
	double tau; 
	vector<double> tempdata = {0, 0, 0, 0, 0, k};
	while (!stop){
		tau = (MAX + MIN) / 2;
		tempdata[3] = tau;
		jobject temp = Curve(tempdata );
		if (rtc::eq( rtc::max(beta, temp ), temp ))
			MIN = tau;
		else
			MAX = tau;

		if (abs(MAX-MIN) < epsilon){
			tau = MIN;
			stop = true;
		}
	}
	return tau;
}

double rtc::minbdf_BSF(const vector<double>& beta, double deadline, double k){
	// string callfun = "minbdf_BSF";
	if( k<=0 || deadline <= 0){
		cout << "k: "<< k << "  deadline: " << deadline << endl;
		cerr<<"parameter K and deadline should be positive! <-- minbdf_BSF" ;
		exit(RTC_EINPUT);
	}
	unsigned ndata = beta.size();
	if ( ndata%3 != 0 ){
		cerr<<"rtc::minbdf_BSF: input vector size error\n" ;
		exit(RTC_EINPUT);
	}
	int segmentLength = 3;
	unsigned nsegments = beta.size()/segmentLength;
	
	double tau = deadline;
	
	double x, y, tempslope, curtau;

	for (int i = 0; i < (int)nsegments; ++i)
	{	
		int segmentStartId;
		segmentStartId = i*segmentLength;
		x = beta[segmentStartId];
		y = beta[segmentStartId + 1];
		tempslope = beta[segmentStartId + 2];

		if ( abs(y) < EPSILON)
			continue;
		if (tempslope > k){
			tau = 0;
			break;
		}else{
			curtau = x - y/k;
		}
		if (curtau < tau){
			tau = curtau;
		}
	}
	if (tau < 0){
		tau = 0;
	}
	return tau;
}

double rtc::minbdf_BSF2(jobject beta, double deadline, double k){
	// string callfun = "minbdf_BSF";
	if( k<=0 || deadline <= 0){
		cout << "k: "<< k << "  deadline: " << deadline << endl;
		cerr<<"parameter K and deadline should be positive! <-- minbdf_BSF" ;
		exit(RTC_EINPUT);
	}
	double MAX = deadline;
	double MIN = 0;
	double epsilon = 0.1;
	bool stop = false;
	double tau; 
	double tempdata[2][3] = {{0, 0, 0}, {0, 0, k}};

	while (!stop){
		tau = (MAX + MIN) / 2;
		tempdata[1][0] = tau;
		
		jobject temp = Curve(tempdata, (unsigned) 2);
		jobject maxtwo = rtc::max(beta, temp );
		bool xx = rtc::eq(maxtwo , temp );

		if (xx)
			MIN = tau;
		else
			MAX = tau;

		if (abs(MAX-MIN) < epsilon){
			tau = MIN;
			stop = true;
		}
	}
	return tau;
}

double rtc::minspeedbdfEDG(jobject beta, double delay){
	// string callfun = "minspeedbdfEDG";
	if( delay <= 0){
		cerr<<"parameter delay should be positive! <-- minspeedbdfEDG" ;
		exit(RTC_EINPUT);
	}
	double maxspeed = 1;
	double minspeed = 0;
	double epsilon  = 0.00001;
	bool stop       = false;
	vector<double> tempdata;
	jobject base;
	double slope;
	tempdata = vector<double>({0, 0, 0, delay, 0, 1});
	while (!stop){
		slope       = (maxspeed + minspeed)/2;
		tempdata[5] = slope;
		base        = Curve(tempdata );
		if ( eq( math_max(beta, base ), base ) )
			maxspeed = slope;
		else
			minspeed = slope;

		if (abs(maxspeed - minspeed) < epsilon){
			slope = maxspeed;
			stop = true;
		}
	}
	return slope;
}

double rtc::minspeedbdfEDG(const vector<double>& beta, double delay){
	// string callfun = "minspeedbdfEDG";
	if( delay <= 0){
		cerr<<"parameter delay should be positive! <-- minspeedbdfEDG" ;
		exit(RTC_EINPUT);
	}
	unsigned ndata = beta.size();
	if ( ndata%3 != 0 ){
		cerr<<"rtc::minspeedbdfEDG: input vector size error\n" ;
		exit(RTC_EINPUT);
	}
	int segmentLength = 3;
	unsigned nsegments = beta.size()/segmentLength;
	
	double slope = 0;
	int segmentStartId;
	double x, y, curSlope, tempslope;

	for (int i = 0; i < (int)nsegments; ++i)
	{	
		segmentStartId = i*segmentLength;
		x = beta[segmentStartId];
		y = beta[segmentStartId + 1];
		tempslope = beta[segmentStartId + 2];

		if(x > delay){
			curSlope = y / (x - delay);
			curSlope = tempslope > curSlope ? tempslope : curSlope;
		}else{
			if (y>EPSILON){
				slope = 1;
				break;
			}
			curSlope = tempslope;
		}

		if (curSlope > slope)
			slope = curSlope;
	}	
	return slope;
}



double rtc::minspeedbdfEDG2(jobject beta, double delay){
	// string callfun = "minspeedbdfEDG";
	if( delay <= 0){
		cerr<<"parameter delay should be positive! <-- minspeedbdfEDG" ;
		exit(RTC_EINPUT);
	}
	double maxspeed = 1;
	double minspeed = 0;
	double epsilon  = 0.01;
	bool stop       = false;
	double tempdata[2][3] = {{0,0,0},{delay, 0, 1}};
	jobject base;
	double slope;
	while (!stop){
		slope       = (maxspeed + minspeed)/2;
		tempdata[1][2] = slope;
		base        = Curve(tempdata, (unsigned)2);
		if ( eq( math_max(beta, base ), base ) )
			maxspeed = slope;
		else
			minspeed = slope;

		if (abs(maxspeed - minspeed) < epsilon){
			slope = maxspeed;
			stop = true;
		}
	}
	return slope;
}


vector<double> rtc::segementsData(jobject a, double xmax){
	jobject segments = rtc::segmentsLT(a, xmax);
	string s = rtc::segLTtoString(segments);
	vector<double>ret =  stringToVector<double>(s);
	return ret;
}


bool rtc::eqZero(const vector<double> & data){
	bool eqzero =true;
	int numeldata = data.size();
	if (numeldata < 3 || numeldata%3 != 0){
		cerr<<"rtc::eqZero: input vector size error\n" ;
		exit(RTC_EINPUT);
	}
	for (int i = 1; i < numeldata; i+=3){
		if (data[i]>EPSILON || data[i+1]>EPSILON){
			eqzero = false;
			break;
		}
	}
	return eqzero;
}


/************************** HIGH LEVEL METHODS ***********************/
jobject rtc::affine(jobject c, double a, double b ){
	// callfun = "rtc::affine<---" + callfun;
	if(a <= 0){
		cerr << "RTCAFFINE - Argument A must be a positive scalar.<---"  << endl;
		exit(RTC_EINPUT);
	}
	jobject result = clone(c );
	try{
		scaleX(result, a );
	}
	catch (...){
		cerr << "RTCAFFINE - A * Period of periodic part must be an integer.<---" << endl;
		exit(RTC_EINPUT);
	}
	move(result, b, 0 );
	return result;
}

jobject rtc::plus(jobject a, jobject b ){
	// callfun = "rtc::plus<---" + callfun;
	return add(a, b );
}
jobject rtc::plus(jobject a, double b ){
	// callfun = "rtc::plus<---" + callfun;
	jobject result = clone(a );
	move(result, 0, b );
	return result;
}
jobject rtc::minus(jobject a, jobject b ){
	// callfun = "rtc::minus<---" + callfun;
	return sub(a, b );
}
jobject rtc::minus(jobject a, double b ){
	// callfun = "rtc::minus<---" + callfun;
	jobject result = clone(a );
	move(result, 0, -b );
	return result;
}

bool rtc::eq(jobject a, jobject b ){
	// callfun = "rtc::eq<---" + callfun;
	if(equals(a, b )){
		return true;
	}
	else{
		jobject atemp = clone(a);
		jobject btemp = clone(b);
		simplify(atemp );
		simplify(btemp );
		if(equals(atemp, btemp ))
			return true;
		else
			return false;
	}
}

jobject rtc::min(jobject a, jobject b ){
	// callfun = "rtc::min<---" + callfun;
	return math_min(a, b );
}
jobject rtc::min(jobject a, double b ){
	// callfun = "rtc::min<---" + callfun;
	vector<double> d = {0, b, 0};
	jobject curveb = Curve(d );
	return math_min(a, curveb );
}

jobject rtc::max(jobject a, jobject b ){
	// callfun = "rtc::max<---" + callfun;
	return math_max(a, b );
}
jobject rtc::max(jobject a, double b ){
	// callfun = "rtc::max<---" + callfun;
	vector<double> d = {0, b, 0};
	jobject curveb = Curve(d );
	return math_max(a, curveb );
}


jobject rtc::approxs(jobject c, double px0New, int pdxNew, bool isUpper ){
	// callfun = "rtc::approxs<---" + callfun;
	if ((pdxNew < 0) || std::isinf(pdxNew)){
		cerr << "pdxNew out of range<---" ;
		exit(RTC_EINPUT);
	} 
	if ( (px0New < 0) || (hasPeriodicPart(c )&& std::isinf(px0New)) ) {
		cerr << "px0New out of range<---" ;
		exit(RTC_EINPUT);
	}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// % Construct Initial Aperiodic Part
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	jobject rLast = clone(c );
	move(rLast, -px0New, 0 );
	double py0New = y0epsilon(rLast );

	vector<double> temp = {0, py0New, 0};
	jobject rInit = min(c, Curve(temp ) );

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// % Construct Aperiodic Approximation
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	jobject result;
	jobject rTail;
	if (pdxNew == 0){
		if (std::isinf(px0New)){
			result = c;
			simplify(result );
			return result;
		}


		//% determine slope
		double slope;
		if (hasPeriodicPart(rLast ))
			slope = pdy(rLast )/pdx(rLast );
		else
			slope = s(lastSegment(aperiodicSegments(rLast ) ) );


		//% determine position of last segment
		double distance;
		if (isUpper){
			vector<double> temp2 = {0, 0, slope};
			distance = maxVDist(rLast, Curve(temp2 ) );
		}
		else{
			vector<double> temp3 = {0, py0New, slope};
			distance = py0New - maxVDist(Curve(temp3 ), rLast );
		}

		
		if (px0New == 0){
			vector<double> temp4 = {0, distance, slope};
			rTail = Curve(temp4 );
		}
		else{
			vector<double> temp5 = {0, 0, 0, px0New, distance, slope};
			rTail = Curve(temp5 );
		}
		
	}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// % Construct Periodic Approximation
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	else {
		//determine slope of periodic part and number of unfoldings
		double slope;
		int xNumber;
		if (hasPeriodicPart(rLast )){
			slope = pdy(rLast )/pdx(rLast );
			double xLast = px0(rLast ) + lcm(pdxNew, (int) pdx(rLast ));
			xNumber = ceil(xLast/pdxNew);
		}
		else{
			slope = s(lastSegment(aperiodicSegments(rLast ) ) );
			double xLast = x(lastSegment(aperiodicSegments(rLast ) ) );
			xNumber = ceil(xLast/pdxNew) + 1;
		}
		//approximate last segments
		jobject current = clone(rLast );
		double midnum;
		if (isUpper)
			midnum = 0;
		else
			//midnum = INFINITY;
			 midnum = std::numeric_limits<double>::infinity();
		vector<double> temp6 = {0, midnum, 0};
		jobject rPer = Curve(temp6 );

		jobject next; double yMax;
		for (int i = 0; i < xNumber; ++i)
		{
			next = clone(current );
			move(next, -pdxNew, 0 );
			yMax = y0epsilon(next );
			move(next, 0, -slope*pdxNew );
			vector<double> temp7 = {0, yMax, 0};
			current = min(current, Curve(temp7 ) );
			if (isUpper)
				rPer = max(rPer, current );
			else
				rPer = min(rPer, current );
			current = clone(next );
		}

		//remove all segments with x values larger or equal to period
		jobject tmp = aperiodicSegments(rPer );
		trimLT(tmp, (double)pdxNew );
		string describe = segLTtoString(tmp );
		vector<double> tmpdata = stringToVector<double>(describe );

		if (px0New == 0)
			rTail = Curve(tmpdata, 0, pdxNew, pdxNew*slope );
		else{
			vector<double> temp8 = {0.0, 0.0, 0.0};
			rTail = Curve(temp8, tmpdata, px0New, 0, pdxNew, pdxNew*slope );
		}
	}
	result = max(rTail, rInit );
	if (isUpper)
		result = maxconv(result, (double)0 );

	simplify(result );
	return result;
}

jobject rtc::maxconv(jobject a, jobject b ){
	// callfun = "rtc::maxconv<---" + callfun;
	return maxPlusConv(a, b );
}
jobject rtc::maxconv(jobject a, double b ){
	// callfun = "rtc::maxconvdouble<---" + callfun;
	if (abs(b) < 0.000001)
		return maxPlusConv0(a );
	else{
		vector<double> d = {0, b, 0};
		return maxPlusConv(a, Curve(d ) );
	}
}

/********************** LOW LEVEL METHODS ********************************/
jobject rtc::Curve(double data[][3], unsigned nrows){
	jobjectArray arg4jni = rtcjni::jni2darray(data, nrows, 3);
	jvalue _firstarg;
	_firstarg.l          = arg4jni;
	const jvalue arg[1]  = {_firstarg};
	jobject ret          = rtcjni::newobject(rtcjni::_Curve, rtcjni::_curve, arg );
	return ret;
}



jobject rtc::Curve(vector<double> aper ){
	// callfun              = "rtc::Curve([[D)V<---" + callfun;
	jobjectArray arg4jni = rtcjni::vector2jni2darray(aper );
	jvalue _firstarg;
	_firstarg.l          = arg4jni;
	const jvalue arg[1]  = {_firstarg};
	jobject ret          = rtcjni::newobject(rtcjni::_Curve, rtcjni::_curve, arg );
	return ret;
}


jobject rtc::Curve(vector<double> per, double py0, long period, double pdy ){
	// callfun              = "rtc::Curve([[DDJD)V<---" + callfun;
	jobjectArray arg4jni = rtcjni::vector2jni2darray(per );
	jvalue arg1, arg2, arg3, arg4; 
	arg1.l               = arg4jni;
	arg2.d               = (jdouble) py0;
	arg3.j               = (jlong) period;
	arg4.d               = (jdouble) pdy;
	const jvalue arg[4]  = {arg1, arg2, arg3, arg4};
	jobject ret = rtcjni::newobject(rtcjni::_Curve, rtcjni::_curve1M3d, arg );
	return ret;
}


jobject rtc::Curve(vector<double> aper, vector<double> per, double px0, 
	double py0, long period, double pdy ){
	// callfun               = "rtc::Curve([[D[[DDDJD)V<---" + callfun;
	jobjectArray arg4jni1 = rtcjni::vector2jni2darray(aper );
	jobjectArray arg4jni2 = rtcjni::vector2jni2darray(per );
	jvalue arg1, arg2, arg3, arg4, arg5, arg6; 
	arg1.l                = arg4jni1;
	arg2.l                = arg4jni2;
	arg3.d                = (jdouble) px0;
	arg4.d                = (jdouble) py0;
	arg5.j                = (jlong) period;
	arg6.d                = (jdouble) pdy;
	const jvalue arg[6]   = {arg1, arg2, arg3, arg4, arg5, arg6};
	jobject ret = rtcjni::newobject(rtcjni::_Curve, rtcjni::_curve2M4d, arg );
	return ret;

}



jobject rtc::createPJDCurve(long p, double j, double d, bool upper ){
	// callfun = "rtc::createPJDCurve<---" + callfun;
	if ( p<=0 || j<0 || d<0){
		cerr << "Invalid inpt parameter<---"  << endl;
		exit(RTC_EINPUT);
	}
	jobject ret        = NULL;
	
	jstring _curvename = rtcjni::string2jstring("pjd" );
	jvalue _p, _j, _d, _name;
	_p.j               = (jlong) p; 
	_j.d               = (jdouble) j; 
	_d.d               = (jdouble) d; 
	_name.l            = _curvename;
	const jvalue args[4] = {_p, _j, _d, _name};

	jvalue uniret;
 	if (upper)
		uniret = rtcjni::callstaticmethod(rtcjni::_CurveFactory, 
			rtcjni::_createUpperPJDCurve, args, _object );
	else
		uniret = rtcjni::callstaticmethod(rtcjni::_CurveFactory, 
			rtcjni::_createLowerPJDCurve, args, _object );

	ret = uniret.l;
	return ret;
}

long rtc::period(jobject rtccurve ){
	const jvalue * arg = NULL;
	// callfun            = "rtc::period<---" + callfun; 
	jvalue ret         = rtcjni::callmethod(rtccurve, rtcjni::_period, 
		arg, _long );
	long p             = (long) ret.l;
	return p;

}

bool rtc::hasPeriodicPart(jobject rtccurve ){
	const jvalue * arg = NULL;
	// callfun            = "rtc::hasPeriodicPart<---" + callfun; 
	jvalue ret         = rtcjni::callmethod(rtccurve, rtcjni::_hasPeriodicPart, 
		arg, _bool );
	bool flag          = (bool) ret.z;
	return flag;
}

double rtc::y0epsilon(jobject a ){
	// callfun = "rtc::y0epsilon<---" + callfun;
	return rtcjni::jobjectIndoubleOut(rtcjni::_y0epsilon, a );
}

double rtc::pdx(jobject a ){
	// callfun = "rtc::pdx<---" + callfun; 
	return rtcjni::jobjectIndoubleOut(rtcjni::_pdx, a );
}
double rtc::px0(jobject a ){
	// callfun = "rtc::px0<---" + callfun; 
	return rtcjni::jobjectIndoubleOut(rtcjni::_px0, a );
}

double rtc::pdy(jobject a ){
	// callfun = "rtc::pdy<---" + callfun; 
	return rtcjni::jobjectIndoubleOut(rtcjni::_pdy, a );
}

double rtc::s(jobject a ){
	// callfun = "rtc::s<---" + callfun; 
	return rtcjni::jobjectIndoubleOut(rtcjni::_s, a );
}

double rtc::x(jobject a ){
	// callfun = "rtc::x<---" + callfun; 
	return rtcjni::jobjectIndoubleOut(rtcjni::_x, a );
}

jobject rtc::clone(jobject a ){
	// callfun = "rtc::clone<---" + callfun; 
	return rtcjni::jobjectInjobjectOut(rtcjni::_clone, a );
}

jobject rtc::aperiodicSegments(jobject a ){
	// callfun = "rtc::aperiodicSegments<---" + callfun; 
	return rtcjni::jobjectInjobjectOut(rtcjni::_aperiodicSegments, a );
}

void rtc::simplify(jobject a ){
	const jvalue * arg = NULL;
	// callfun = "rtc::simplify<---" + callfun; 
	rtcjni::callmethod(a, rtcjni::_simplify, arg, _void );
}

bool rtc::equals(jobject a, jobject b ){
	// callfun = "rtc::equals<---" + callfun; 
	jvalue argb; argb.l = b; 
	const jvalue arg[1] = {argb};
	jvalue ret = rtcjni::callmethod(a, rtcjni::_equals, arg, _bool );
	return (bool) ret.z;
}

double rtc::maxVDist(jobject a, jobject b  ){
	// callfun             = "rtc::maxVDist<---" + callfun; 
	double result       = -1.0;
	jvalue arga, argb; 
	arga.l              = a; 
	argb.l              = b; 
	const jvalue arg[2] = {arga, argb};
	jvalue ret = rtcjni::callstaticmethod(rtcjni::_CurveMath,
		rtcjni:: _maxVDist, arg, _double );
	result = (double) ret.d;
	
	return result;
}

jobject rtc::maxPlusConv(jobject a, jobject b ){
	// callfun = "rtc::maxPlusConv<---" + callfun; 
	return rtcjni::staticTwojobjectInjobjectOut(rtcjni::_CurveMath, 
		rtcjni::_maxPlusConv, a, b );
}

jobject rtc::maxPlusConv0(jobject a ){
	// callfun = "rtc::maxPlusConv0<---" + callfun; 
	jobject result = NULL;
	jvalue arga;  arga.l = a;
	const jvalue arg[1] = {arga};
	jvalue ret = rtcjni::callstaticmethod(rtcjni::_CurveMath, 
		rtcjni::_maxPlusConv0, arg, _object );
	result = ret.l;

	return result;
}

jobject rtc::lastSegment(jobject a ){
	// callfun = "rtc::lastSegment<---" + callfun; 
	return rtcjni::jobjectInjobjectOut(rtcjni::_lastSegment, a );
}

jobject rtc::add(jobject a, jobject b ){
	// callfun = "rtc::add<---" + callfun; 
	return rtcjni::staticTwojobjectInjobjectOut(rtcjni::_CurveMath, 
		rtcjni::_add, a, b );
}

jobject rtc::sub(jobject a, jobject b ){
	// callfun = "rtc::sub<---" + callfun; 
	return rtcjni::staticTwojobjectInjobjectOut(rtcjni::_CurveMath, 
		rtcjni::_sub, a, b );
}

jobject rtc::math_max(jobject a, jobject b ){
	// callfun = "rtc::math_max<---" + callfun; 
	return rtcjni::staticTwojobjectInjobjectOut(rtcjni::_CurveMath, 
		rtcjni::_max, a, b );
}

jobject rtc::math_min(jobject a, jobject b ){
	// callfun = "rtc::math_min<---" + callfun; 
	return rtcjni::staticTwojobjectInjobjectOut(rtcjni::_CurveMath, 
		rtcjni::_min, a, b );
}

void rtc::scaleX(jobject rtccurve, double scalar ){
	// callfun = "rtc::scaleX<---" + callfun; 
	rtcjni::jobjectdoubleInvoidOut(rtcjni::_scaleX, rtccurve, scalar );
}

void rtc::scaleY(jobject rtccurve, double scalar ){
	// callfun = "rtc::scaleY<---" + callfun; 
	rtcjni::jobjectdoubleInvoidOut(rtcjni::_scaleY, rtccurve, scalar );
}

void rtc::trimLT(jobject seglist, double xMax ){
	// callfun = "rtc::trimLT<---" + callfun; 
	rtcjni::jobjectdoubleInvoidOut(rtcjni::_trimLT, seglist, xMax );
}


void rtc::move(jobject rtccurve, double dx, double dy ){
	// callfun = "rtc::move<---" + callfun; 
	jvalue arg1, arg2;
	arg1.d = dx; arg2.d = dy;
	const jvalue arg[2] = {arg1, arg2};
	rtcjni::callmethod(rtccurve, rtcjni::_move, arg, _void );
}

void rtc::setName(jobject rtccurve, const char * name ){
	// callfun = "rtc::setName<---" + callfun; 
	jstring _name = rtcjni::string2jstring(name );
	jvalue arg1;
	arg1.l = _name;
	const jvalue arg[1] = {arg1};
	rtcjni::callmethod(rtccurve, rtcjni::_setname, arg, _void );
}

string rtc::name(jobject rtccurve ){
	// callfun = "rtc::name<---" + callfun; 
	return rtcjni::jobjectInstringOut(rtcjni::_name, rtccurve );
}

string rtc::toString(jobject rtccurve ){
	// callfun = "rtc::toString<---" + callfun; 
	return rtcjni::jobjectInstringOut(rtcjni::_tostring, rtccurve );
}

jobject  rtc::segmentsLT(jobject rtccurve, double xMax){
	return rtcjni::jobjectdoubleInjobjectOut(rtcjni::_segmentsLT, 
		rtccurve, xMax );
}


string rtc::segLTtoString(jobject rtccurve){
	return rtcjni::jobjectInstringOut(rtcjni::_segLTtoString, rtccurve );
}



int gcd(int a, int b){
    for (;;){
        if (a == 0) return b;
        b %= a;
        if (b == 0) return a;
        a %= b;
    }
}

int lcm(int a, int b){
    int temp = gcd(a, b);
    return temp ? (a / temp * b) : 0;
}


// vector<double> rtc::string2vector(string sin ){
// 	// callfun = "rtc::stringToVector<---" + callfun;
// 	string base = sin;
// 	if (base.length() < 15){
// 		cerr<<"Invalid input string<---"<<endl;
// 		exit(RTC_EINPUT);
// 	}
// 	std::replace(base.begin(), base.end(), '{', ' ');
// 	std::replace(base.begin(), base.end(), '}', ' ');
// 	std::replace(base.begin(), base.end(), '(', ' ');
// 	std::replace(base.begin(), base.end(), ')', ' ');
// 	std::replace(base.begin(), base.end(), ',', ' ');
// 	std::string::size_type sz;
// 	vector<double> result;
// 	try{
// 		do{
// 			result.push_back(stod(base, &sz));
// 			base = base.substr(sz);
// 		}while(base.length()>=5);
// 	}
// 	catch(...){
// 		cerr<<"Unknown error happens for string "<<sin<<endl;
// 		exit(RTC_ERR);
// 	}
// 	if (result.size()%3 != 0){
// 		cerr<<"Invalid input string<---"<<endl;
// 		exit(RTC_EINPUT);
// 	}
// 	return result;
// }

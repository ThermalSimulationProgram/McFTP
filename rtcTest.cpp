#include "UnitTesting.h"

#include <iostream>
#include <vector>

#include "rtc.h"
#include "utils.h"


using namespace std;

void testrtc(){
	rtc::initialize();

	long period = 100;
	double jitter = 150;
	long delay = 1;

	bucket bb = rtc::initbucket(period, jitter, delay);

	jobject alpha = rtc::historyAwareAlphaByLeakyBucket_BSW(bb,  50, 3);
	cout << rtc::toString(alpha);


//b = rtcpjdu(134,67,11);
	 jobject pjd = rtc::createPJDCurve(100, 0, 0, true );
	 jobject pjd2 = rtc::affine(pjd, 1, 90);
	 vector<double> pjdtestdata = rtc::segementsData(pjd2, 1200);
	 displayvector(pjdtestdata, "pjdtestdata");

	 cout << rtc::minspeedbdfEDG(pjd2, 59) << endl;
	 cout << rtc::minspeedbdfEDG(pjdtestdata, 59) << endl;

	 cout << rtc::minbdf_BSF(pjd2, 120, 0.4) << endl;
	 cout << rtc::minbdf_BSF(pjdtestdata, 120, 0.4) << endl;

	 jobject c = rtc::maxconv(alpha, pjd );
	cout << rtc::toString(c );
	//d = rtcapproxs(c, 500, 0, 1);
	jobject d = rtc::approxs(c, 700, 1, true );
	cout << rtc::toString(d );
	vector<double> data = {0,0,0,5,0,1,10,5,1};
	vector<double> data2 = {0,2,0,5,2,1,10.1,7,1};
	jobject apercurve = rtc::Curve(data );
	jobject apercurve2 = rtc::Curve(data);
	jobject apercurve3 = rtc::Curve(data2);
	bool isequal1 = rtc::equals(pjd, apercurve );
	bool isequal2 = rtc::equals(apercurve2, apercurve );
	bool isequal3 = rtc::equals(apercurve3, apercurve );
	cout << "isequal1: " << isequal1 << endl;
	cout << "isequal2: " << isequal2 << endl;
	cout << "isequal3: " << isequal3 << endl;
	double y0 = rtc::y0epsilon(apercurve3 );
	cout << "y0: " << y0 << endl;
	double pdxx = rtc::pdx(apercurve3 );
	cout << "pdxx: " << pdxx << endl;

	jobject aclone = rtc::clone(apercurve3 );
	bool isequal4 = rtc::equals(apercurve3, aclone );
	cout << "isequal4: " << isequal4 << endl;

	double maxvd = rtc::maxVDist(apercurve3, apercurve );
	cout << "maxvd: " << maxvd << endl;

	string curve3 = rtc::toString(apercurve3 );
	cout << curve3 << endl;

	jobject minconvret = rtc::sub(apercurve, pjd );
	cout << rtc::toString(minconvret);



}
#ifndef _RTCJNIAPI_H
#define _RTCJNIAPI_H

#include <jni.h>
#include <string>
#include <vector>

#define RTC_ERR		(-1)
#define RTC_EINPUT 	(-2)
#define RTC_ENULL 	(-3)


enum _method_return_type{
	_void,
	_object,
	_bool,
	_byte,
	_char,
	_short,
	_int,
	_long,
	_float,
	_double,
};

enum _method_type{
	_normal,
	_static,
	_nonvirtual
};


class rtcjni{

private:
	/******************* JNI related variables *******************/
	static JavaVM *jvm;
	static JNIEnv *env;
	static JavaVMInitArgs vm_args;
	static jthrowable exc;

public:
	/******************* RTCTOOLBOX CLASSES AND METHODS *******************/	
	static jclass _Curve;
	static jclass _CurveFactory;
	static jclass _CurveMath;
	static jclass _Segment;
	static jclass _SegmentList;

	// curve creater ID
	static jmethodID _curve;
	static jmethodID _curve2M4d;
	static jmethodID _curve1M3d;
	static jmethodID _createLowerPJDCurve;
	static jmethodID _createUpperPJDCurve;

	//operations
	//////////Curve methodIDs
	static jmethodID _period;
	static jmethodID _hasPeriodicPart;
	static jmethodID _scaleX;
	static jmethodID _scaleY;
	static jmethodID _move;
	static jmethodID _setname;
	static jmethodID _name;
	static jmethodID _equals;
	static jmethodID _simplify;
	static jmethodID _y0epsilon;
	static jmethodID _pdx;
	static jmethodID _px0;
	static jmethodID _pdy;
	static jmethodID _clone;
	static jmethodID _aperiodicSegments;
	static jmethodID _tostring;
	static jmethodID _segmentsLT;

	//////////CurveMath methodIDs
	static jmethodID _maxVDist;
	static jmethodID _maxPlusConv;
	static jmethodID _maxPlusConv0;
	static jmethodID _add;
	static jmethodID _sub;
	static jmethodID _max;
	static jmethodID _min;
	
	//////////Segment methodIDs
	static jmethodID _s;
	static jmethodID _x;
	static jmethodID _segLTtoString;
	
	//////////SegmentList methodIDs
	static jmethodID _lastSegment;
	static jmethodID _trimLT;


	/********************* PUBLIC METHODS **************************/
	static void initialize();
	static void handleException(const std::string &);

	static jobjectArray vector2jni2darray(std::vector<double>);
	static jobjectArray jni2darray(double data[][3], unsigned, unsigned);
	static std::string jstring2string(jstring);
	static jstring string2jstring(std::string);

	static jclass findclass(const char *);
	static jmethodID getmethodid(jclass, const char *, const char *, enum _method_type);
	static jvalue callmethod(jobject, jmethodID, const jvalue*, _method_return_type);
	static jvalue callstaticmethod(jclass, jmethodID, const jvalue*, _method_return_type);
	static jobject newobject(jclass, jmethodID, const jvalue*);

	static void jobjectdoubleInvoidOut(jmethodID, jobject, double);
	static jobject jobjectdoubleInjobjectOut(jmethodID, jobject, double);
	static double jobjectIndoubleOut(jmethodID, jobject);
	static jobject staticTwojobjectInjobjectOut(jclass, jmethodID, jobject, jobject);
	static jobject jobjectInjobjectOut(jmethodID, jobject);
	static std::string jobjectInstringOut(jmethodID, jobject);

};
















#endif
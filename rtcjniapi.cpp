#include "rtcjniapi.h"

#include <iostream>

#define _INFO 0
using namespace std;



JavaVM * rtcjni::jvm;
JNIEnv * rtcjni::env;
JavaVMInitArgs rtcjni::vm_args;
jthrowable rtcjni::exc;


jclass rtcjni::_Curve;
jclass rtcjni::_CurveFactory;
jclass rtcjni::_CurveMath;
jclass rtcjni::_Segment;
jclass rtcjni::_SegmentList;

//////////Curve and CurveFactory methods
jmethodID rtcjni::_curve;
jmethodID rtcjni::_curve2M4d;
jmethodID rtcjni::_curve1M3d;
jmethodID rtcjni::_createLowerPJDCurve;
jmethodID rtcjni::_createUpperPJDCurve;
jmethodID rtcjni::_period;
jmethodID rtcjni::_hasPeriodicPart;
jmethodID rtcjni::_scaleX;
jmethodID rtcjni::_scaleY;
jmethodID rtcjni::_move;
jmethodID rtcjni::_name;
jmethodID rtcjni::_setname;
jmethodID rtcjni::_equals;
jmethodID rtcjni::_simplify;
jmethodID rtcjni::_y0epsilon;
jmethodID rtcjni::_pdx;
jmethodID rtcjni::_px0;
jmethodID rtcjni::_pdy;
jmethodID rtcjni::_clone;
jmethodID rtcjni::_aperiodicSegments;
jmethodID rtcjni::_tostring;
jmethodID rtcjni::_segmentsLT;

//////////CurveMath methods
jmethodID rtcjni::_maxVDist;
jmethodID rtcjni::_maxPlusConv;
jmethodID rtcjni::_maxPlusConv0;
jmethodID rtcjni::_add;
jmethodID rtcjni::_sub;
jmethodID rtcjni::_max;
jmethodID rtcjni::_min;

//////////Segment methods
jmethodID rtcjni::_s;
jmethodID rtcjni::_x;
jmethodID rtcjni::_segLTtoString;


//////////SegmentList methods
jmethodID rtcjni::_lastSegment;
jmethodID rtcjni::_trimLT;




void rtcjni::initialize(){
	JavaVMOption *options = new JavaVMOption[1];
	//options = new JavaVMOption[1];
	options[0].optionString = const_cast<char*> (".");
	int res;

	vm_args.version            = JNI_VERSION_1_6;
	vm_args.nOptions           = 1;
	vm_args.options            = options;
	vm_args.ignoreUnrecognized = true;

	res = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
	if (res != JNI_OK){
		cout << "rtcjni::initialize:creat JVM failed" << endl;
		handleException("rtcjni::initialize");
	}
	else{
		#if _INFO == 1
		cout << "rtcjni::initialize: JVM has been created" << endl;
		#endif
	}
		
	delete options;

	_Curve        = findclass("ch/ethz/rtc/kernel/Curve");
	_CurveFactory = findclass("ch/ethz/rtc/kernel/CurveFactory");
	_CurveMath    = findclass("ch/ethz/rtc/kernel/CurveMath");
	_Segment      = findclass("ch/ethz/rtc/kernel/Segment");
	_SegmentList  = findclass("ch/ethz/rtc/kernel/SegmentList");

	///////constructors
	_curve               = getmethodid(_Curve, "<init>", "([[D)V", _normal);
	_curve1M3d           = getmethodid(_Curve, "<init>", "([[DDJD)V", _normal);
	_curve2M4d           = getmethodid(_Curve, "<init>", "([[D[[DDDJD)V", _normal);
	_createLowerPJDCurve = getmethodid(_CurveFactory, "createLowerPJDCurve", 
	"(JDDLjava/lang/String;)Lch/ethz/rtc/kernel/Curve;", _static);
	_createUpperPJDCurve = getmethodid(_CurveFactory, "createUpperPJDCurve", 
	"(JDDLjava/lang/String;)Lch/ethz/rtc/kernel/Curve;", _static);

	///////Curve methods
	_scaleX            = getmethodid(_Curve, "scaleX", "(D)V", _normal);
	_scaleY            = getmethodid(_Curve, "scaleY", "(D)V", _normal);
	_move              = getmethodid(_Curve, "move", "(DD)V", _normal);
	_hasPeriodicPart   = getmethodid(_Curve, "hasPeriodicPart", "()Z", _normal);
	_period            = getmethodid(_Curve, "period", "()J", _normal);
	_setname           = getmethodid(_Curve, "setName", "(Ljava/lang/String;)V", _normal);
	_name              = getmethodid(_Curve, "name", "()Ljava/lang/String;", _normal);
	_equals            = getmethodid(_Curve, "equals", "(Ljava/lang/Object;)Z", _normal);
	_simplify          = getmethodid(_Curve, "simplify", "()V", _normal);
	_y0epsilon         = getmethodid(_Curve, "y0epsilon", "()D", _normal);
	_pdx               = getmethodid(_Curve, "pdx", "()D", _normal);
	_px0               = getmethodid(_Curve, "px0", "()D", _normal);
	_pdy               = getmethodid(_Curve, "pdy", "()D", _normal);
	_clone             = getmethodid(_Curve, "clone", "()Lch/ethz/rtc/kernel/Curve;", _normal);
	_aperiodicSegments = getmethodid(_Curve, "aperiodicSegments", 
	"()Lch/ethz/rtc/kernel/SegmentList;", _normal);
	_tostring          = getmethodid(_Curve, "toString", 
	"()Ljava/lang/String;", _normal);
	_segmentsLT 	   = getmethodid(_Curve, "segmentsLT", 
		"(D)Lch/ethz/rtc/kernel/SegmentList;", _normal);

	///////CurveMath methods
	_maxVDist     = getmethodid(_CurveMath, "maxVDist", 
	"(Lch/ethz/rtc/kernel/Curve;Lch/ethz/rtc/kernel/Curve;)D", _static);
	_maxPlusConv  = getmethodid(_CurveMath, "maxPlusConv", 
	"(Lch/ethz/rtc/kernel/Curve;Lch/ethz/rtc/kernel/Curve;)Lch/ethz/rtc/kernel/Curve;", _static);
	_maxPlusConv0 = getmethodid(_CurveMath, "maxPlusConv0", 
	"(Lch/ethz/rtc/kernel/Curve;)Lch/ethz/rtc/kernel/Curve;", _static);
	_add          = getmethodid(_CurveMath, "add", 
	"(Lch/ethz/rtc/kernel/Curve;Lch/ethz/rtc/kernel/Curve;)Lch/ethz/rtc/kernel/Curve;", _static);
	_sub          = getmethodid(_CurveMath, "sub", 
	"(Lch/ethz/rtc/kernel/Curve;Lch/ethz/rtc/kernel/Curve;)Lch/ethz/rtc/kernel/Curve;", _static);
	_max          = getmethodid(_CurveMath, "max", 
	"(Lch/ethz/rtc/kernel/Curve;Lch/ethz/rtc/kernel/Curve;)Lch/ethz/rtc/kernel/Curve;", _static);
	_min          = getmethodid(_CurveMath, "min", 
	"(Lch/ethz/rtc/kernel/Curve;Lch/ethz/rtc/kernel/Curve;)Lch/ethz/rtc/kernel/Curve;", _static);


	//////////Segment methods
	_s = getmethodid(_Segment, "s", "()D", _normal);
	_x = getmethodid(_Segment, "x", "()D", _normal);

	//////////SegmentList methods
	_lastSegment   = getmethodid(_SegmentList, "lastSegment", 
	"()Lch/ethz/rtc/kernel/Segment;", _normal);
	_trimLT        = getmethodid(_SegmentList, "trimLT", "(D)V", _normal);
	_segLTtoString = getmethodid(_SegmentList, "toString", 
		"()Ljava/lang/String;", _normal);

	cout << "rtcjniapi:: All the classes and methods are successfully found!" << endl;
}

void rtcjni::handleException(const string & debuginfo)
{
	exc = env->ExceptionOccurred();
	if (exc) {
		cout << "<---" << debuginfo << endl; 
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
}

jobject rtcjni::newobject(jclass cls, jmethodID mtd, const jvalue* arg){
	// callfun = "rtcjni::newobject<---" + callfun;
	jobject result = env->NewObjectA(cls, mtd, arg);
	handleException("rtcjni::newobject");
	return result;
}

jobjectArray rtcjni::vector2jni2darray(vector<double> vecin){
	// callfun = "rtcjni::vector2jni2darray<---" + callfun;
	unsigned cols = 3;
	if(vecin.size()%cols != 0 || vecin.size() < cols){
		cerr << "Input data is not correct!<---"  << endl;
		exit(RTC_EINPUT);
	}
	unsigned rows = vecin.size()/cols;
	double data[rows][3];
	// data = new double *[cols];
	// for (unsigned i = 0; i < rows; ++i){
	// 	data[i] = new double[cols];
	// }
	unsigned counter = 0;
	for (unsigned i = 0; i < rows; ++i){
		for (unsigned j = 0; j < cols; ++j){
			data[i][j] = vecin[counter];
			counter++;
		}
	}
	jobjectArray result = jni2darray(data, rows, cols);
	if(result==NULL){
		cerr << "Failed to create a jobjectArray!<---" << endl;
		exit(RTC_ERR);
	}
	else
		// delete data;
		return result;

}

jobjectArray rtcjni::jni2darray(double data[][3], unsigned nrows, unsigned ncols){
	// callfun = "rtcjni::jni2darray<---" + callfun;
	if(nrows==0 || ncols == 0){
		return NULL;
	}
	const jsize NumColumns = ncols;
	const jsize NumRows    = nrows;
	
	jdoubleArray row       = env->NewDoubleArray(NumColumns);
	jobjectArray ret       = env->NewObjectArray(NumRows, env->GetObjectClass(row), 0);
	for (unsigned i = 0; i < nrows; ++i){
		jdoubleArray row = env->NewDoubleArray(NumColumns);
		env->SetDoubleArrayRegion(row, (jsize)0, NumColumns, (jdouble *) data[i]);
		(env)->SetObjectArrayElement(ret, i, row);
		handleException("rtcjni::jni2darray");
	}
	return ret;
}

string rtcjni::jstring2string(jstring in){
	// callfun = "rtcjni::jstring2string<---" + callfun;
	string out = "";
	if(in == NULL){
		return out;
	}
	const char * strReturn = env->GetStringUTFChars(in, NULL);
	handleException("rtcjni::jstring2string");
	if (strReturn != NULL){
		out = string(strReturn);
		env->ReleaseStringUTFChars(in, strReturn);
	}
	return out;
}

jstring rtcjni::string2jstring(string in){
	// callfun        = "rtcjni::string2jstring<---"+callfun;
	jstring result = env->NewStringUTF(in.c_str());
	handleException("rtcjni::string2jstring");
	return result;
}


void rtcjni::jobjectdoubleInvoidOut(jmethodID mtd, jobject a, double d){
	// callfun = "rtcjni::jobjectdoubleInvoidOut<---" + callfun; 
	jvalue s; s.d = d;
	const jvalue  arg[1] = {s};
	callmethod(a, mtd, arg, _void);
}


jobject rtcjni::jobjectdoubleInjobjectOut(jmethodID mtd, jobject a, double d){
	// callfun = "rtcjni::jobjectdoubleInvoidOut<---" + callfun; 
	jobject result      = NULL;
	jvalue s; s.d = d;
	const jvalue  arg[1] = {s};
	jvalue ret = callmethod(a, mtd, arg, _object);
	result = ret.l;
	return result;
}

double rtcjni::jobjectIndoubleOut(jmethodID mtd, jobject a){
	// callfun            = "rtcjni::jobjectIndoubleOut<---" + callfun; 
	double result      = -1.0;
	const jvalue * arg = NULL;
	jvalue ret         = callmethod(a, mtd, arg, _double);
	result             = (double) ret.d;
	return result;
}

jobject rtcjni::staticTwojobjectInjobjectOut(jclass cls, jmethodID mtd, jobject a, 
	jobject b){
	// callfun             = "rtcjni::staticTwojobjectInjobjectOut<---" + callfun; 
	jobject result      = NULL;
	jvalue arga, argb; 
	arga.l              = a;  
	argb.l              = b; 
	const jvalue arg[2] = {arga, argb};
	jvalue ret          = callstaticmethod(cls, mtd, arg, _object);
	result              = ret.l;
	return result;
}

jobject rtcjni::jobjectInjobjectOut(jmethodID mtd, jobject a){
	// callfun            = "rtcjni::jobjectInjobjectOut<---" + callfun; 
	jobject result     = NULL;
	const jvalue * arg = NULL;
	jvalue ret         = callmethod(a, mtd, arg, _object);
	result             = ret.l;
	return result;
}

string rtcjni::jobjectInstringOut(jmethodID mtd, jobject a){
	// callfun            = "rtcjni::jobjectInstringOut<---" + callfun; 
	string result      = "";
	const jvalue * arg = NULL;
	jvalue ret         = callmethod(a, mtd, arg, _object);
	jstring _result    = (jstring)ret.l;
	result             = jstring2string(_result);
	return result;
}



jclass rtcjni::findclass(const char * classname){
	jclass ret = env->FindClass(classname);
	if ( ret == NULL){
		string info = "rtcjni::findclass: "+ string(classname) + " <---"
		+ "initialize()";
		handleException(info);
	}
	else{
		#if _INFO == 1
		cout << "Successfully finds class: \t\t" << classname << endl;
		#endif
	}
		

	return ret;
}

jmethodID rtcjni::getmethodid(jclass cls, const char * mtdname, const char * signature, 
	enum _method_type mtdtype){

	jmethodID ret;
	switch (mtdtype){
		case _normal:
		ret = env->GetMethodID(cls, mtdname, signature);
		break;
		case _static:
		ret = env->GetStaticMethodID(cls,  mtdname, signature);
		break;
		default:
		//cout << "rtcjni::getmethodid:not recognized method type" << endl;
		ret = NULL;
		break;
	}
	if (ret == NULL){
		string info = "rtcjni::getmethodid: method type: {"+ to_string(mtdtype) + "} <---"
		+ "initialize()";
		handleException(info);
	}
	else{
		#if _INFO == 1
		if (mtdtype == _static)
			cout << "Successfully gets static method: \t" << mtdname << endl;
		else
			cout << "Successfully gets method: \t\t" << mtdname << endl;
		#endif
	}
	return ret;
}

jvalue rtcjni::callmethod(jobject instance, jmethodID mtd, const jvalue* args, 
	enum _method_return_type mtdrettype){
	jvalue  ret ;
	// string info = "rtcjni::callmethod: return type: {"+ to_string(mtdrettype) + "} <---"
	//  + callingfunction;
	if (instance == NULL){
		cerr << "rtcjni::callmethod: invalid jobject <---"  << endl;
		exit(RTC_EINPUT);
	}
	if (mtd == NULL){
		cerr << "rtcjni::callmethod: invalid jmethodID <---"  << endl;
		exit(RTC_EINPUT);
	}

	switch (mtdrettype){
		case _void:{
			env->CallVoidMethodA(instance, mtd, args);
			break;
		}

		case _object:{
			jobject rl = env->CallObjectMethodA(instance, mtd, args);
			ret.l = rl;
			break;
		}

		case _bool:{
			jboolean rz = env->CallBooleanMethodA(instance, mtd, args);
			ret.z = rz;
			break;
		}
		case _byte:{
			jbyte rb = env->CallByteMethodA(instance, mtd, args);
			ret.b = rb;
			break;
		}

		case _char:{
			jchar rc = env->CallCharMethodA(instance, mtd, args);
			ret.c = rc;
			break;
		}

		case _short:{
			jshort rs = env->CallShortMethodA(instance, mtd, args);
			ret.s = rs;
			break;
		}

		case _int:{
			jint ri = env->CallIntMethodA(instance, mtd, args);
			ret.i = ri;
			break;
		}

		case _long:{
			jlong rj = env->CallLongMethodA(instance, mtd, args);
			ret.j = rj;
			break;
		}

		case _float:{
			jfloat rf = env->CallFloatMethodA(instance, mtd, args);
			ret.f = rf;
			break;
		}

		case _double:{
			jdouble rd = env->CallDoubleMethodA(instance, mtd, args);
			ret.d = rd;
			break;
		}

		default:
		cout << "rtcjni::callmethod:not recognized method return type <-- " << endl;
		break;
	}

	handleException("rtcjni::callmethod");
	return ret;
}


jvalue rtcjni::callstaticmethod(jclass cls, jmethodID mtd, const jvalue* args, 
	enum _method_return_type mtdrettype){

	// string info = "rtcjni::callstaticmethod: return type: {"+ to_string(mtdrettype) + "} <---";

	 if (cls == NULL || mtd == NULL){
		cerr << "rtcjni::callmethod: invalid jobject or jmethodID <---"  << endl;
		exit(RTC_EINPUT);
	}
	jvalue  ret;
	switch (mtdrettype){
		case _void:{
			env->CallStaticVoidMethodA(cls, mtd, args);
			break;
		}

		case _object:{
			jobject rl = env->CallStaticObjectMethodA(cls, mtd, args);
			ret.l = rl;
			break;
		}

		case _bool:{
			jboolean rz = env->CallStaticBooleanMethodA(cls, mtd, args);
			ret.z = rz;
			break;
		}

		case _byte:{
			jbyte rb = env->CallStaticByteMethodA(cls, mtd, args);
			ret.b = rb;
			break;
		}

		case _char:{
			jchar rc = env->CallStaticCharMethodA(cls, mtd, args);
			ret.c = rc;
			break;
		}

		case _short:{
			jshort rs = env->CallStaticShortMethodA(cls, mtd, args);
			ret.s = rs;
			break;
		}

		case _int:{
			jint ri = env->CallStaticIntMethodA(cls, mtd, args);
			ret.i = ri;
			break;
		}

		case _long:{
			jlong rj = env->CallStaticLongMethodA(cls, mtd, args);
			ret.j = rj;
			break;
		}

		case _float:{
			jfloat rf = env->CallStaticFloatMethodA(cls, mtd, args);
			ret.f = rf;
			break;
		}

		case _double:{
			jdouble rd = env->CallStaticDoubleMethodA(cls, mtd, args);
			//cout << "rtcjni::callstaticmethod: returned double is: " << rd << endl;
			ret.d = rd;
			break;
		}

		default:
		cerr << "rtcjni::callstaticmethod:not recognized method return type <--" << endl;
		exit(RTC_ERR);
		break;
	}
	
	handleException("rtcjni::callstaticmethod");
	return ret;
}



#include "PerformanceCounters.h"


#include <iostream>
#include <cstring>

extern "C" {
#include "../../third_party_lib/papi-5.5.1/src/testlib/papi_test.h"
}


#include "performance_counter/event_info.h"

using namespace std;


PerformanceCounters::PerformanceCounters(){
	initialized = false;
	counterNumber = 0;
}


bool PerformanceCounters::initialize(){
	int retval = PAPI_library_init( PAPI_VER_CURRENT );
	if ( retval != PAPI_VER_CURRENT ){
		cout << "Library PAPI: Performance Application Programming Interface initialize failed" << endl;
		exit(1);
	}
	initialized = true;	
	return true;
}

bool PerformanceCounters::addCounter(const string counterName){
	char* _counterName = new char[counterName.length() + 1];
	strcpy(_counterName, counterName.c_str());
	int _counterId;
	int retval = PAPI_event_name_to_code(_counterName, &_counterId);
	delete[] _counterName;
	if ( retval != PAPI_OK ){
		return false;
	}
	
	counters[counterNumber] = _counterId;
	counterNames.push_back(counterName);
	++counterNumber;
	return true;	 
}

bool PerformanceCounters::startAllCounters(){
	int retval = PAPI_start_counters( ( int * ) counters, counterNumber );
	if ( retval != PAPI_OK ){
		cout << "Library PAPI: Performance Application Programming Interface failed to start counters added by user" << endl;
		exit(1);
	}
	return true;
}


bool PerformanceCounters::readAllValues(){
	int retval = PAPI_read_counters( values, counterNumber );
	if ( retval != PAPI_OK ){
		return false;
	}
	// retval = PAPI_reset( EventSet ); 
	// if (retval != PAPI_OK){
	// 	return false;
	// }
	return true;
}

long long PerformanceCounters::getCounterValue(const std::string counterName){
	int id = -1;
	for (int i = 0; i < counterNumber; ++i)
	{
		if (counterNames[i] == counterName){
			id = i;
			break;
		}
	}
	return getCounterValue(id);
}

long long PerformanceCounters::getCounterValue(int counterId){
	if (counterId < 0 || counterId >= counterNumber){
		return 0;
	}
	return values[counterId];
}
		
int PerformanceCounters::getCounterNumber(){
	return counterNumber;
}

void PerformanceCounters::endAllCounters(){
	/* Remove all events in the eventset */
// if (PAPI_cleanup_eventset(counters) != PAPI_OK)
//   cout << "Library PAPI: Performance Application Programming Interface failed to clean counters added by user" << endl;

// /* Free all memory and data structures, EventSet must be empty. */
// if (PAPI_destroy_eventset(counters) != PAPI_OK)
//   cout << "Library PAPI: Performance Application Programming Interface failed to destroy counters added by user" << endl;
}	





void PerformanceCounters::printCounterInfo(){
	int argc = 1;
	char _method[50] = "PerformanceCounters::printCounterInfo";
	char * p = _method;
	char ** argv = &p;
	print_info( argc, argv);
}














static void
print_help( char **argv )
{
	printf( "Usage: %s [options] [EVENTNAMEs]\n", argv[0] );
	printf( "Options:\n\n" );
	printf( "General command options:\n" );
	printf( "\t-u          Display output values as unsigned integers\n" );
	printf( "\t-x          Display output values as hexadecimal\n" );
	printf( "\t-h          Print this help message\n" );
	printf( "\tEVENTNAMEs  Specify one or more preset or native events\n" );
	printf( "\n" );
	printf( "This utility performs work while measuring the specified events.\n" );
	printf( "It can be useful for sanity checks on given events and sets of events.\n" );
}





void PerformanceCounters::PAPI_test(int argc, char **argv){

	int retval;
	int num_events;
	long long *values;
	char *success;
	PAPI_event_info_t info;
	int EventSet = PAPI_NULL;
	int i, j, event, data_type = PAPI_DATATYPE_INT64;
	int u_format = 0;
	int hex_format = 0;

	tests_quiet( argc, argv );	/* Set TESTS_QUIET variable */

	if ( ( retval =
		   PAPI_library_init( PAPI_VER_CURRENT ) ) != PAPI_VER_CURRENT )
		test_fail( __FILE__, __LINE__, "PAPI_library_init", retval );

	if ( ( retval = PAPI_create_eventset( &EventSet ) ) != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_create_eventset", retval );

	/* Automatically pass if no events, for run_tests.sh */
	if ((( TESTS_QUIET ) && ( argc == 2)) || ( argc == 1 )) {
		test_pass( __FILE__, NULL, 0 );
	}

	values =
		( long long * ) malloc( sizeof ( long long ) * ( size_t ) argc );
	success = ( char * ) malloc( ( size_t ) argc );

	if ( success == NULL || values == NULL )
		test_fail_exit( __FILE__, __LINE__, "malloc", PAPI_ESYS );

	for ( num_events = 0, i = 1; i < argc; i++ ) {
		if ( !strcmp( argv[i], "-h" ) ) {
			print_help( argv );
			exit( 1 );
		} else if ( !strcmp( argv[i], "-u" ) ) {
			u_format = 1;
		} else if ( !strcmp( argv[i], "-x" ) ) {
			hex_format = 1;
		} else {
			if ( ( retval = PAPI_add_named_event( EventSet, argv[i] ) ) != PAPI_OK ) {
				printf( "Failed adding: %s\nbecause: %s\n", argv[i], 
					PAPI_strerror(retval));
			} else {
				success[num_events++] = i;
				printf( "Successfully added: %s\n", argv[i] );
			}
		}
	}

	/* Automatically pass if no events, for run_tests.sh */
	if ( num_events == 0 ) {
		test_pass( __FILE__, NULL, 0 );
	}


	printf( "\n" );

	do_flops( 1 );
	do_flush(  );

	if ( ( retval = PAPI_start( EventSet ) ) != PAPI_OK ) {
	   test_fail_exit( __FILE__, __LINE__, "PAPI_start", retval );
	}

	do_flops( NUM_FLOPS );
	do_misses( 1, L1_MISS_BUFFER_SIZE_INTS );

	if ( ( retval = PAPI_stop( EventSet, values ) ) != PAPI_OK )
		test_fail_exit( __FILE__, __LINE__, "PAPI_stop", retval );

	for ( j = 0; j < num_events; j++ ) {
		i = success[j];
		if (! (u_format || hex_format) ) {
			retval = PAPI_event_name_to_code( argv[i], &event );
			if (retval == PAPI_OK) {
				retval = PAPI_get_event_info(event, &info);
				if (retval == PAPI_OK) data_type = info.data_type;
				else data_type = PAPI_DATATYPE_INT64;
			}
			switch (data_type) {
			  case PAPI_DATATYPE_UINT64:
				printf( "%s : \t%llu(u)", argv[i], (unsigned long long)values[j] );
				break;
			  case PAPI_DATATYPE_FP64:
				printf( "%s : \t%0.3f", argv[i], *((double *)(&values[j])) );
				break;
			  case PAPI_DATATYPE_BIT64:
				printf( "%s : \t%#llX", argv[i], values[j] );
				break;
			  case PAPI_DATATYPE_INT64:
			  default:
				printf( "%s : \t%lld", argv[i], values[j] );
				break;
			}
			if (retval == PAPI_OK)  printf( " %s", info.units );
			printf( "\n" );
		}
		if (u_format) printf( "%s : \t%llu(u)\n", argv[i], (unsigned long long)values[j] );
		if (hex_format) printf( "%s : \t%#llX\n", argv[i], values[j] );
	}

	printf( "\n----------------------------------\n" );
	printf
		( "Verification: Checks for valid event name.\n This utility lets you add events from the command line interface to see if they work.\n" );
	test_pass( __FILE__, NULL, 0 );
	exit( 1 );
}
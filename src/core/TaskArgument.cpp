#include "core/TaskArgument.h"

#include <iostream>

#include "utils/utils.h"
#include "utils/TimeUtil.h"

using namespace std;

TaskArgument::TaskArgument() : name()
{
   _periodicity = periodic;
   _load_type   = busywait;
   _type        = singlecore;

   taskId = 0;

   period.tv_sec    = 0;
   period.tv_nsec   = 0;
   jitter           = period;
   release_time     = jitter;
   relativeDeadline = period;

   user_defined_load_id = 1;
   benchmark_id         = 1;
   default_coreId       = -1;
}

void TaskArgument::print()
{
   cout << "task name              = " << name << endl;
   cout << "task Id                = " << taskId << endl;
   cout << "task _periodicity      = " << _periodicity << endl;
   cout << "task _load_type        = " << _load_type << endl;
   cout << "task _type             = " << _type << endl;
   cout << "default_coreId         = " << default_coreId << endl;

   cout << "period (ms)            = " << TimeUtil::convert_ms(period) << endl;
   cout << "jitter (ms)            = " << TimeUtil::convert_ms(jitter) << endl;
   cout << "release_time (ms)      = " << TimeUtil::convert_ms(release_time) << endl;
   cout << "relative deadline (ms) = " << TimeUtil::convert_ms(relativeDeadline) << endl;

   cout << "benchmark name         = " << benchmark_name << endl;
   cout << "benchmark id           = " << benchmark_id << endl;
   cout << "user defined load id   = " << user_defined_load_id << endl;
   displayvector(wcets_us, "wcets_us (us)");
   // displayvector(TimeUtil::convert_us(wcets), "wcets");
}

int TaskArgument::getLoadId()
{
   if (_load_type == benchmark)
   {
      return(benchmark_id);
   }
   else
   {
      return(user_defined_load_id);
   }
}

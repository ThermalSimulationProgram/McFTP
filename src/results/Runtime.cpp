#include "results/Runtime.h"

#include <sstream>

#include "utils/TimeUtil.h"

using namespace std;
/********************* MEMBER FUNCTIONS *********************/

///This sets the thread type, id, and runtime of trace
void Runtime::setRuntime(enum _thread_type _type, unsigned int _id, struct timespec _rt)
{
   type    = _type;
   id      = _id;
   runtime = _rt;
}

///This function converts the runtime statistic to a string,
string Runtime::toString()
{
   stringstream out;

   out << type << "," << id << "," << TimeUtil::convert_us(runtime);

   return(out.str());
}

/*********** GETTER FUNCTIONS ***********/

///This function returns the ID of the trace
unsigned int Runtime::getId()
{
   return(id);
}

/*********** OPERATORS ***********/

///This operator defines the sorting criteria to be the statistic's id
int Runtime::operator<(Runtime b)
{
   return(this->getId() < b.getId());
}

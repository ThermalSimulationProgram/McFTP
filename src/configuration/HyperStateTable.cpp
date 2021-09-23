#include "configuration/HyperStateTable.h"

#include <iostream>
#include <climits>

#include "utils/TimeUtil.h"
#include "utils/Operators.h"

// constructor need nothing
HyperStateTable::HyperStateTable() :
   statetable(0)
{
   // default values
   rowId                  = 0;
   tableSize              = 0;
   nextActionTime.tv_sec  = LONG_MAX;
   nextActionTime.tv_nsec = 999999999;
}

// destructor
HyperStateTable::~HyperStateTable()
{
}

// set the state table, current time is needed to update nextActionTime
void HyperStateTable::setStateTable(const StateTable& s, const struct timespec &now)
{
   // we need to implement the new power state immediately
   nextActionTime = now;
   statetable     = s;
   tableSize      = statetable.getSize();
   // from the top row of the state table
   rowId = 0;
}

// get the new power state and the end time of this state. Internel states
// will be updated.
double HyperStateTable::getState(const struct timespec &now, struct timespec &length)
{
   // the new power state.
   // ret == 0 means the idle state
   // a positive ret indicates the clock frequency
   double ret;

   // if we have a non-empty state table
   if (tableSize > 0)
   {
      // return the current power state
      ret = statetable.getFrequency(rowId);
      // how long this state should last, unit: microsecond
      unsigned long lastLength = statetable.getLength(rowId);
      // std::cout << "lastLength: " << lastLength << std::endl;

      length = TimeUtil::Micros(lastLength);

      // update the nextActionTime
      nextActionTime = nextActionTime + length;

      // if we reach the bottom of the table, go to the top
      if (rowId >= tableSize - 1)
      {
         rowId = 0;
      }
      else
      {
         // otherwise, go to next row
         ++rowId;
      }
   }
   else
   {
      // empty state table returns an invalid power state
      length = TimeUtil::Micros(0);
      ret    = -1;        // default: no control
   }

   return(ret);
}

// nondestructively reading the nextActionTime
struct timespec HyperStateTable::peekNextTime()
{
   return(nextActionTime);
}

void HyperStateTable::print()
{
   statetable.print();
}

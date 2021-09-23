#ifndef HYPERSTATETABLE
#define HYPERSTATETABLE


#include <time.h>

#include "configuration/StateTable.h"

// HyperStateTable is defined as an interface to use
// the StateTable

class HyperStateTable {
private:
   // the state table
   StateTable statetable;

   // when the next power state should be implemented
   struct timespec nextActionTime;

   // the index of the current row in the state table
   int rowId;

   // the number of the rows of the state table
   int tableSize;

public:

   // constructor need nothing
   HyperStateTable();

   // destructor
   ~HyperStateTable();

   // set the state table
   void setStateTable(const StateTable& s, const struct timespec &now);

   // nondestructively reading the nextActionTime
   struct timespec peekNextTime();

   // get the new power state and the end time of this state. Internel states
   // will be updated.
   double getState(const struct timespec &now, struct timespec &length);

   void print();
};


#endif

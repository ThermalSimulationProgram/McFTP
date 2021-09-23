#ifndef STATETABLE_H
#define STATETABLE_H

#include <vector>

// This class defines a state table
class StateTable {
protected:
   // vector storing the sequency of frequencies for a core. The frequency must
   // be zero or one of the feasible frequencies.
   std::vector <double> frequencies;

   // vector storing the time lengths of the states, unit microsecond
   std::vector <unsigned long> lengths;

   // the index of the core on which this state table is implemented
   int coreId;

public:
   //Constructor needs the index of the core
   StateTable(int);

   ~StateTable();

   // append a new state at the tail, the length of the state (in microsecond) should
   // also be given. Any length less than 100 microsecond will be revised to 100 us
   void pushState(double f, unsigned long l);

   // get the power state and its length at a row, indexed by id.
   void getState(int id, double& f, unsigned long& l);

   // get the power state at a row, indexed by id.
   double getFrequency(int id);

   // get the length of the power state at a row, indexed by id.
   unsigned long getLength(int id);

   // get how many states in this state table
   unsigned getSize();

   // get the index of the core
   int getTargetCoreId() const;

   void print();
};



#endif

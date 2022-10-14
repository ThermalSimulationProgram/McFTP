#ifndef VECTORED_DATA_H
#define VECTORED_DATA_H

#include <vector>
#include <string>
#include <sstream>

#include "utils/TimeUtil.h"
#include "utils/utils.h"

template <class T>
class VectoredData
{
protected:
   // this vector stores the data
   std::vector <T> data;

   // This variable stores the id of the thread
   unsigned int thread_id;

   // This variable stores the time when the data is stored
   unsigned long storedTime;


public:

   VectoredData(const std::vector <T>& d, unsigned int id) : data(d)
   {
      thread_id  = id;
      storedTime = TimeUtil::getTimeUSec();
   }

   ~VectoredData()
   {
   }

   std::string toString()
   {
      std::stringstream sout;

      sout << thread_id << "," << storedTime << "," << vectorTostring(data);

      return(sout.str());
   }

   std::vector <T> getData()
   {
      return(data);
   }

   unsigned int getId();

   unsigned long getStoredTime();
};



#endif

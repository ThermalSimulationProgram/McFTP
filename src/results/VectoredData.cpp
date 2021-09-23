#include "results/VectoredData.h"

template <class T>
unsigned int VectoredData <T>::getId()
{
   return(thread_id);
}

template <class T>
unsigned long VectoredData <T>::getStoredTime()
{
   return(storedTime);
}

#ifndef _VECTORMATH_H
#define _VECTORMATH_H

#include <algorithm>

#include "utils/vectormath_util.h"
#include "utils/vectormath_calc.h"
#include "utils/vectormath_prop.h"
#include "utils/vectormath_access.h"



template <typename T>
std::vector <int> sort_indexes(const std::vector <T>&v)
{
   // initialize original index locations
   std::vector <int> idx(v.size());

   iota(idx.begin(), idx.end(), 0);

   // sort indexes based on comparing values in v
   sort(idx.begin(), idx.end(),
        [&v](int i1, int i2) {
      return(v[i1] > v[i2]);
   });
   return(idx);
}

template <typename T> std::vector <T> singlesVector(unsigned _length, T sample)
{
   std::vector <T> ret(_length, sample);

   return(ret);
}

#endif

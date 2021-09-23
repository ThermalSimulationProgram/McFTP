#ifndef _VECTORMATHCALC_H
#define _VECTORMATHCALC_H

#include "utils/vectormath_util.h"
// #include "utils.h"

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

// using namespace std;



template < typename _Iter, typename _Iter2, typename _Iter3 >
void __vectorsOperationCore(
   _Iter __start1, _Iter __end1, _Iter2 __start2, _Iter2 __end2,
   enum _vector_operator __operation, _Iter3 __result)
{
   if (std::distance(__start1, __end1) !=
       std::distance(__start2, __end2))
   {
      std::cout << "error while trying doing operation: " << __operation << std::endl;
      std::cerr << "vectormath::vectorOperationCore:vector dimensions does not match\n";
      exit(1);
   }
   _Iter  __temp1 = __start1;
   _Iter2 __temp2 = __start2;

   switch (__operation)
   {
   case  __vominus: {
      for (; __temp1 != __end1; ++__temp1, ++__temp2, ++__result)
      {
         __minus(__temp1, __temp2, __result);
      }
   }

   case __voplus: {
      for (; __temp1 != __end1; ++__temp1, ++__temp2, ++__result)
      {
         __plus(__temp1, __temp2, __result);
      }
   }

   case __votimes: {
      for (; __temp1 != __end1; ++__temp1, ++__temp2, ++__result)
      {
         __times(__temp1, __temp2, __result);
      }
   }

   case __vodivide: {
      for (; __temp1 != __end1; ++__temp1, ++__temp2, ++__result)
      {
         __divide(__temp1, __temp2, __result);
      }
   }

   case __volarger: {
      for (; __temp1 != __end1; ++__temp1, ++__temp2, ++__result)
      {
         __largerthan(__temp1, __temp2, __result);
      }
   }

   case __voless: {
      for (; __temp1 != __end1; ++__temp1, ++__temp2, ++__result)
      {
         __lessthan(__temp1, __temp2, __result);
      }
   }

   case __voequal: {
      for (; __temp1 != __end1; ++__temp1, ++__temp2, ++__result)
      {
         __equalwith(__temp1, __temp2, __result);
      }
   }

   case __volesseq: {
      for (; __temp1 != __end1; ++__temp1, ++__temp2, ++__result)
      {
         __lessequal(__temp1, __temp2, __result);
      }
   }

   case __volargereq: {
      for (; __temp1 != __end1; ++__temp1, ++__temp2, ++__result)
      {
         __largerequal(__temp1, __temp2, __result);
      }
   }

   case __vonoteq: {
      for (; __temp1 != __end1; ++__temp1, ++__temp2, ++__result)
      {
         __noteq(__temp1, __temp2, __result);
      }
   }

   case __voand: {
      for (; __temp1 != __end1; ++__temp1, ++__temp2, ++__result)
      {
         __andop(__temp1, __temp2, __result);
      }
   }

   case __voor: {
      for (; __temp1 != __end1; ++__temp1, ++__temp2, ++__result)
      {
         __orop(__temp1, __temp2, __result);
      }
   }
   }
}

template < typename _Iter, typename _T1, typename _Iter2 >
void __vectorScalarOperationCore(
   _Iter __start, _Iter __end, _T1 __b, enum _vector_operator __operation,
   _Iter2 __result)
{
   std::vector < _T1 > temp(std::distance(__start, __end), __b);

   __vectorsOperationCore(__start, __end, temp.begin(),
                          temp.end(), __operation, __result);
}

template < typename _Iter, typename _T1, typename _Iter2 >
void __vectorScalarOperationCore(
   _T1 __b, _Iter __start, _Iter __end, enum _vector_operator __operation,
   _Iter2 __result)
{
   std::vector < _T1 > temp(std::distance(__start, __end), __b);
   __vectorsOperationCore(temp.begin(), temp.end(), __start,
                          __end, __operation, __result);
}

template < typename _T, typename _T1 >
std::vector < _T > operator - (const std::vector < _T > &__a, _T1 __b){
   std::vector < _T > ret;
   __vectorScalarOperationCore(__a.begin(), __a.end(), __b,
                               __vominus, back_inserter(ret));
   return(ret);
}
template < typename _T, typename _T1 >
std::vector < _T > operator - (_T1 __b, const std::vector < _T > &__a){
   std::vector < _T > ret;
   __vectorScalarOperationCore(__b, __a.begin(), __a.end(),
                               __vominus, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < _T > operator - (const std::vector < _T > &__a, const std::vector < _T1 > &__b){
   std::vector < _T > ret;
   __vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
                          __b.end(), __vominus, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < _T > operator + (const std::vector < _T > &__a, _T1 __b){
   std::vector < _T > ret;
   __vectorScalarOperationCore(__a.begin(), __a.end(), __b,
                               __voplus, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < _T > operator + (_T1 __b, const std::vector < _T > &__a){
   std::vector < _T > ret;
   __vectorScalarOperationCore(__b, __a.begin(), __a.end(),
                               __voplus, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < _T > operator + (const std::vector < _T > &__a, const std::vector < _T1 > &__b){
   std::vector < _T > ret;
   __vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
                          __b.end(), __voplus, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < double > operator * (const std::vector < _T > &__a, _T1 __b){
   std::vector < double > ret;
   __vectorScalarOperationCore(__a.begin(), __a.end(), __b,
                               __votimes, back_inserter(ret));
   return(ret);
   // return __a;
}
template < typename _T, typename _T1 >
std::vector < double > operator * (_T1 __b, const std::vector < _T > &__a){
   std::vector < double > ret;
   __vectorScalarOperationCore(__b, __a.begin(), __a.end(),
                               __votimes, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < double > operator * (const std::vector < _T > &__a, const std::vector < _T1 > &__b){
   std::vector < double > ret;
   __vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
                          __b.end(), __votimes, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < double > operator / (const std::vector < _T > &__a, _T1 __b){
   std::vector < double > ret;
   __vectorScalarOperationCore(__a.begin(), __a.end(), __b,
                               __vodivide, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < double > operator / (_T1 __b, const std::vector < _T > &__a){
   std::vector < double > ret;
   __vectorScalarOperationCore(__b, __a.begin(), __a.end(),
                               __vodivide, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < double > operator / (const std::vector < _T > &__a, const std::vector < _T1 > &__b){
   std::vector < double > ret;
   __vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
                          __b.end(), __vodivide, back_inserter(ret));
   return(ret);
}


template < typename _T, typename _T1 >
std::vector < bool > operator > (const std::vector < _T > &__a, _T1 __b){
   std::vector < bool > ret;
   __vectorScalarOperationCore(__a.begin(), __a.end(), __b,
                               __volarger, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < bool > operator > (_T1 __b, const std::vector < _T > &__a){
   std::vector < bool > ret;
   __vectorScalarOperationCore(__b, __a.begin(), __a.end(),
                               __volarger, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < bool > vectorLarger(const std::vector < _T > &__a, const std::vector < _T1 > &__b)
{
   std::vector < bool > ret;
   __vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
                          __b.end(), __volarger, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < bool > operator < (const std::vector < _T > &__a, _T1 __b){
   std::vector < bool > ret;
   __vectorScalarOperationCore(__a.begin(), __a.end(), __b,
                               __voless, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < bool > operator < (_T1 __b, const std::vector < _T > &__a){
   std::vector < bool > ret;
   __vectorScalarOperationCore(__b, __a.begin(), __a.end(),
                               __voless, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < bool > vectorLess(const std::vector < _T > &__a, const std::vector < _T1 > &__b)
{
   std::vector < bool > ret;
   __vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
                          __b.end(), __voless, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < bool > operator == (const std::vector < _T > &__a,
                                  _T1 __b){
   std::vector < bool > ret;
   __vectorScalarOperationCore(__a.begin(), __a.end(), __b,
                               __voequal, back_inserter(ret));
   // dumpMatrix(ret);

   return(ret);
}

template < typename _T, typename _T1 >
std::vector < bool > operator == (_T1 __b, const std::vector < _T > &__a){
   std::vector < bool > ret;
   __vectorScalarOperationCore(__b, __a.begin(), __a.end(),
                               __voequal, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < bool > vectorEqual(const std::vector < _T > &__a, const std::vector < _T1 > &__b)
{
   std::vector < bool > ret;
   __vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
                          __b.end(), __voequal, back_inserter(ret));
   return(ret);
}



template < typename _T, typename _T1 >
std::vector < bool > operator <= (const std::vector < _T > &__a, _T1 __b){
   std::vector < bool > ret;
   __vectorScalarOperationCore(__a.begin(), __a.end(), __b,
                               __volesseq, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < bool > operator <= (_T1 __b, const std::vector < _T > &__a){
   std::vector < bool > ret;
   __vectorScalarOperationCore(__b, __a.begin(), __a.end(),
                               __volesseq, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < bool > vectorLessEq(const std::vector < _T > &__a, const std::vector < _T1 > &__b)
{
   std::vector < bool > ret;
   __vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
                          __b.end(), __volesseq, back_inserter(ret));
   return(ret);
}


template < typename _T, typename _T1 >
std::vector < bool > operator >= (const std::vector < _T > &__a, _T1 __b){
   std::vector < bool > ret;
   __vectorScalarOperationCore(__a.begin(), __a.end(), __b,
                               __volargereq, back_inserter(ret));
   return(ret);
   // return __a;
}

template < typename _T, typename _T1 >
std::vector < bool > operator >= (_T1 __b, const std::vector < _T > &__a){
   std::vector < bool > ret;
   __vectorScalarOperationCore(__b, __a.begin(), __a.end(),
                               __volargereq, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < bool > vectorLargerEq(const std::vector < _T > &__a, const std::vector < _T1 > &__b)
{
   std::vector < bool > ret;
   __vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
                          __b.end(), __volargereq, back_inserter(ret));
   return(ret);
}

template < typename _T, typename _T1 >
std::vector < bool > operator & (const std::vector < _T > &__a, std::vector < _T1 > __b){
   std::vector < bool > ret;
   __vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
                          __b.end(), __voand, back_inserter(ret));
   return(ret);
}



template < typename T > std::vector < double > vecFloor(const std::vector < T > &a)
{
   std::vector < double > ret;
   for (unsigned i = 0; i < a.size(); ++i)
   {
      ret.push_back((double)std::floor(a[i]));
   }
   return(ret);
}



#endif

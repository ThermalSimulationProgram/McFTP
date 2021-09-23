#ifndef _VECTORMATHUTIL_H
#define _VECTORMATHUTIL_H

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

// using namespace std;

#define EPSILON    (0.00000001)

std::vector < int > integerVector(int start, int end);
std::vector < int > integerVector(int start, int end, int interval);

bool vectorAny(const std::vector < bool >&);
bool vectorAll(const std::vector < bool >&);


template < typename T > bool isMatrix(
   const std::vector < std::vector < T >>& a)
{
   int nrows = a.size();

   if (nrows > 0)
   {
      int ncolms = a[0].size();
      for (int i = 0; i < nrows; ++i)
      {
         if ((int)a[i].size() != ncolms)
         {
            return(false);
         }
      }
   }

   return(true);
}

template < typename T > inline
bool largerEqThan(T x, T y)
{
   return(x >= y);
}

template < typename T > inline
bool largerThan(T x, T y)
{
   return(x > y);
}

template < typename T > inline
bool smallerEqThan(T x, T y)
{
   return(x <= y);
}

template < typename T > inline
bool smallerThan(T x, T y)
{
   return(x < y);
}

enum _vector_operator
{
   __voplus,
   __vominus,
   __votimes,
   __vodivide,
   __volarger,
   __voless,
   __voequal,
   __volesseq,
   __volargereq,
   __vonoteq,
   __voand,
   __voor,
};

template < typename _Iter, typename _Iter2, typename _Iter3 >
inline void __plus(_Iter __a, _Iter2 __b, _Iter3 __c)
{
   *__c = *__a + (*__b);
}

template < typename _Iter, typename _Iter2, typename _Iter3 >
inline void __minus(_Iter __a, _Iter2 __b, _Iter3 __c)
{
   *__c = *__a - (*__b);
}

template < typename _Iter, typename _Iter2, typename _Iter3 >
inline void __times(_Iter __a, _Iter2 __b, _Iter3 __c)
{
   *__c = *__a * (*__b);
}

template < typename _Iter, typename _Iter2, typename _Iter3 >
inline void __divide(_Iter __a, _Iter2 __b, _Iter3 __c)
{
   if (std::abs((*__b)) <= EPSILON)
   {
      std::cout << "__a:" << *__a << std::endl;
      std::cout << "__b:" << *__b << std::endl;
      std::cerr << "vectormath::__divided:divided by zero\n";
      exit(1);
   }
   *__c = (double)*__a / (*__b);
}

template < typename _Iter, typename _Iter2, typename _Iter3 >
inline void __largerthan(_Iter __a, _Iter2 __b, _Iter3 __c)
{
   *__c = (double)(*__a) > (double)(*__b);
}

template < typename _Iter, typename _Iter2, typename _Iter3 >
inline void __lessthan(_Iter __a, _Iter2 __b, _Iter3 __c)
{
   *__c = (double)(*__a) < (double)(*__b);
}

template < typename _Iter, typename _Iter2, typename _Iter3 >
inline void __equalwith(_Iter __a, _Iter2 __b, _Iter3 __c)
{
   *__c = (std::abs((double)(*__a) - (double)(*__b)) < EPSILON);
}

template < typename _Iter, typename _Iter2, typename _Iter3 >
inline void __lessequal(_Iter __a, _Iter2 __b, _Iter3 __c)
{
   *__c = ((double)(*__a) < (double)(*__b)) ||
          (std::abs((double)(*__a) - (double)(*__b)) < EPSILON);
}

template < typename _Iter, typename _Iter2, typename _Iter3 >
inline void __largerequal(_Iter __a, _Iter2 __b, _Iter3 __c)
{
   *__c = ((double)(*__a) > (double)(*__b)) ||
          (std::abs((double)(*__a) - (double)(*__b)) < EPSILON);
}

template < typename _Iter, typename _Iter2, typename _Iter3 >
inline void __noteq(_Iter __a, _Iter2 __b, _Iter3 __c)
{
   *__c = (std::abs((double)(*__a) - (double)(*__b)) >= EPSILON);
}

template < typename _Iter, typename _Iter2, typename _Iter3 >
inline void __andop(_Iter __a, _Iter2 __b, _Iter3 __c)
{
   *__c = *__a && (*__b);
}

template < typename _Iter, typename _Iter2, typename _Iter3 >
inline void __orop(_Iter __a, _Iter2 __b, _Iter3 __c)
{
   *__c = *__a || (*__b);
}

#endif

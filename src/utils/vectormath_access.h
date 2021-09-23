#ifndef _VECTORMATHACCESS_H
#define _VECTORMATHACCESS_H

#include "utils/vectormath_util.h"


#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

// using namespace std;


template < typename T, typename T1 > void vectorAssign(std::vector < T >& pvec,
                                                       const std::vector < int >& ids, const std::vector < T1 >& values)
{
   int numel = (int)pvec.size();

   if (ids.size() != values.size())
   {
      std::cerr << "vectormath::vectorAssign:Input index does not match the values\n";
      exit(1);
   }
   for (unsigned i = 0; i < ids.size(); ++i)
   {
      if (ids[i] >= numel)
      {
         std::cerr << "vectormath::vectorAssign:Input index exceeds valid range of input vector\n";
         exit(1);
      }
      if (ids[i] < 0)
      {
         std::cerr << "vectormath::vectorAssign:Input index is negative\n";
         exit(1);
      }
      pvec[ids[i]] = (T)values[i];
   }
}

template < typename T, typename T1 > void vectorAssign(std::vector < T >& pvec,
                                                       const std::vector < bool >& logicalIds, const std::vector < T1 >& values)
{
   unsigned nvalues = values.size();
   unsigned numel   = pvec.size();
   unsigned counter = 0;

   for (unsigned i = 0; i < logicalIds.size(); ++i)
   {
      if (i >= numel)
      {
         break;
      }

      if (logicalIds[i])
      {
         if (counter >= nvalues)
         {
            std::cerr << "vectormath::vectorAssign:Input index does not match the values\n";
            exit(1);
         }
         pvec[i] = (T)values[counter];
         counter++;
      }
   }
}

template < typename T, typename T1 > void vectorAssign(std::vector < T >& pvec,
                                                       const std::vector < int >& ids, const T1& value)
{
   int numel = (int)pvec.size();

   for (unsigned i = 0; i < ids.size(); ++i)
   {
      if (ids[i] >= numel)
      {
         std::cerr << "vectormath::vectorAssign:Input index exceeds valid range of input vector\n";
         exit(1);
      }
      if (ids[i] < 0)
      {
         std::cerr << "vectormath::vectorAssign:Input index is negative\n";
         exit(1);
      }
      pvec[ids[i]] = (T)value;
   }
}

template < typename T, typename T1 >
void vectorAssign(std::vector < std::vector < T >>& pvec, const std::vector < int >& rowIds,
                  const int&  colId, const std::vector < T1 >& values)
{
   unsigned nrows = pvec.size();

   if (rowIds.size() != values.size())
   {
      std::cerr << "vectormath::vectorAssign:Input index does not match the values\n";
      exit(1);
   }
   if (colId < 0)
   {
      std::cerr << "vectormath::vectorAssign: Column Input index is negative\n";
      exit(1);
   }

   for (unsigned i = 0; i < rowIds.size(); ++i)
   {
      if (rowIds[i] >= nrows || colId >= pvec[rowIds[i]].size())
      {
         std::cerr << "vectormath::vectorAssign:Input index exceeds valid range of input vector\n";
         exit(1);
      }
      if (rowIds[i] < 0)
      {
         std::cerr << "vectormath::vectorAssign: Row Input index is negative\n";
         exit(1);
      }

      pvec[rowIds[i]][colId] = (T)values[i];
   }
}

template < typename T, typename T1 >
void vectorAssign(std::vector < std::vector < T >>& pvec, const std::vector < int >& rowIds,
                  const int&  colId, const T1& value)
{
   int nrows = (int)pvec.size();

   if (colId < 0)
   {
      std::cerr << "vectormath::vectorAssign: Column Input index is negative\n";
      exit(1);
   }
   for (unsigned i = 0; i < rowIds.size(); ++i)
   {
      if (rowIds[i] >= nrows || colId >= (int)pvec[rowIds[i]].size())
      {
         std::cerr << "vectormath::vectorAssign:Input index exceeds valid range of input vector\n";
         exit(1);
      }
      if (rowIds[i] < 0)
      {
         std::cerr << "vectormath::vectorAssign: Row Input index is negative\n";
         exit(1);
      }
      pvec[rowIds[i]][colId] = (T)value;
   }
}

template < typename T, typename T1 >
void vectorAssign(std::vector < std::vector < T >>& pvec, const int& rowId,
                  const std::vector < int >& colIds, const std::vector < T1 >& values)
{
   unsigned nrows = pvec.size();

   if (rowId >= nrows)
   {
      std::cerr << "vectormath::vectorAssign:Invalid input row index\n";
      exit(1);
   }
   if (rowId < 0)
   {
      std::cerr << "vectormath::vectorAssign: Row Input index is negative\n";
      exit(1);
   }
   if (colIds.size() != values.size())
   {
      std::cerr << "vectormath::vectorAssign:Input index does not match the values\n";
      exit(1);
   }
   unsigned ncols = pvec[rowId].size();

   for (unsigned i = 0; i < colIds.size(); ++i)
   {
      if (colIds[i] >= ncols)
      {
         std::cerr << "vectormath::vectorAssign:Invalid input index\n";
         exit(1);
      }
      if (colIds[i] < 0)
      {
         std::cerr << "vectormath::vectorAssign:Column Input index is negative\n";
         exit(1);
      }
      pvec[rowId][colIds[i]] = (T)values[i];
   }
}

template < typename T, typename T1 >
void vectorAssign(std::vector < std::vector < T >>& pvec, const int& rowId,
                  const std::vector < int >& colIds, const T1& value)
{
   unsigned nrows = pvec.size();

   if (rowId >= nrows)
   {
      std::cerr << "vectormath::vectorAssign:Invalid input row index\n";
      exit(1);
   }
   if (rowId < 0)
   {
      std::cerr << "vectormath::vectorAssign: Row Input index is negative\n";
      exit(1);
   }
   unsigned ncols = pvec[rowId].size();

   for (unsigned i = 0; i < colIds.size(); ++i)
   {
      if (colIds[i] >= ncols)
      {
         std::cerr << "vectormath::vectorAssign:Invalid input index\n";
         exit(1);
      }
      if (colIds[i] < 0)
      {
         std::cerr << "vectormath::vectorAssign:Column Input index is negative\n";
         exit(1);
      }
      pvec[rowId][colIds[i]] = (T)value;
   }
}

template < typename T > std::vector < T > vectorExtract(const std::vector < T > &a,
                                                        const std::vector < int > &ids)
{
   std::vector < T > ret;
   int numel = (int)a.size();

   for (unsigned i = 0; i < ids.size(); ++i)
   {
      if (ids[i] >= numel)
      {
         std::cerr << "vectormath::vectorExtract:Invalid input index\n";
         exit(1);
      }
      if (ids[i] < 0)
      {
         std::cerr << "vectormath::vectorExtract:negative input index error\n";
         exit(1);
      }
      ret.push_back(a[ids[i]]);
   }
   return(ret);
}

template < typename T > std::vector < T > vectorExtract(const std::vector < T > &a,
                                                        const std::vector < bool > &logicalIds)
{
   std::vector < T > ret;
   unsigned numel = a.size();

   for (unsigned i = 0; i < logicalIds.size(); ++i)
   {
      if (i >= numel)
      {
         break;
      }
      if (logicalIds[i])
      {
         ret.push_back(a[i]);
      }
   }
   return(ret);
}

template < typename T > std::vector < T > vectorExtract(const std::vector < std::vector < T >> &a,
                                                        const std::vector < int > &rowIds, const int colId)
{
   std::vector < T > ret;
   unsigned nrows = a.size();

   if (nrows == 0)
   {
      return(ret);
   }
   unsigned ncols = a[0].size();

   if (colId >= ncols)
   {
      std::cerr << "vectormath::vectorExtract:Invalid input index\n";
      exit(1);
   }
   if (colId < 0)
   {
      std::cerr << "vectormath::vectorExtract: Column Input index is negative\n";
      exit(1);
   }
   ret.reserve(rowIds.size());
   for (unsigned i = 0; i < rowIds.size(); ++i)
   {
      if (rowIds[i] >= nrows)
      {
         std::cerr << "vectormath::vectorExtract:Invalid input index\n";
         exit(1);
      }
      if (rowIds[i] < 0)
      {
         std::cerr << "vectormath::vectorExtract: Row Input index is negative\n";
         exit(1);
      }
      ret.push_back(a[rowIds[i]][colId]);
   }
   return(ret);
}

template < typename T > std::vector < std::vector < T >> vectorExtract(const std::vector < std::vector < T >> &a,
                                                                       const std::vector < int > &rowIds, const std::vector < int > &colIds)
{
   std::vector < std::vector < T >> ret;
   int nrows = (int)a.size();

   if (nrows == 0)
   {
      return(ret);
   }

   for (unsigned i = 0; i < rowIds.size(); ++i)
   {
      if (rowIds[i] >= nrows)
      {
         std::cerr << "vectormath::vectorExtract:Invalid input index\n";
         exit(1);
      }
      if (rowIds[i] < 0)
      {
         std::cerr << "vectormath::vectorExtract: Row Input index is negative\n";
         exit(1);
      }
      std::vector < T > tmp = vectorExtract(a[rowIds[i]], colIds);
      ret.push_back(tmp);
   }
   return(ret);
}

template < typename T > std::vector < T > vectorExtract(const std::vector < std::vector < T >> &a,
                                                        const std::vector < int > &ids)
{
   std::vector < T > ret;
   int nrows = (int)a.size();

   if (nrows == 0)
   {
      return(ret);
   }
   int ncols = (int)a[0].size();
   int numel = nrows * ncols;

   ret.reserve(ids.size());
   for (unsigned i = 0; i < ids.size(); ++i)
   {
      if (ids[i] >= numel)
      {
         std::cerr << "vectormath::vectorExtract:Invalid input index\n";
         exit(1);
      }
      if (ids[i] < 0)
      {
         std::cerr << "vectormath::vectorExtract: Input index is negative\n";
         exit(1);
      }
      int rowid, colid;
      colid = ids[i] / nrows;
      rowid = ids[i] % nrows;
      ret.push_back(a[rowid][colid]);
   }
   return(ret);
}

template < typename T > std::vector < T > vectorExtract(const std::vector < std::vector < T >> &a,
                                                        const std::vector < bool > &logicalIds)
{
   std::vector < T > ret;
   unsigned nrows = a.size();

   if (nrows == 0)
   {
      return(ret);
   }
   unsigned ncols = a[0].size();
   unsigned numel = nrows * ncols;
   unsigned rowid, colid;

   ret.reserve(logicalIds.size());
   for (unsigned i = 0; i < logicalIds.size(); ++i)
   {
      if (i >= numel)
      {
         break;;
      }
      if (logicalIds[i])
      {
         colid = i / nrows;
         rowid = i % nrows;
         ret.push_back(a[rowid][colid]);
      }
   }
   return(ret);
}

template < typename T > T vectorExtract(const std::vector < std::vector < T >>&a,
                                        int ids)
{
   T ret = 0;

   if (ids < 0)
   {
      std::cerr << "vectormath::vectorExtract: Input index is negative\n";
      exit(1);
   }
   int nrows = (int)a.size();

   if (nrows == 0)
   {
      return(ret);
   }
   int ncols = (int)a[0].size();
   int numel = (int)nrows * ncols;
   int rowid, colid;

   if (ids >= numel)
   {
      std::cerr << "vectormath::vectorExtract:Invalid input index\n";
      exit(1);
   }
   colid = ids / nrows;
   rowid = ids % nrows;
   ret   = a[rowid][colid];

   return(ret);
}

#endif

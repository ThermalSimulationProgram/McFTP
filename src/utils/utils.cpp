#include "utils/utils.h"

#include <algorithm>
#include <stdio.h>

#include "utils/r8lib.hpp"
#include "utils/matrix_exponential.hpp"

using namespace std;
string formatString(const string& base)
{
   string ret = base;

   std::replace(ret.begin(), ret.end(), '{', ' ');
   std::replace(ret.begin(), ret.end(), '}', ' ');
   std::replace(ret.begin(), ret.end(), ',', ' ');
   std::replace(ret.begin(), ret.end(), '(', ' ');
   std::replace(ret.begin(), ret.end(), ')', ' ');
   std::replace(ret.begin(), ret.end(), '[', ' ');
   std::replace(ret.begin(), ret.end(), ']', ' ');

   return(ret);
}

// istringstream stringToiss(const string& base){
//    std::string fstring = formatString(base);

//    std::istringstream iss(fstring);
//    return iss;
// }

void dumpLinesSeperator()
{
   std::cout << "**********************************************************************" << std::endl;
}

std::string getexepath()
{
   char    result[PATH_MAX];
   ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);

   return(std::string(result, (count > 0) ? count : 0));
}

void matrixAAndBToWAndV(
   int N,
   double a[],
   double b[],
   double period,
   double **W,
   double **V,
   int nLoops)
{
   printf("start computing [%d * %d] matrix W and V \n", N, N);

   double *At = r8mat_copy_new(N, N, a);

   r8mat_scale(N, N, period, At);

   *W = r8mat_expm1(N, At);


   *V = r8mat_zeros_new(N, N);
   double *eAtB = r8mat_zeros_new(N, N);

   int    integralStepNumber = nLoops; // 100 sub intervals to calculate integral
   double integralStep       = period / integralStepNumber;

   for (int i = 0; i <= integralStepNumber; ++i)
   {
      printf("loop %d ... \n", i);
      r8mat_copy(N, N, a, At);
      double deltaT = i * integralStep;
      r8mat_scale(N, N, deltaT, At);

      double *eAt = r8mat_expm1(N, At);

      r8mat_mm(N, N, N, eAt, b, eAtB);

      r8mat_add(N, N, 1.0, *V, integralStep, eAtB, *V);


      delete [] eAt;
   }
   delete [] At;
}

void checkVectorMatrixOrder(vector <vector <double> > A, int order, string title)
{
   int N = A.size();

   if (N <= 0)
   {
      printf("Error, given matrix is empty!\n");
      exit(1);
   }
   else
   {
      if (N != order)
      {
         printf("Error, given matrix row number should be %d!\n", order);
         exit(1);
      }
      for (int i = 0; i < N; ++i)
      {
         if ((int)A[i].size() != N)
         {
            printf("Error, given matrix must be a square matrix!\n");
            exit(1);
         }
      }
   }
}

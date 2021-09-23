#include "taskloads/Benchmark.h"

#include <cmath>
#include <stdlib.h>
#include <iostream>
#include <cstring>

#include "utils/TimeUtil.h"



using namespace std;



// { "ackermann",		stress_cpu_ackermann },
//    { "bitops",		stress_cpu_bitops },
//    { "correlate",		stress_cpu_correlate },
//    { "crc16",		stress_cpu_crc16 },
//    { "djb2a",		stress_cpu_djb2a },
//    { "euler",		stress_cpu_euler },
//    { "explog",		stress_cpu_explog },
//    { "fft",		stress_cpu_fft },
//    { "fibonacci",		stress_cpu_fibonacci },
//    { "float",     stress_cpu_fp },
//    { "fnv1a",		stress_cpu_fnv1a },
//    { "gcd",		stress_cpu_gcd },
//    { "hamming",		stress_cpu_hamming },
//    { "hyperbolic",		stress_cpu_hyperbolic },
//    { "idct",		stress_cpu_idct },
//    { "int",       stress_cpu_int },
//    { "jenkin",		stress_cpu_jenkin },
//    { "jmp",		stress_cpu_jmp },
//    { "ln2",		stress_cpu_ln2 },
//    { "loop",		stress_cpu_loop },
//    { "matrixprod",		stress_cpu_matrix_prod },
//    { "nsqrt",		stress_cpu_nsqrt },
//    { "omega",		stress_cpu_omega },
//    { "phi",		stress_cpu_phi },
//    { "pi",			stress_cpu_pi },
//    { "pjw",		stress_cpu_pjw },
//    { "prime",		stress_cpu_prime },
//    { "psi",		stress_cpu_psi },
//    { "queens",		stress_cpu_queens },
//    { "rand48",		stress_cpu_rand48 },
//    { "sdbm",		stress_cpu_sdbm },
//    { "sqrt",      stress_cpu_sqrt },
//    { "trig",		stress_cpu_trig },
//    { "union",		stress_cpu_union },



// Constructor uses the default benchmark, sqrt(rand())
Benchmark::Benchmark() : Benchmark("default")
{
   // -1 means the default benchmark
   benchmarkId = -1;
}

// Constructor needs the name of the benchmark
Benchmark::Benchmark(const std::string& benchmarkName) : TaskLoad(),
   RND(), cpuStressor()
{
   // get the number of total benchmarks
   int totalNumber = cpuStressor.getBenchmarkNumber();

   // find the benchmark id
   bool found = false;

   if (benchmarkName != "")
   {
      for (int i = 1; i <= totalNumber; ++i)
      {
         if (benchmarkName == cpuStressor.getBenchmarkName(i))
         {
            benchmarkId = i;
            found       = true;
            break;
         }
      }
   }

   // set as the default benchmark if not found
   if (!found)
   {
      benchmarkId = -1;
   }
}

bool Benchmark::runLoads(unsigned long wcet_us)
{
   // get start time
   unsigned long timeIn;
   // record the real executed time
   unsigned long realLength = 0;

   unsigned long timeEnd;

   if (benchmarkId < 1)
   {
      do
      {
         setSuspendPoint();                // set a suspend point here. When recieves a suspend signal, pause execution here
         setCheckBlockBegin();

         timeIn = TimeUtil::convert_us(TimeUtil::getTime());
         for (int i = 0; i < 3000; ++i)
         {
            sqrt(RND.mwc32());
         }

         timeEnd     = TimeUtil::convert_us(TimeUtil::getTime());
         realLength += timeEnd - timeIn;

         setCheckBlockEnd();
      }while (realLength < wcet_us);
   }
   else
   {
      do
      {
         setSuspendPoint();                // set a suspend point here. When recieves a suspend signal, pause execution here
         setCheckBlockBegin();

         timeIn = TimeUtil::convert_us(TimeUtil::getTime());
         cpuStressor.stressWithBenchmark(benchmarkId);
         timeEnd     = TimeUtil::convert_us(TimeUtil::getTime());
         realLength += timeEnd - timeIn;

         setCheckBlockEnd();
      }while (realLength < wcet_us);
   }

   return(true);
}

// unsigned long Benchmark::consume_us_benchmarks(unsigned long length){

//    if (benchmarkId < 1){
//       // default
//       return consume_us_rand(length);
//    }else{
//       unsigned long timeIn = TimeUtil::convert_us(TimeUtil::getTime());
//       unsigned long realLength;
//       do{
//          cpuStressor.stressWithBenchmark(benchmarkId);
//          realLength = TimeUtil::convert_us(TimeUtil::getTime()) - timeIn;

//       }while(realLength < length);

//       return realLength;
//    }
// }


// unsigned long Benchmark::consume_us_rand(unsigned long length){
//    unsigned long timeIn = TimeUtil::convert_us(TimeUtil::getTime());
//    unsigned long realLength;

//    do{
//       for (int i = 0; i < 3000; ++i){
//          sqrt(RND.mwc32());
//       }
//       realLength = TimeUtil::convert_us(TimeUtil::getTime()) - timeIn;

//    }while(realLength < length);

//    return realLength;
// }

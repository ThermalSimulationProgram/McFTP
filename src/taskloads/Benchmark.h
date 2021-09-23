#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <unistd.h>
#include <stdlib.h>
#include <string>

#include "benchmarks/benchmarks.h"
#include "core/TaskLoad.h"

// A wrapper function to run benchmarks with given time length
class Benchmark : public TaskLoad
{
private:
   // randome number generator
   MWC RND;

   // cpu_stressor calls the benchmarks
   CPUStressor cpuStressor;

   // the index of the benchmark will be called
   int benchmarkId;

public:

   // Constructor uses the default benchmark, sqrt(rand())
   Benchmark();

   // Constructor needs the name of the benchmark
   Benchmark(const std::string& benchmarkName);

   // this function will be called by Workers
   bool runLoads(unsigned long wcet_us);

   // // auxiliary function. Call the benchmark for given time, units microsecond
   // unsigned long consume_us_benchmarks(unsigned long timelength);

   // // auxiliary function. Call the default benchmark for given time, units microsecond
   // unsigned long consume_us_rand(unsigned long timelength);
};


#endif

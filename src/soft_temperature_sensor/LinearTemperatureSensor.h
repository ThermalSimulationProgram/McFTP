#ifndef MCFTP_LINEAR_TEMPERATURE_SENSOR_H
#define MCFTP_LINEAR_TEMPERATURE_SENSOR_H

#include <vector>

void initializeLinearTemperatureSensor();

std::vector < double > LinearTemperatureSensor(
   std::vector < long long > &performanceCounterValues);

std::vector < double > TemperaturePredictCore(
   std::vector < long long > &performanceCounterValues,
   int Ncores,
   int thermalModelOrder,
   std::vector < double > &_coefRate,
   std::vector < double > &_weights,
   std::vector < double > &_coefA,
   std::vector < double > &_coefB,
   double *_W,
   double *_V,
   double *_KTamb,
   double *_Ti,
   double *_temp1,
   double *_temp2,
   double *_newT);

#endif

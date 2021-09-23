#ifndef TEMPERATURE_SAVE_OPTION_H
#define TEMPERATURE_SAVE_OPTION_H

#include <string>

class TemperatureSaveOption
{
public:
   bool isSaveGlobalPeakTemperature;
   bool isSaveSensorPeakTemperature;
   bool isSaveSensorMeanTemperature;
   bool isSaveSensorReadOverhead;
   bool isSaveSensorTemperatureTrace;

   TemperatureSaveOption();

   ~TemperatureSaveOption();

   void dump();

   void print(std::string name);
};



#endif

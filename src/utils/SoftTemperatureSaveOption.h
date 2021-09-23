#ifndef SOFT_TEMPERATURE_SAVE_OPTION
#define SOFT_TEMPERATURE_SAVE_OPTION

#include "utils/TemperatureSaveOption.h"

class SoftTemperatureSaveOption : public TemperatureSaveOption
{
public:
   bool isSavePerformanceCounterValues;

   SoftTemperatureSaveOption();
   ~SoftTemperatureSaveOption();

   void print(std::string name);
};
#endif

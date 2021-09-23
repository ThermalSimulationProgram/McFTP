#include "soft_temperature_sensor/LinearTemperatureSensor.h"


#include <iostream>

#include "configuration/Scratch.h"
#include "utils/utils.h"
#include "utils/c8lib.hpp"
#include "utils/r8lib.hpp"
#include "utils/Semaphores.h"

using namespace std;

vector <double> previousT;
int             thermalModelOrder     = 0;
bool            parametersInitialized = false;


vector <double> coefRate;
vector <double> weights;
vector <double> coefA;
vector <double> coefB;
double *        W;
double *        V;
double *        KTamb;
double *        Ti;

double *temp1;
double *temp2;
double *newT;

void initializeLinearTemperatureSensor()
{
   if (!parametersInitialized)
   {
      if (!Scratch::isInitialized())
      {
         Semaphores::print_sem.wait_sem();
         cout << "LinearTemperatureSensor warning: necessary parameters are not initialized!" << endl;
         Semaphores::print_sem.post_sem();
      }
      else
      {
         coefA = Scratch::coefA;
         coefB = Scratch::coefB;

         for (int i = 0; i < (int)Scratch::soft_sensors.size(); ++i)
         {
            coefRate.push_back(Scratch::soft_sensors[i].valueScale);
            weights.push_back(Scratch::soft_sensors[i].weight);
         }

         thermalModelOrder = Scratch::softSensorW.size();

         previousT = Scratch::ambientT;


         W     = vectorMatrixTo2DArray(Scratch::softSensorW);
         V     = vectorMatrixTo2DArray(Scratch::softSensorV);
         KTamb = vectorMatrixTo2DArray(Scratch::softSensorK);
         Ti    = vectorToArray(previousT);

         temp1 = r8mat_zeros_new(thermalModelOrder, thermalModelOrder);
         temp2 = r8mat_zeros_new(thermalModelOrder, thermalModelOrder);
         newT  = r8mat_zeros_new(thermalModelOrder, 1);

         parametersInitialized = true;
      }
   }
}

vector <double> LinearTemperatureSensor(vector <long long>& performanceCounterValues)
{
   vector <double> ret;

   if (!parametersInitialized)
   {
      Semaphores::print_sem.wait_sem();
      cout << "LinearTemperatureSensor error: parameters are not initialized!" << endl;
      Semaphores::print_sem.post_sem();
      return(ret);
   }
   int allPCValuesSize = performanceCounterValues.size();

   if (allPCValuesSize % Scratch::getNcores() != 0)
   {
      Semaphores::print_sem.wait_sem();
      cout << "LinearTemperatureSensor error: performance counter values' size error!" << endl;
      Semaphores::print_sem.post_sem();
      return(ret);
   }

   return(TemperaturePredictCore(performanceCounterValues, Scratch::getNcores(),
                                 thermalModelOrder, coefRate, weights, coefA, coefB, W, V, KTamb, Ti,
                                 temp1, temp2, newT));
}

vector <double> TemperaturePredictCore(
   vector <long long>& performanceCounterValues,
   int Ncores,
   int thermalModelOrder,
   vector <double>& _coefRate,
   vector <double>& _weights,
   vector <double>& _coefA,
   vector <double>& _coefB,
   double *_W,
   double *_V,
   double *_KTamb,
   double *_Ti,
   double *_temp1,
   double *_temp2,
   double *_newT)
{
   vector <double> ret;

   if ((int)_coefA.size() != Ncores || (int)_coefB.size() != Ncores)
   {
      cout << "LinearTemperatureSensor error: coefA or coefB size error!" << endl;
      return(ret);
   }

   int allPCValuesSize = performanceCounterValues.size();

   int allCountersNum = allPCValuesSize / Ncores;

   if ((int)_coefRate.size() != allCountersNum || (int)_weights.size() != allCountersNum)
   {
      cout << "LinearTemperatureSensor error: coefRate or weight size error!" << endl;
      return(ret);
   }

   vector <double> rates;

   for (int i = 0; i < Ncores; ++i)
   {
      double temp    = 0.0;
      int    startId = i * allCountersNum;
      for (int j = 0; j < allCountersNum; ++j)
      {
         temp += performanceCounterValues[startId + j] / _coefRate[j] * _weights[j];
      }

      rates.push_back(temp * _coefA[i] + _coefB[i] + _KTamb[i]);
   }
   for (int i = Ncores; i < thermalModelOrder; ++i)
   {
      rates.push_back(_KTamb[i]);
   }
   double *_u = vectorToArray(rates);



   r8mat_mm(thermalModelOrder, thermalModelOrder, 1, _W, _Ti, _temp1);


   r8mat_mm(thermalModelOrder, thermalModelOrder, 1, _V, _u, _temp2);


   r8mat_add(thermalModelOrder, 1, 1, _temp1, 1, _temp2, _newT);


   r8mat_copy(thermalModelOrder, 1, _newT, _Ti);



   ret.reserve(Ncores);
   for (int i = 0; i < Ncores; ++i)
   {
      ret.push_back(_newT[i]);
   }


   delete[] _u;
   return(ret);
}

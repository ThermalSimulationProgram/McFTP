#ifndef _PERIODICJITTER_H
#define _PERIODICJITTER_H

#include "core/Dispatcher.h"

/***************************************
 *        CLASS DECLARATION            * 
 ***************************************/

class PeriodicJitter : public Dispatcher {

 private:

  /*********** VARIABLES ***********/

  ///Auxiliary variable to keep track of delta Period
  struct timespec deltaPeriod;

  ///The dispatcher's jitter
  struct timespec jitter;

  ///The dispatcher's period
  struct timespec period;

 public:

  /*********** CONSTRUCTOR ***********/

  ///Constructor needs simulation pointer, and id
  PeriodicJitter(unsigned int id);

  /*********** INHERITED FUNCTIONS ***********/

  /**** FROM DISPATCHER ****/

  ///This function was a flagged loop that activates the Worker according to the task periodicity
  void dispatch();

  /*********** GETTER AND SETTER FUNCTIONS ***********/
  
  ///This function returns the period
  struct timespec getPeriod();

  ///This function sets the jitter
  void setJitter(struct timespec j);

  ///This function sets the period
  void setPeriod(struct timespec p);
};
#endif

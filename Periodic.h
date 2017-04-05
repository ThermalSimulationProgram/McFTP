#ifndef _PERIODIC_H
#define _PERIODIC_H

#include "core/Dispatcher.h"

/***************************************
 *        CLASS DECLARATION            * 
 ***************************************/

class Periodic : public Dispatcher {

 private:

  /*********** VARIABLES ***********/

  ///This timespec hols the Dispatcher's period
  struct timespec period;

 public:

  /*********** CONSTRUCTOR ***********/

  ///Constructor needs simulation pointer, and id
  Periodic(unsigned int id);

  /*********** INHERITED FUNCTIONS ***********/
  
  /**** FROM DISPATCHER ****/  
  
  ///This function was a flagged loop that activates the Worker according to the task periodicity
  void dispatch();

  /*********** GETTER AND SETTER FUNCTIONS ***********/

  ///This function returns the period
  struct timespec getPeriod();

  ///This function sets the period
  void setPeriod(struct timespec p);
};
#endif

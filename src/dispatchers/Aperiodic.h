#ifndef _APERIODIC_H
#define _APERIODIC_H

#include "core/Dispatcher.h"

/***************************************
 *        CLASS DECLARATION            * 
 ***************************************/

class Aperiodic : public Dispatcher {

 private:

  /*********** VARIABLES ***********/

  ///This parameter specifies the release time of the aperiodic dispatcher
  struct timespec releaseTime;

 public:

  /*********** CONSTRUCTOR ***********/

  ///Constructor needs simulation pointer, and id
  Aperiodic(unsigned int id);

  /*********** INHERITED FUNCTIONS ***********/
  
  /**** FROM DISPATCHER ****/
  
  ///This function was a flagged loop that activates the Worker according to the task periodicity
  void dispatch();

  /*********** GETTER AND SETTER FUNCTIONS ***********/

  ///This function returns the release time
  struct timespec getReleaseTime();  

  ///This function sets the release time for the aperiodic dispatcher
  void setReleaseTime(struct timespec r);
};
#endif

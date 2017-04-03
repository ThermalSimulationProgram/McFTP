#include "Priorities.h"

///Maximum number of threads under a scheduler
#define MAX_LEVELS 10

/***************************************
 *        CLASS DEFINITION             * 
 ***************************************/

/*********** MEMBER FUNCTIONS ***********/

int Priorities::get_main_pr() {
  return 7+MAX_LEVELS;
}


int Priorities::get_sched_pr(int level) {
  if(level<0 || level>MAX_LEVELS) {
    return 0;
  }
  else {
     return 6+MAX_LEVELS-level; 
  }
}

int Priorities::get_disp_pr() {
  return 5;
}

int Priorities::get_server_pr() {
  return 4;
}

int Priorities::get_active_pr() {
  return 3;
}

int Priorities::get_idle_pr() {
  return 2;
}

int Priorities::get_inactive_pr() {
  return 1;
}

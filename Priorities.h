#ifndef _PRIORITIES_H
#define _PRIORITIES_H

/***************************************
 *        CLASS DECLARATION            * 
 ***************************************/

class Priorities {

  public:

  /*********** MEMBER FUNCTIONS ***********/
  
  ///This function returns the MAIN_PR constant, to be assigned to the main thread
  static int get_main_pr();
  
  ///This function returns the DISP_PR constant, to be assigned all dispatcher threads
  static int get_disp_pr();

  ///This function returns the SCHED_PR constant, which depends on the scheduler's hierarchical level
  static int get_sched_pr(int level);
    
  ///This function returns the SERVER_PR constant
  static int get_server_pr();
  
  ///This function returns the ACTIVE_PR constant, to be assigned to all active workers
  static int get_active_pr();
  
  ///This function returns the IDLE_PR constant, to be assigned to the IDLE thread
  static int get_idle_pr();
  
  ///This function returns the INACTIVE_PR constant, to be assigned to all non-active threads
  static int get_inactive_pr();    
};

#endif

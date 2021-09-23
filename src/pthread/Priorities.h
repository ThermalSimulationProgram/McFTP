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

   ///This function returns the THERMALAPP_PR constant, to be assigned to the thermal approach thread
   static int get_thermalapp_pr();

   ///This function returns the DISPATCHER_PR constant, to be assigned all dispatcher threads
   static int get_dispatcher_pr();

   ///This function returns the POWERMANAGER_PR constant, to be assigned to the power manager thread
   static int get_powermanager_pr();

   ///This function returns the TEMPWATCHER_PR constant, to be assigned to the temperature watcher thread
   static int get_tempwatcher_pr();

   ///This function returns the ACTIVE_PR constant, to be assigned to all active workers
   static int get_active_pr();

   ///This function returns the IDLE_PR constant, to be assigned to the IDLE thread
   static int get_idle_pr();

   ///This function returns the INACTIVE_PR constant, to be assigned to all non-active threads
   static int get_inactive_pr();
};

#endif

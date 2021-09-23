#include "pthread/Priorities.h"


/***************************************
*        CLASS DEFINITION             *
***************************************/

/*********** MEMBER FUNCTIONS ***********/

int Priorities::get_main_pr()
{
   return(8);
}

int Priorities::get_thermalapp_pr()
{
   return(7);
}

int Priorities::get_dispatcher_pr()
{
   return(6);
}

int Priorities::get_powermanager_pr()
{
   return(5);
}

int Priorities::get_tempwatcher_pr()
{
   return(4);
}

int Priorities::get_active_pr()
{
   return(3);
}

int Priorities::get_idle_pr()
{
   return(2);
}

int Priorities::get_inactive_pr()
{
   return(1);
}

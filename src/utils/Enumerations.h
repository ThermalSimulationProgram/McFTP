#ifndef _ENUMERATIONS_H
#define _ENUMERATIONS_H



///This enumeration descibes the possible task models
enum _task_periodicity
{
   aperiodic,
   periodic,
   periodic_jitter
};

//This enumeration decribes the type of the structure of a task
enum _task_type
{
   singlecore, // the task is executed by one core
   pipelined,  // the task is divided into several sub-tasks. They are executed serially by several cores
};

//This enumeration decribes the type of the load of a task
enum _task_load_type
{
   busywait,
   benchmark,
   userdefined
};



enum _worker_state
{
   _sleep,
   _active
};
///This enum describes all traced actions
enum _task_action
{
   task_arrival, //0
   sched_start,
   sched_end,
   task_start,                //3
   schedule_adaption,         //4
   schedule_interrput_sleep,
   schedule_interrput_active, //6
   task_end,
   deadline_met,
   deadline_missed,
   sim_end,             //10

   sleep_start,
   sleep_end,
   active_start,
   active_end
};


///This enum describes what thread types can produce traces
enum _thread_type
{
   _main,
   _power_manager,
   _thermal_approach,
   _dispatcher,
   _temp_watcher,
   _worker
};


///This enumeration describes the two time types
enum _time_type
{
   _absolute,
   _relative
};



#endif

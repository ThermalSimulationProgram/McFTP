#include "utils/Semaphores.h"
// #include <iostream>

scoped_sem_t Semaphores::print_sem(1);
scoped_sem_t Semaphores::dispatch_sem(0);
scoped_sem_t Semaphores::rtcinit_sem(0);


scoped_sem_t::scoped_sem_t(int v)
{
   sem_init(&sem, 0, v);
}

scoped_sem_t::~scoped_sem_t()
{
   sem_destroy(&sem);
}

int scoped_sem_t::wait_sem()
{
   return(sem_wait(&sem));
}

int scoped_sem_t::post_sem()
{
   return(sem_post(&sem));
}

int scoped_sem_t::timedwait_sem(const struct timespec *abs_time)
{
   return(sem_timedwait(&sem, abs_time));
}

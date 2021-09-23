#ifndef _SEMAPHORES_H
#define _SEMAPHORES_H

#include <semaphore.h>



class scoped_sem_t {
private:
   sem_t sem;

public:
   explicit scoped_sem_t(int);
   ~scoped_sem_t();

   int wait_sem();
   int post_sem();
   int timedwait_sem(const struct timespec *);
};


class Semaphores {
public:
   static scoped_sem_t print_sem;
   static scoped_sem_t dispatch_sem;
   static scoped_sem_t rtcinit_sem;
};



#endif

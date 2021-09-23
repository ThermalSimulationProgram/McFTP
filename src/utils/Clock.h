#ifndef HSFCLOCK_H
#define HSFCLOCK_H

#if MONOTONIC == 1
  #define HSF_CLOCK    CLOCK_MONOTONIC
#else
  #define HSF_CLOCK    CLOCK_REALTIME
#endif

#endif

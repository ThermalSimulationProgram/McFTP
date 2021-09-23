#include "benchmarks/MWC.h"

#include <sys/time.h>
#include <unistd.h>

MWC::MWC()
{
   w = MWC_SEED_W;
   z = MWC_SEED_Z;
}

MWC::~MWC()
{
}

/*
 *  mwc32()
 *      Multiply-with-carry random numbers
 *      fast pseudo random number generator, see
 *      http://www.cse.yorku.ca/~oz/marsaglia-rng.html
 */
uint32_t MWC::mwc32()
{
   uint32_t ret;

   z   = 36969 * (z & 65535) + (z >> 16);
   w   = 18000 * (w & 65535) + (w >> 16);
   ret = (z << 16) + w;

   return(ret);
}

uint64_t MWC::mwc64()
{
   return((((uint64_t)mwc32()) << 32) | mwc32());
}

uint16_t MWC::mwc16()
{
   return(mwc32() & 0xffff);
}

uint8_t MWC::mwc8()
{
   return(mwc32() & 0xff);
}

void MWC::mwc_seed(void)
{
   w = MWC_SEED_W;
   z = MWC_SEED_Z;
}

void MWC::mwc_reseed(void)
{
   if (NO_RAND_SEED)
   {
      w = MWC_SEED_W;
      z = MWC_SEED_Z;
   }
   else
   {
      struct timeval tv;
      int            i, n;

      z = 0;
      if (gettimeofday(&tv, NULL) == 0)
      {
         z = (uint64_t)tv.tv_sec ^ (uint64_t)tv.tv_usec;
      }
      z += ~((unsigned char *)&z - (unsigned char *)&tv);
      w  = (uint64_t)getpid() ^ (uint64_t)getppid() << 12;

      n = (int)z % 1733;
      for (i = 0; i < n; i++)
      {
         (void)mwc32();
      }
   }
}

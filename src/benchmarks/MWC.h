#ifndef MWC_H
#define MWC_H

#include <cstdint>
#include <semaphore.h>

#define MWC_SEED_Z      (362436069UL)
#define MWC_SEED_W      (521288629UL)
#define NO_RAND_SEED    (0)


// Only for cpu stress test.
// fast PSEUDO random number generator
// Note it is NOT thread safe
class MWC {
   /* Fast random number generator seed */
private:
   uint32_t w;
   uint32_t z;
public:
   MWC();
   ~MWC();

   // get a 32bit number
   uint32_t mwc32(void);
   uint64_t mwc64(void);
   uint16_t mwc16(void);
   uint8_t  mwc8(void);

   // reset the seed
   void mwc_reseed(void);
   void mwc_seed(void);
};



#endif

#ifndef MWC_H
#define MWC_H

#include <cstdint>
#include <semaphore.h>

#define MWC_SEED_Z      (362436069UL)
#define MWC_SEED_W      (521288629UL)
#define NO_RAND_SEED    (0)

class MWC {
   /* Fast random number generator state */
private:
   uint32_t w;
   uint32_t z;
   // sem_t mwc_sem;
public:
   MWC();
   ~MWC();

   uint32_t mwc32(void);
   uint64_t mwc64(void);
   uint16_t mwc16(void);
   uint8_t  mwc8(void);

   void mwc_reseed(void);
   void mwc_seed(void);
};



#endif

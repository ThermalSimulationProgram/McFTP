#include "benchmarks/benchmarks.h"


#include <math.h>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <cstring>

#if defined(__GNUC__) && defined(__linux__)
#include <malloc.h>
#endif


uint64_t opt_flags = PR_ERROR | PR_INFO | OPT_FLAGS_MMAP_MADVISE;



// #define GAMMA  (0.57721566490153286060651209008240243104215933593992L)
// #define OMEGA	(0.56714329040978387299996866221035554975381578718651L)
// #define PSI	(3.359885666243177553172011302918927179688905133732L)

/* Some awful *BSD math lib workarounds */
#if defined(__NetBSD__)
#define rintl    rint
#define logl     log
#define expl     exp
#define powl     pow
#define cosl     cos
#define sinl     sin
#define coshl    cosh
#define sinhl    sinh
#define ccosl    ccos
#define csinl    csin
#define cabsl    cabs
#define sqrtl    sqrt
#endif

#if defined(__FreeBSD__)
#define ccosl    ccos
#define csinl    csin
#define cpow     pow
#define powl     pow
#endif

#if defined(__minix__)
#define cabsl    cabs
#endif



void uint64_put(const uint64_t a)
{
}

void double_put(const double a)
{
}

static const uint32_t queens_solutions[] = {
   0, 1, 0, 0, 2, 10, 4, 40, 92, 352, 724, 2680, 14200
};

using namespace std;

void stress_cpu_queens(const char *name)
{
   uint32_t all = 1;

   for (int i = 0; i < 7; ++i)
   {
      queens_try(0, 0, 0, all);
      all = (all + all) + 1;
   }
}

/*
 *  stress_cpu_queens
 *	solve the queens problem for sizes 1..12
 */
uint32_t queens_try(
   uint32_t left_diag,
   uint32_t cols,
   uint32_t right_diag,
   uint32_t all)
{
   register uint32_t solutions = 0;
   register uint32_t poss      = ~(left_diag | cols | right_diag) & all;

   while (poss)
   {
      register uint32_t bit      = poss & -poss;
      register uint32_t new_cols = cols | bit;

      poss      -= bit;
      solutions += (new_cols == all) ?
                   1 : queens_try((left_diag | bit) << 1,
                                  new_cols, (right_diag | bit) >> 1, all);
   }
   return(solutions);
}

void HOT stress_cpu_sqrt(const char *name)
{
   int i;

   for (i = 0; i < 256; i++)
   {
      uint64_t rnd = i * i + 123;
      double   r   = sqrt((double)rnd) * sqrt((double)rnd);
      double_put(r);
   }
}

void stress_cpu_loop(const char *name)
{
   uint32_t i, i_sum = 0;

   for (i = 0; i < 1500; i++)
   {
      i_sum += i;
      FORCE_DO_NOTHING();
   }
}

void HOT OPTIMIZE3 stress_cpu_gcd(const char *name)
{
   uint32_t i_sum = 0;
   uint32_t i     = 0;

   for (i = 0; i < 512; i++)
   {
      register uint32_t a = i, b = i % (3 + (1997 ^ i));

      while (b != 0)
      {
         register uint32_t r = b;
         b = a % b;
         a = r;
      }
      i_sum += a;
      FORCE_DO_NOTHING();
      i++;
      if (i > 16384)
      {
         i = 0;
      }
   }
}

/*
 *  stress_cpu_bitops()
 *	various bit manipulation hacks from bithacks
 *	https://graphics.stanford.edu/~seander/bithacks.html
 */
void HOT OPTIMIZE3 stress_cpu_bitops(const char *name)
{
   uint32_t i_sum = 0;
   uint32_t i     = 0;

   for (i = 0; i < 128; i++)
   {
      {
         register uint32_t r, v, s = (sizeof(v) * 8) - 1;

         /* Reverse bits */
         r = v = i;
         for (v >>= 1; v; v >>= 1, s--)
         {
            r <<= 1;
            r  |= v & 1;
         }
         r    <<= s;
         i_sum += r;
      }
      {
         /* parity check */
         register uint32_t v = i;

         v     ^= v >> 16;
         v     ^= v >> 8;
         v     ^= v >> 4;
         v     &= 0xf;
         i_sum += v;
      }
      {
         /* Brian Kernighan count bits */
         register uint32_t j, v = i;

         for (j = 0; v; j++)
         {
            v &= v - 1;
         }
         i_sum += j;
      }
      {
         /* round up to nearest highest power of 2 */
         register uint32_t v = i - 1;

         v     |= v >> 1;
         v     |= v >> 2;
         v     |= v >> 4;
         v     |= v >> 8;
         v     |= v >> 16;
         i_sum += v;
      }
   }
}

/*
 *  stress_cpu_trig()
 *	simple sin, cos trig functions
 */
void HOT stress_cpu_trig(const char *name)
{
   int         i     = 0;
   long double d_sum = 0.0;

   (void)name;

   for (i = 0; i < 15; i++)
   {
      long double theta = (2.0 * M_PI * (double)i) / 1500.0;
      {
         d_sum += (cosl(theta) * sinl(theta));
         d_sum += (cos(theta) * sin(theta));
         d_sum += (cosf(theta) * sinf(theta));
      }
      {
         long double theta2 = theta * 2.0;

         d_sum += cosl(theta2);
         d_sum += cos(theta2);
         d_sum += cosf(theta2);
      }
      {
         long double theta3 = theta * 3.0;

         d_sum += sinl(theta3);
         d_sum += sin(theta3);
         d_sum += sinf(theta3);
      }
   }

   double_put(d_sum);
}

/*
 *  stress_cpu_hyperbolic()
 *	simple hyperbolic sinh, cosh functions
 */
void HOT stress_cpu_hyperbolic(const char *name)
{
   static int i     = 0;
   double     d_sum = 0.0;

   (void)name;

   // for (i = 0; i < 1500; i++) {
   long double theta = (2.0 * M_PI * (double)i) / 1500.0;

   {
      d_sum += (coshl(theta) * sinhl(theta));
      d_sum += (cosh(theta) * sinh(theta));
      d_sum += (double)(coshf(theta) * sinhf(theta));
   }
   {
      long double theta2 = theta * 2.0;

      d_sum += coshl(theta2);
      d_sum += cosh(theta2);
      d_sum += (double)coshf(theta2);
   }
   {
      long double theta3 = theta * 3.0;

      d_sum += sinhl(theta3);
      d_sum += sinh(theta3);
      d_sum += (double)sinhf(theta3);
   }
   // }
   i++;
   if (i > 16384)
   {
      i = 0;
   }
   double_put(d_sum);
}

/*
 *  stress_cpu_rand()
 *	generate lots of pseudo-random integers
 */
void HOT OPTIMIZE3 stress_cpu_rand(const char *name)
{
   int      i;
   uint32_t i_sum = 0;
   MWC      RND   = MWC();

   for (i = 0; i < 16; i++)
   {
      i_sum += RND.mwc32();
   }
}

/*
 *  stress_cpu_rand48()
 *	generate random values using rand48 family of functions
 */
void HOT OPTIMIZE3 stress_cpu_rand48(const char *name)
{
   int      i;
   double   d = 0;
   long int l = 0;

   (void)name;

   srand48(0x0defaced);
   for (i = 0; i < 256; i++)
   {
      d += drand48();
      l += lrand48();
   }
   double_put(d);
   uint64_put(l);
}

/*
 *  stress_cpu_nsqrt()
 *	iterative Newton–Raphson square root
 */
void HOT OPTIMIZE3 stress_cpu_nsqrt(const char *name)
{
   int i;
   const long double precision = 1.0e-12;
   const int         max_iter  = 56;

   for (i = 0; i < 10; i++)
   {
      long double n  = (double)i;
      long double lo = (n < 1.0) ? n : 1.0;
      long double hi = (n < 1.0) ? 1.0 : n;
      long double rt;
      int         j = 0;

      while ((j++ < max_iter) && ((hi - lo) > precision))
      {
         long double g = (lo + hi) / 2.0;
         if ((g * g) > n)
         {
            hi = g;
         }
         else
         {
            lo = g;
         }
      }
      rt = (lo + hi) / 2.0;
      double_put((double)rt);
   }
}

/*
 *  stress_cpu_phi()
 *	compute the Golden Ratio
 */
void HOT OPTIMIZE3 stress_cpu_phi(const char *name)
{
   long double       phi; /* Golden ratio */
   register uint64_t a, b;
   const uint64_t    mask = 1ULL << 63;
   int i;

   MWC RND = MWC();

   /* Pick any two starting points */
   a = RND.mwc64() % 99;
   b = RND.mwc64() % 99;

   /* Iterate until we approach overflow */
   for (i = 0; (i < 64) && !((a | b) & mask); i++)
   {
      /* Find nth term */
      register uint64_t c = a + b;

      a = b;
      b = c;
   }
   /* And we have the golden ratio */
   phi = (long double)b / (long double)a;
   double_put((double)phi);
}

/*
 *  fft_partial()
 *    partial Fast Fourier Transform
 */
void HOT OPTIMIZE3 fft_partial(
   complex <double> *data,
   complex <double> *tmp,
   const int n,
   const int m)
{
   if (m < n)
   {
      const int m2 = m * 2;
      int       i;

      fft_partial(tmp, data, n, m2);
      fft_partial(tmp + m, data + m, n, m2);
      for (i = 0; i < n; i += m2)
      {
         const complex <double> negI(0, -1.0);
         complex <double>       v = tmp[i];
         complex <double>       t =
            std::exp((negI * M_PI * (double)i) /
                     (double)n) * tmp[i + m];
         data[i / 2]       = v + t;
         data[(i + n) / 2] = v - t;
      }
   }
}

/*
 *  stress_cpu_fft()
 *	Fast Fourier Transform
 */
void HOT stress_cpu_fft(const char *name)
{
   complex <double> buf[FFT_SIZE], tmp[FFT_SIZE];
   int i;

   (void)name;

   for (i = 0; i < FFT_SIZE; i++)
   {
      buf[i] = complex <double>(i % 63, 0);
      // buf[i] = (double complex)(i % 63);
   }

   memcpy(tmp, buf, sizeof(complex <double>) * FFT_SIZE);
   fft_partial(buf, tmp, FFT_SIZE, 1);
}

/*
 *   stress_cpu_euler()
 *	compute e using series
 */
void HOT OPTIMIZE3 stress_cpu_euler(const char *name)
{
   for (int i = 0; i < 25; ++i)
   {
      long double e = 1.0, last_e;
      long double fact      = 1.0;
      long double precision = 1.0e-20;
      int         n         = 1;
      do
      {
         last_e = e;
         fact  *= n;
         n++;
         e += (1.0 / fact);
      } while ((n < i) && (fabsl(e - last_e) > precision));
   }
}

/*
 *  random_buffer()
 *	fill a uint8_t buffer full of random data
 *	buffer *must* be multiple of 4 bytes in size
 */
void random_buffer(uint8_t *data, const size_t len)
{
   size_t i;
   MWC    RND = MWC();

   for (i = 0; i < len / 4; i++)
   {
      uint32_t v = RND.mwc32();

      *data++ = v;
      v     >>= 8;
      *data++ = v;
      v     >>= 8;
      *data++ = v;
      v     >>= 8;
      *data++ = v;
   }
}

/*
 *  stress_cpu_hash_generic()
 *	stress test generic string hash function
 */
void stress_cpu_hash_generic(
   const char *name,
   const char *hash_name,
   uint32_t (*hash_func)(const char *str),
   const uint32_t result)
{
   char     buffer[64];
   size_t   i;
   uint32_t i_sum = 0;

   random_buffer((uint8_t *)buffer, sizeof(buffer));
   /* Make it ASCII range ' '..'_' */
   for (i = 0; i < sizeof(buffer); i++)
   {
      buffer[i] = (buffer[i] & 0x3f) + ' ';
   }

   for (i = sizeof(buffer) - 1; i; i--)
   {
      buffer[i] = '\0';
      i_sum    += hash_func(buffer);
   }
}

/*
 *  jenkin()
 *	Jenkin's hash on random data
 *	http://www.burtleburtle.net/bob/hash/doobs.html
 */
uint32_t HOT OPTIMIZE3 jenkin(const uint8_t *data, const size_t len)
{
   register uint8_t  i;
   register uint32_t h = 0;

   for (i = 0; i < len; i++)
   {
      h += *data++;
      h += h << 10;
      h ^= h >> 6;
   }
   h += h << 3;
   h ^= h >> 11;
   h += h << 15;

   return(h);
}

/*
 *  stress_cpu_jenkin()
 *	multiple iterations on jenkin hash
 */
void stress_cpu_jenkin(const char *name)
{
   uint8_t  buffer[32];
   size_t   i;
   uint32_t i_sum = 0;

   // const uint32_t sum = 0x96673680;

   random_buffer(buffer, sizeof(buffer));
   for (i = 0; i < sizeof(buffer); i++)
   {
      i_sum += jenkin(buffer, sizeof(buffer));
   }
}

/*
 *  pjw()
 *	Hash a string, from Aho, Sethi, Ullman, Compiling Techniques.
 */
uint32_t HOT OPTIMIZE3 pjw(const char *str)
{
   register uint32_t h = 0;

   while (*str)
   {
      register uint32_t g;

      h = (h << 4) + (*str);
      if (0 != (g = h & 0xf0000000))
      {
         h = h ^ (g >> 24);
         h = h ^ g;
      }
      str++;
   }
   return(h);
}

/*
 *  stress_cpu_pjw()
 *	stress test hash pjw
 */
void stress_cpu_pjw(const char *name)
{
   stress_cpu_hash_generic(name, "pjw", pjw, 0xa89a91c0);
}

/*
 *  djb2a()
 *	Hash a string, from Dan Bernstein comp.lang.c (xor version)
 */
uint32_t HOT OPTIMIZE3 djb2a(const char *str)
{
   register uint32_t hash = 5381;
   register int      c;

   while ((c = *str++))
   {
      /* (hash * 33) ^ c */
      hash = ((hash << 5) + hash) ^ c;
   }
   return(hash);
}

/*
 *  stress_cpu_djb2a()
 *	stress test hash djb2a
 */
void stress_cpu_djb2a(const char *name)
{
   stress_cpu_hash_generic(name, "djb2a", djb2a, 0x6a60cb5a);
}

/*
 *  fnv1a()
 *	Hash a string, using the improved 32 bit FNV-1a hash
 */
uint32_t HOT OPTIMIZE3 fnv1a(const char *str)
{
   register uint32_t hash      = 5381;
   const uint32_t    fnv_prime = 16777619;   /* 2^24 + 2^9 + 0x93 */
   register int      c;

   while ((c = *str++))
   {
      hash ^= c;
      hash *= fnv_prime;
   }
   return(hash);
}

/*
 *  stress_cpu_fnv1a()
 *	stress test hash fnv1a
 */
void HOT stress_cpu_fnv1a(const char *name)
{
   stress_cpu_hash_generic(name, "fnv1a", fnv1a, 0x8ef17e80);
}

/*
 *  sdbm()
 *	Hash a string, using the sdbm data base hash and also
 *	apparently used in GNU awk.
 */
uint32_t OPTIMIZE3 sdbm(const char *str)
{
   register uint32_t hash = 0;
   register int      c;

   while ((c = *str++))
   {
      hash = c + (hash << 6) + (hash << 16) - hash;
   }
   return(hash);
}

/*
 *  stress_cpu_sdbm()
 *	stress test hash sdbm
 */
void stress_cpu_sdbm(const char *name)
{
   stress_cpu_hash_generic(name, "sdbm", sdbm, 0x46357819);
}

/*
 *  stress_cpu_idct()
 *	compute 8x8 Inverse Discrete Cosine Transform
 */
void HOT OPTIMIZE3 stress_cpu_idct(const char *name)
{
   const double invsqrt2 = 1.0 / sqrt(2.0);
   const double pi_over_16 = M_PI / 16.0;
   const int    sz = 3;
   int          i, j, u, v;
   float        data[sz][sz];

   /*
    *  Set up DCT
    */
   for (i = 0; i < sz; i++)
   {
      for (j = 0; j < sz; j++)
      {
         data[i][j] = (i + j == 0) ? 2040: 0;
      }
   }
   for (i = 0; i < sz; i++)
   {
      const double pi_i = (i + i + 1) * pi_over_16;

      for (j = 0; j < sz; j++)
      {
         const double pi_j = (j + j + 1) * pi_over_16;
         double       sum  = 0.0;

         for (u = 0; u < sz; u++)
         {
            const double cos_pi_i_u = cos(pi_i * u);

            for (v = 0; v < sz; v++)
            {
               const double cos_pi_j_v =
                  cos(pi_j * v);

               sum += (data[u][v] *
                       (u ? 1.0 : invsqrt2) *
                       (v ? 1.0 : invsqrt2) *
                       cos_pi_i_u * cos_pi_j_v);
            }
         }
         // idct[i][j] = 0.25 * sum;
      }
   }
}

void stress_cpu_int(const char *name)
{
   stress_cpu_int_unit <uint64_t>(
      0x013f7f6dc1d79197cULL, 0x01863d2c6969a51ceULL,
      (uint64_t)C1, (uint64_t)C2, (uint64_t)C3);

   stress_cpu_int_unit <uint32_t>(
      0x1ce9b547UL, 0xa24b33aUL,
      (uint32_t)C1, (uint32_t)C2, (uint32_t)C3);

   stress_cpu_int_unit <uint16_t> (
      0x1871, 0x07f0,
      (uint16_t)C1, (uint16_t)C2, (uint16_t)C3);

   stress_cpu_int_unit <uint8_t>(
      0x12, 0x1a,
      (uint8_t)C1, (uint8_t)C2, (uint8_t)C3);
}

void stress_cpu_fp(const char *name)
{
   int    i;
   MWC    RND = MWC();
   double a = 0.18728, b = RND.mwc32(), c = RND.mwc32(), d;

   (void)name;

   for (i = 0; i < 10; i++)
   {
      float_ops <double>(a, b, c, d);
   }
   double_put(a + b + c + d);
}

/*
 *  stress_cpu_matrix_prod(void)
 *	matrix product
 */
void HOT OPTIMIZE3 stress_cpu_matrix_prod(const char *name)
{
   int       i, j, k;
   const int n   = 8;
   MWC       RND = MWC();

   long double a[n][n], b[n][n], r[n][n];
   long double v   = 1 / (long double)((uint32_t) ~0);
   long double sum = 0.0;

   (void)name;

   for (i = 0; i < n; i++)
   {
      for (j = 0; j < n; j++)
      {
         a[i][j] = (long double)RND.mwc32() * v;
         b[i][j] = (long double)RND.mwc32() * v;
         r[i][j] = 0.0;
      }
   }

   for (i = 0; i < n; i++)
   {
      for (j = 0; j < n; j++)
      {
         for (k = 0; k < n; k++)
         {
            r[i][j] += a[i][k] * b[k][j];
         }
      }
   }

   for (i = 0; i < n; i++)
   {
      for (j = 0; j < n; j++)
      {
         sum += r[i][j];
      }
   }
   double_put(sum);
}

/*
 *   stress_cpu_fibonacci()
 *	compute fibonacci series
 */
void HOT stress_cpu_fibonacci(const char *name)
{
   // const uint64_t fn_res = 0xa94fad42221f2702ULL;
   register uint64_t f1 = 0, f2 = 1, fn;
   int i = 0;

   do
   {
      fn = f1 + f2;
      f1 = f2;
      f2 = fn;
      i++;
   } while (i < 2000);

   // } while (!(fn & 0x8000000000000000ULL));
}

/*
 *  stress_cpu_psi
 *	compute the constant psi,
 *    the reciprocal Fibonacci constant
 */
void HOT OPTIMIZE3 stress_cpu_psi(const char *name)
{
   long double f1 = 0.0, f2 = 1.0;
   long double psi = 0.0, last_psi;
   long double precision = 1.0e-20;
   int         i         = 0;
   const int   max_iter  = 100;

   do
   {
      long double fn = f1 + f2;
      f1       = f2;
      f2       = fn;
      last_psi = psi;
      psi     += 1.0 / f1;
      i++;
   } while ((i < max_iter) && (fabsl(psi - last_psi) > precision));

   double_put(psi);
}

/*
 *   stress_cpu_ln2
 *	compute ln(2) using series
 */
void HOT OPTIMIZE3 stress_cpu_ln2(const char *name)
{
   long double  ln2 = 0.0, last_ln2 = 0.0;
   long double  precision = 1.0e-5;
   register int n         = 1;
   const int    max_iter  = 10000;

   /* Not the fastest converging series */
   do
   {
      last_ln2 = ln2;
      /* Unroll, do several ops */
      ln2 += (long double)1.0 / (long double)n++;
      ln2 -= (long double)1.0 / (long double)n++;
      ln2 += (long double)1.0 / (long double)n++;
      ln2 -= (long double)1.0 / (long double)n++;
      ln2 += (long double)1.0 / (long double)n++;
      ln2 -= (long double)1.0 / (long double)n++;
      ln2 += (long double)1.0 / (long double)n++;
      ln2 -= (long double)1.0 / (long double)n++;
   } while ((n < max_iter) && (fabsl(ln2 - last_ln2) > precision));

   double_put(ln2);
}

/*
 *  ackermann()
 *	a naive/simple implementation of the ackermann function
 */
uint32_t HOT ackermann(const uint32_t m, const uint32_t n)
{
   if (m == 0)
   {
      return(n + 1);
   }
   else if (n == 0)
   {
      return(ackermann(m - 1, 1));
   }
   else
   {
      return(ackermann(m - 1, ackermann(m, n - 1)));
   }
}

/*
 *   stress_cpu_ackermann
 *	compute ackermann function
 */
void stress_cpu_ackermann(const char *name)
{
   uint32_t a = ackermann(3, 2);

   uint64_put(a);
}

/*
 *   stress_cpu_explog
 *	compute exp(log(n))
 */
void HOT stress_cpu_explog(const char *name)
{
   uint32_t i;
   double   n = 1e6;

   (void)name;

   for (i = 1; i < 32; i++)
   {
      n = exp(log(n) / 1.00002);
   }
}

/*
 *  This could be a ternary operator, v = (v op val) ? a : b
 *  but it may be optimised down, so force a compare and jmp
 *  with -O0 and a if/else construct
 */
#define JMP(v, op, val, a, b) \
   if (v op val)              \
   v = a;                     \
   else                       \
   v = b;                     \
   uint64_put(next + i);

/*
 *   stress_cpu_jmp
 *	jmp conditionals
 */
void HOT stress_cpu_jmp(const char *name)
{
   register int i, next = 0;

   (void)name;

   for (i = 1; i < 100; i++)
   {
      /* Force lots of compare jmps */
      JMP(next, ==, 1, 2, 3);
      JMP(next, >, 2, 0, 1);
      JMP(next, <, 1, 1, 0);
      JMP(next, ==, 1, 2, 3);
      JMP(next, >, 2, 0, 1);
      JMP(next, <, 1, 1, 0);
      JMP(next, ==, 1, 2, 3);
      JMP(next, >, 2, 0, 1);
      JMP(next, <, 1, 1, 0);
      JMP(next, ==, 1, 2, 3);
      JMP(next, >, 2, 0, 1);
      JMP(next, <, 1, 1, 0);
   }
}

/*
 *  ccitt_crc16()
 *	perform naive CCITT CRC16
 */
uint16_t HOT OPTIMIZE3 ccitt_crc16(const uint8_t *data, size_t n)
{
   /*
    *  The CCITT CRC16 polynomial is
    *     16    12    5
    *    x   + x   + x  + 1
    *
    *  which is 0x11021, but to make the computation
    *  simpler, this has been reversed to 0x8408 and
    *  the top bit ignored..
    *  We can get away with a 17 bit polynomial
    *  being represented by a 16 bit value because
    *  we are assuming the top bit is always set.
    */
   const uint16_t    polynomial = 0x8408;
   register uint16_t crc        = ~0;

   if (!n)
   {
      return(0);
   }

   for (; n; n--)
   {
      uint8_t i;
      uint8_t val = (uint16_t)0xff & *data++;

      for (i = 8; i; --i, val >>= 1)
      {
         bool do_xor = 1 & (val ^ crc);
         crc >>= 1;
         crc  ^= do_xor ? polynomial : 0;
      }
   }

   crc = ~crc;
   return((crc << 8) | (crc >> 8));
}

/*
 *   stress_cpu_crc16
 *	compute 1024 rounds of CCITT CRC16
 */
void stress_cpu_crc16(const char *name)
{
   uint8_t buffer[16];
   size_t  i;


   random_buffer(buffer, sizeof(buffer));
   for (i = 0; i < sizeof(buffer); i++)
   {
      uint64_put(ccitt_crc16(buffer, i));
   }
}

/*
 * stress_cpu_correlate()
 *
 *  Introduction to Signal Processing,
 *  Prentice-Hall, 1995, ISBN: 0-13-209172-0.
 */
void HOT OPTIMIZE3 stress_cpu_correlate(const char *name)
{
   const size_t data_len = 128;
   const size_t corr_len = data_len / 16;
   size_t       i, j;
   double       data_average = 0.0;
   double       data[data_len], corr[corr_len + 1];

   (void)name;
   MWC RND = MWC();

   /* Generate some random data */
   for (i = 0; i < data_len; i++)
   {
      data[i]       = RND.mwc64();
      data_average += data[i];
   }
   data_average /= (double)data_len;

   /* And correlate */
   for (i = 0; i <= corr_len; i++)
   {
      corr[i] = 0.0;
      for (j = 0; j < data_len - i; j++)
      {
         corr[i] += (data[i + j] - data_average) *
                    (data[j] - data_average);
      }
      corr[i] /= (double)corr_len;
      double_put(corr[i]);
   }
}

/*
 *  stress_cpu_prime()
 *
 */
void stress_cpu_prime(const char *name)
{
   uint32_t i, nprimes = 0;

   for (i = 0; i < 500; i++)
   {
      if (is_prime(i))
      {
         nprimes++;
      }
   }
}

/*
 *  factorial()
 *	compute n!
 */
long double HOT OPTIMIZE3 factorial(int n)
{
   long double f = 1;

   while (n > 0)
   {
      f *= (long double)n;
      n--;
   }
   return(f);
}

/*
 *  stress_cpu_pi()
 *	compute pi using the Srinivasa Ramanujan
 *	fast convergence algorithm
 */
void HOT OPTIMIZE3 stress_cpu_pi(const char *name)
{
   long double       s = 0.0, pi = 0.0, last_pi = 0.0;
   const long double precision = 1.0e-20;
   const long double c         = 2.0 * sqrtl(2.0) / 9801.0;
   const int         max_iter  = 5;
   int k = 0;

   do
   {
      last_pi = pi;
      s      += (factorial(4 * k) *
                 ((26390.0 * (long double)k) + 1103)) /
                (powl(factorial(k), 4.0) * powl(396.0, 4.0 * k));
      pi = 1 / (s * c);
      k++;
   } while ((k < max_iter) && (fabsl(pi - last_pi) > precision));


   double_put(pi);
}

/*
 *  stress_cpu_omega()
 *	compute the constant omega
 *	See http://en.wikipedia.org/wiki/Omega_constant
 */
void HOT OPTIMIZE3 stress_cpu_omega(const char *name)
{
   for (int i = 0; i < 15; ++i)
   {
      long double       omega = 0.5, last_omega = 0.0;
      const long double precision = 1.0e-20;
      const int         max_iter  = i;
      int n = 0;

      /* Omega converges very quickly */
      do
      {
         last_omega = omega;
         omega      = (1 + omega) / (1 + expl(omega));
         n++;
      } while ((n < max_iter) && (fabsl(omega - last_omega) > precision));

      double_put(omega);
   }
}

#define HAMMING(G, i, nybble, code)               \
   {                                              \
      int8_t res;                                 \
      res = (((G[3] >> i) & (nybble >> 3)) & 1) ^ \
            (((G[2] >> i) & (nybble >> 2)) & 1) ^ \
            (((G[1] >> i) & (nybble >> 1)) & 1) ^ \
            (((G[0] >> i) & (nybble >> 0)) & 1);  \
      code ^= ((res & 1) << i);                   \
   }

/*
 *  hamming84()
 *	compute Hamming (8,4) codes
 */
uint8_t HOT OPTIMIZE3 hamming84(const uint8_t nybble)
{
   /*
    * Hamming (8,4) Generator matrix
    * (4 parity bits, 4 data bits)
    *
    *  p1 p2 p3 p4 d1 d2 d3 d4
    *  0  1  1  1  1  0  0  0
    *  1  0  1  1  0  1  0  0
    *  1  1  0  1  0  0  1  0
    *  1  1  1  0  0  0  0  1
    *
    * Where:
    *  d1..d4 = 4 data bits
    *  p1..p4 = 4 parity bits:
    *    p1 = d2 + d3 + d4
    *    p2 = d1 + d3 + d4
    *    p3 = d1 + d2 + d4
    *    p4 = d1 + d2 + d3
    *
    * G[] is reversed to turn G[3-j] into G[j] to save a subtraction
    */
   static const uint8_t G[] = {
      0xf1,             /* 0b11110001 */
      0xd2,             /* 0b11010010 */
      0xb4,             /* 0b10110100 */
      0x78,             /* 0b01111000 */
   };

   register uint8_t code = 0;

   /* Unrolled 8 bit loop x unrolled 4 bit loop  */
   HAMMING(G, 7, nybble, code);
   HAMMING(G, 6, nybble, code);
   HAMMING(G, 5, nybble, code);
   HAMMING(G, 4, nybble, code);
   HAMMING(G, 3, nybble, code);
   HAMMING(G, 2, nybble, code);
   HAMMING(G, 1, nybble, code);
   HAMMING(G, 0, nybble, code);

   return(code);
}

/*
 *  stress_cpu_hamming()
 *	compute hamming code on 65536 x 4 nybbles
 */
void HOT OPTIMIZE3 stress_cpu_hamming(const char *name)
{
   uint32_t i;
   uint32_t sum = 0;

   for (i = 0; i < 64; i++)
   {
      uint32_t encoded;

      /* 4 x 4 bits to 4 x 8 bits hamming encoded */
      encoded =
         (hamming84((i >> 12) & 0xf) << 24) |
         (hamming84((i >> 8) & 0xf) << 16) |
         (hamming84((i >> 4) & 0xf) << 8) |
         (hamming84((i >> 0) & 0xf) << 0);
      sum += encoded;
   }
}

/*
 *  stress_cpu_union
 *	perform bit field operations on a packed union
 */
void stress_cpu_union(const char *name)
{
   typedef union
   {
      struct
      {
         uint64_t b1  : 1;
         uint64_t b10 : 10;
         uint64_t b2  : 2;
         uint64_t b9  : 9;
         uint64_t b3  : 3;
         uint64_t b8  : 8;
         uint64_t b4  : 4;
         uint64_t b7  : 7;
         uint64_t b5  : 5;
         uint64_t b6  : 6;
      }        bits64;
      uint64_t u64 : 64;
      union
      {
         uint8_t b1 : 1;
         uint8_t b7 : 7;
         uint8_t b8 : 8;
      }        bits8;
      struct
      {
         uint16_t b15 : 15;
         uint16_t b1  : 1;
      }        bits16;
      struct
      {
         uint32_t b10 : 10;
         uint32_t b20 : 20;
         uint32_t     : 1;
         uint32_t b1  : 1;
      }        bits32;
      uint32_t u32 : 30;
   } __attribute__ ((packed)) u_t;

   static u_t u;
   size_t     i;

   (void)name;
   for (i = 0; i < 100; i++)
   {
      u.bits64.b1 ^= 1;
      u.bits64.b2--;
      u.bits32.b10 ^= ~0;
      u.bits64.b3++;
      u.bits16.b1--;
      u.bits8.b1++;
      u.bits64.b4  *= 2;
      u.bits32.b20 += 3;
      u.u64        += 0x1037fc2ae21ef829ULL;
      u.bits64.b6--;
      u.bits8.b7  *= 3;
      u.bits64.b5 += (u.bits64.b4 << 1);
      u.bits32.b1 ^= 1;
      u.bits64.b7++;
      u.bits8.b8 ^= 0xaa;
      u.bits64.b8--;
      u.bits16.b15 ^= 0xbeef;
      u.bits64.b9++;
      u.bits64.b10 *= 5;
   }
}

/*
 *  stress_cpu_all()
 *	iterate over all cpu stressors, only for single thread
 */
HOT OPTIMIZE3 void stress_cpu_all(const char *name)
{
   static int i = 1;            /* Skip over stress_cpu_all */

   // cout << cpu_methods[i].name << endl;
   cpu_methods[i].func(name);
   i++;
   if (!cpu_methods[i].func)
   {
      i = 1;
   }
}

// Constructor needs nothing
CPUStressor::CPUStressor()
{
   // count the number of benchmarks
   for (int i = 0; i < 100; ++i)
   {
      if (!methods[i].func)
      {
         numberStressor = i - 1;
         break;
      }
   }

   // start from the first benchmark
   index = 1;
}

int CPUStressor::getBenchmarkNumber()
{
   return(numberStressor);
}

std::string CPUStressor::getBenchmarkName(int id)
{
   std::string ret;

   if (id < 0 || id > numberStressor)
   {
      return(ret);
   }
   ret = std::string(methods[id].name);
   return(ret);
}

// run the benchmarks one by one
void CPUStressor::stressOnce()
{
   const char *name = "";

   methods[index].func(name);
   index++;
   if (!methods[index].func)
   {
      index = 1;
   }
}

// run the benchmark specified by index
void CPUStressor::stressWithBenchmark(int _index)
{
   if (_index < 1 || _index > numberStressor)
   {
      return;
   }

   const char *name = "";

   methods[_index].func(name);
}

int getBenchmarkId(std::string name)
{
   CPUStressor cs = CPUStressor();

   int totalNumber = cs.getBenchmarkNumber();
   int benchmarkId;
   // find the benchmark id
   bool found = false;

   if (name != "")
   {
      for (int i = 1; i <= totalNumber; ++i)
      {
         if (name == cs.getBenchmarkName(i))
         {
            benchmarkId = i;
            found       = true;
            break;
         }
      }
   }

   // set as the default benchmark if not found
   if (!found)
   {
      benchmarkId = -1;
   }
   return(benchmarkId);
}

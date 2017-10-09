/*
 * Intel Core PMU
 *
 * Copyright (c) 2006 Hewlett-Packard Development Company, L.P.
 * Contributed by Stephane Eranian <eranian@hpl.hp.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __PFMLIB_CORE_H__
#define __PFMLIB_CORE_H__

#include <perfmon/pfmlib.h>
/*
 * privilege level mask usage for Intel Core
 *
 * PFM_PLM0 = OS (kernel, hypervisor, ..)
 * PFM_PLM1 = unused (ignored)
 * PFM_PLM2 = unused (ignored)
 * PFM_PLM3 = USR (user level)
 */

#ifdef __cplusplus
extern "C" {
#endif

#define PMU_CORE_NUM_FIXED_COUNTERS 3	/* number of fixed counters */
#define PMU_CORE_NUM_GEN_COUNTERS 2	/* number of generic counters */
#define PMU_CORE_NUM_COUNTERS 5		/* number of counters */

typedef union {
	unsigned long long val;			/* complete register value */
	struct {
		unsigned long sel_event_select:8;	/* event mask */
		unsigned long sel_unit_mask:8;		/* unit mask */
		unsigned long sel_usr:1;		/* user level */
		unsigned long sel_os:1;			/* system level */
		unsigned long sel_edge:1;		/* edge detec */
		unsigned long sel_pc:1;			/* pin control */
		unsigned long sel_int:1;		/* enable APIC intr */
		unsigned long sel_res1:1;		/* reserved */
		unsigned long sel_en:1;			/* enable */
		unsigned long sel_inv:1;		/* invert counter mask */
		unsigned long sel_cnt_mask:8;		/* counter mask */
		unsigned long sel_res2:32;
	} perfevtsel;
} pfm_core_sel_reg_t;

typedef struct {
	unsigned long		cnt_mask;	/* threshold (cnt_mask)  */
	unsigned int		flags;		/* counter specific flag */
} pfmlib_core_counter_t;

#define PFM_CORE_SEL_INV	0x1	/* inverse */
#define PFM_CORE_SEL_EDGE	0x2	/* edge detect */

/*
 * model-specific parameters for the library
 */
typedef struct {
	unsigned int pebs_used; /* set to 1 if PEBS is used */
} pfmlib_core_pebs_t;

typedef struct {
	pfmlib_core_counter_t	pfp_core_counters[PMU_CORE_NUM_COUNTERS];
	pfmlib_core_pebs_t	pfp_core_pebs;
	uint64_t		reserved[4];	/* for future use */
} pfmlib_core_input_param_t;

typedef struct {
	uint64_t	reserved[8];		/* for future use */
} pfmlib_core_output_param_t;

#ifdef __cplusplus /* extern C */
}
#endif

/*
 * PMU-specific interface
 */
extern int pfm_core_is_pebs(pfmlib_event_t *e);

#endif /* __PFMLIB_CORE_H__ */

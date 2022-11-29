#ifndef _MRAM_MEMORY_H
#define _MRAM_MEMORY_H
#include <mram.h>
#define NR_SLAVE_TASKLETS (NR_TASKLETS - 1)

#define TASKLET_0_SPACE_ADDR ((__mram_ptr void *)DPU_MRAM_HEAP_POINTER)
#define TASKLET_SPACE_SIZE (0x200000)
#define TASKLET_1_SPACE_ADDR ((__mram_ptr void *)(0x2000000))
#define TASKLET_2_SPACE_ADDR ((__mram_ptr void *)(0x2200000))
#define TASKLET_3_SPACE_ADDR ((__mram_ptr void *)(0x2400000))
#define TASKLET_4_SPACE_ADDR ((__mram_ptr void *)(0x2600000))
#define TASKLET_5_SPACE_ADDR ((__mram_ptr void *)(0x2800000))
#define TASKLET_6_SPACE_ADDR ((__mram_ptr void *)(0x2a00000))
#define TASKLET_7_SPACE_ADDR ((__mram_ptr void *)(0x2c00000))
#define TASKLET_8_SPACE_ADDR ((__mram_ptr void *)(0x2e00000))
#define TASKLET_9_SPACE_ADDR ((__mram_ptr void *)(0x3000000))
#define TASKLET_10_SPACE_ADDR ((__mram_ptr void *)(0x3200000))
#define TASKLET_11_SPACE_ADDR ((__mram_ptr void *)(0x3400000))
#define TASKLET_12_SPACE_ADDR ((__mram_ptr void *)(0x3600000))
#define TASKLET_13_SPACE_ADDR ((__mram_ptr void *)(0x3800000))
#define TASKLET_14_SPACE_ADDR ((__mram_ptr void *)(0x3a00000))
#define TASKLET_15_SPACE_ADDR ((__mram_ptr void *)(0x3c00000))
#define TASKLET_16_SPACE_ADDR ((__mram_ptr void *)(0x3e00000))

#endif
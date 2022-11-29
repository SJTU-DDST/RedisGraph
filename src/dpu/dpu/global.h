#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <defs.h>
#include <stdint.h>
#include <mram.h>
#include <alloc.h>
#include "mram_alloc.h"
#include "../util/param.h"
#include "COO_Matrix_DPU.h"
#include "GB_MatrixSlice_DPU.h"
#include "mram_memory.h"
#include "heap_wram.h"

extern __host mram_allocator *allocators[NR_SLAVE_TASKLETS];

extern __mram_noinit param parameter;
extern __mram_noinit uint32_t tasklets_work[NR_SLAVE_TASKLETS+2];

extern __mram_noinit GB_MatrixSlice_head GB_MatrixSlice_head_A;
extern __mram_noinit GB_MatrixSlice_head GB_MatrixSlice_head_B;

extern __host COO_Matrix_DPU coo_result_slave[NR_SLAVE_TASKLETS];
extern __mram_ptr void * tasklets_space_addr[NR_SLAVE_TASKLETS];
extern __host GB_MatrixSlice_DPU GB_MatrixSlice_A;
extern __host GB_MatrixSlice_DPU GB_MatrixSlice_B;
extern COO_Matrix_DPU coo_result;
extern __host heap heap_for_merge[NR_TASKLETS];
#endif
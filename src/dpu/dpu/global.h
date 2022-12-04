#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <defs.h>
#include <stdint.h>
#include <mram.h>
#include <alloc.h>
#include "linear_mram_alloc.h"
#include "../util/param.h"
#include "coo_matrix_dpu.h"
#include "gb_matrix_slice_dpu.h"
#include "mram_memory.h"
#include "heap_wram.h"

extern __host linear_mram_allocator *allocators[NR_SLAVE_TASKLETS];

extern __mram_noinit query_param query_parameter;

extern __mram_noinit uint32_t tasklets_work[NR_SLAVE_TASKLETS+2];

extern __mram_noinit gb_matrix_slice_head GB_MatrixSlice_head_A;
extern __mram_noinit gb_matrix_slice_head GB_MatrixSlice_head_B;

extern __host coo_matrix_dpu coo_result_slave[NR_SLAVE_TASKLETS];
extern __mram_ptr void * tasklets_space_addr[NR_SLAVE_TASKLETS];
extern __host gb_matrix_slice_dpu GB_MatrixSlice_A;
extern __host gb_matrix_slice_dpu GB_MatrixSlice_B;

extern coo_matrix_dpu coo_result;
extern coo_matrix_dpu coo_result_tranpose;

extern __host heap heap_for_merge[NR_TASKLETS];

#endif
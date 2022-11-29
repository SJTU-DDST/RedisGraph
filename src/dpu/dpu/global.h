#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <defs.h>
#include <stdint.h>
#include <mram.h>
#include <alloc.h>
#include "mram_alloc.h"
#include "../util/param.h"
//#include "../util/COO_Matrix_DPU.h"
#include "../util/gb_matrix_slice.h"
#include "gb_matrix_slice_dpu.h"
#include "mram_memory.h"

extern __host mram_allocator *allocators[NR_SLAVE_TASKLETS];

extern __mram_noinit param parameter;
extern __mram_noinit uint32_t tasklets_work[NR_SLAVE_TASKLETS+2]; //  NR_SLAVE_TASKLETS + 1 (actual size) + 1 (for align8)

extern __mram_noinit gb_matrix_slice_head GB_MatrixSlice_head_A;
extern __mram_noinit gb_matrix_slice_head GB_MatrixSlice_head_B;
extern __mram_noinit gb_matrix_slice_head GB_MatrixSlice_head_C;

//extern __host COO_Matrix_DPU coo_matrix[NR_SLAVE_TASKLETS];

extern __mram_ptr void * tasklets_space_addr[NR_SLAVE_TASKLETS];
extern __host gb_matrix_slice_dpu GB_MatrixSlice_A;
extern __host gb_matrix_slice_dpu GB_MatrixSlice_B;
extern __host gb_matrix_slice_dpu GB_MatrixSlice_C;
extern gb_matrix_slice_dpu C_Slice_each_tsklt[NR_SLAVE_TASKLETS]; // results of each tasklet
#endif
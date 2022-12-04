#include "global.h"
__host linear_mram_allocator *allocators[NR_SLAVE_TASKLETS];

__mram_noinit query_param query_parameter;

__mram_noinit uint32_t tasklets_work[NR_SLAVE_TASKLETS + 2]; // size:NR_SLAVE_TASKLETS+1,+2 for align8

__mram_noinit gb_matrix_slice_head GB_MatrixSlice_head_A;
__mram_noinit gb_matrix_slice_head GB_MatrixSlice_head_B;

__host coo_matrix_dpu coo_result_slave[NR_SLAVE_TASKLETS];
__mram_ptr void *tasklets_space_addr[NR_SLAVE_TASKLETS] = {
    TASKLET_1_SPACE_ADDR, TASKLET_2_SPACE_ADDR, TASKLET_3_SPACE_ADDR, TASKLET_4_SPACE_ADDR,
    TASKLET_5_SPACE_ADDR, TASKLET_6_SPACE_ADDR, TASKLET_7_SPACE_ADDR, TASKLET_8_SPACE_ADDR,
    TASKLET_9_SPACE_ADDR, TASKLET_10_SPACE_ADDR, TASKLET_11_SPACE_ADDR, TASKLET_12_SPACE_ADDR,
    TASKLET_13_SPACE_ADDR, TASKLET_14_SPACE_ADDR, TASKLET_15_SPACE_ADDR, TASKLET_16_SPACE_ADDR};
__host gb_matrix_slice_dpu GB_MatrixSlice_A;
__host gb_matrix_slice_dpu GB_MatrixSlice_B;

coo_matrix_dpu coo_result = (coo_matrix_dpu)(DPU_MRAM_HEAP_POINTER + COO_RESULT_OFFSET);
coo_matrix_dpu coo_result_tranpose = (coo_matrix_dpu)(DPU_MRAM_HEAP_POINTER + COO_RESULT_TRANSPOSE_OFFSET);

__host heap heap_for_merge[NR_TASKLETS];
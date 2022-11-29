#ifndef _RDPU_H
#define _RDPU_H
#include <dpu.h>
#include <assert.h>
#include <stdint.h>
#include "../util/GB_MatrixSlice.h"
#include "../util/COO_Matrix.h"
#include "../util/param.h"
#ifndef NR_DPUS
#define NR_DPUS 4
#endif

#define NR_SLAVE_TASKLETS (NR_TASKLETS - 1)
#define DEFAULT_COO_RESULTS_CAPACITY 1024
#ifndef DPU_BINARY
#define DPU_BINARY "./dpu_task"
#endif
#define NR_MATRIX_ROW ((uint32_t)(1024 * 128))

typedef struct _dpu_push_info
{
    GB_MatrixSlice_head *Slice_head_A;
    GB_MatrixSlice_head *Slice_head_B;
    param *parameter;
    COO_Matrix *coo_results;
    uint32_t coo_results_capacity;
} dpu_push_info;

typedef struct _dpu_set_matrix_info
{
    struct dpu_set_t dpu_set;
    uint32_t nr_dpus;
    uint32_t nr_slave_tasklets;
    uint32_t nr_matrix_row;
    GrB_Type type;
    uint64_t sum_flop;
    uint32_t flop_size;
    uint32_t tasklets_work_size;
    uint32_t row_split_size;
    uint32_t *flop;
    uint32_t *tasklets_work;
    uint32_t *row_split;
    dpu_push_info push_info;
} dpu_set_matrix_info;

dpu_set_matrix_info *dpu_set_matrix_info_init(dpu_set_matrix_info *set, uint32_t nr_matrix_row, GrB_Type type);
void dpu_set_matrix_info_free(dpu_set_matrix_info *set);
dpu_set_matrix_info *dpu_set_matrix_info_reset(dpu_set_matrix_info *set, uint32_t nr_matrix_row, GrB_Type type);
dpu_push_info *dpu_push_info_init(dpu_push_info *push_info, uint32_t nr_dpus);
void dpu_push_info_free(dpu_push_info *push_info, uint32_t nr_dpus);
void dpu_set_matrix_info_dump(dpu_set_matrix_info *set);
#endif
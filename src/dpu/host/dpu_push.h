#ifndef _DPU_PUSH_H
#define _DPU_PUSH_H

#include <dpu.h>
#include <assert.h>
#include "load_balance.h"
#include "../util/gb_matrix_slice.h"
#include "../util/param.h"
#include "../util/coo_matrix.h"

typedef struct _dpu_push_info
{
    uint32_t nr_dpus;
    gb_matrix_slice_head *Slice_head_A;
    gb_matrix_slice_head *Slice_head_B;
    query_param *query_parameter;
} dpu_push_info;

// dpu_push_info
dpu_push_info *dpu_push_info_init(dpu_push_info *push_info, uint32_t nr_dpus);
void dpu_push_info_free(dpu_push_info *push_info);
void dpu_push_info_dump(dpu_push_info *push_info);
void dpu_push_mxm_gb_sparse_matrix_prepare(dpu_push_info *push_info, load_balance_info *balance_info,
                                           gb_matrix A, gb_matrix B);
void dpu_push_mxm_gb_sparse_matrix(dpu_push_info *push_info, load_balance_info *balance_info, struct dpu_set_t dpu_set,
                                   gb_matrix A, gb_matrix B);

#endif
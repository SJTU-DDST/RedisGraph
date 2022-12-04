#ifndef _LOAD_BALANCE_H
#define _LOAD_BALANCE_H
#include <stdint.h>
#include "../util/gb_matrix.h"

typedef struct _load_balance_info
{
    uint32_t vdim;
    uint32_t nr_dpu;
    uint32_t nr_slave_tasklets;
    uint64_t sum_flop;
    uint32_t flop_size;
    uint32_t tasklets_work_size;
    uint32_t row_split_size;
    uint32_t *flop;
    uint32_t *tasklets_work;
    uint32_t *row_split;
} load_balance_info;

// load_balance_info
load_balance_info *load_balance_info_init(load_balance_info *balance_info, uint32_t vdim,
                                          uint32_t nr_dpus, uint32_t nr_slave_tasklets);
void load_balance_info_free(load_balance_info *balance_info);
load_balance_info *load_balance_info_reset(load_balance_info *balance_info, uint32_t vdim);
void load_balance_info_dump(load_balance_info *balance_info);

void load_balance_mxm_gb_sparse_matrix(load_balance_info *balance_info, gb_matrix A, gb_matrix B);

#endif
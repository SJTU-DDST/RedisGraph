#ifndef _DPU_PULL_H
#define _DPU_PULL_H
#include "../util/coo_matrix.h"
#include "../util/align.h"
#include <dpu.h>

#define COO_RESULT_OFFSET 0x1000000
#define COO_RESULT_TRANSPOSE_OFFSET 0x1800000
typedef struct _dpu_pull_info
{
    coo_matrix *coo_results;
    coo_matrix *coo_results_transpose;
    uint32_t coo_results_num;
    uint32_t per_coo_results_capacity;
} dpu_pull_info;

// dpu_pull_info
dpu_pull_info *dpu_pull_info_init(dpu_pull_info *pull_info, uint32_t nr_dpus);
void dpu_pull_info_free(dpu_pull_info *pull_info);
void dpu_pull_info_dump(const dpu_pull_info *pull_info);
void dpu_pull_coo_results(dpu_pull_info *pull_info, struct dpu_set_t dpu_set);
void dpu_pull_coo_results_transpose(dpu_pull_info *pull_info, struct dpu_set_t dpu_set);

#endif
#ifndef _DPU_SET_POOL_H
#define _DPU_SET_POOL_H
#include "rdpu.h"
int dpu_set_pool_init(int num_dpu_set);
dpu_set_matrix_info *dpu_set_apply();
void dpu_set_release(dpu_set_matrix_info *dpu_set);
void dpu_set_pool_destroy();
#endif
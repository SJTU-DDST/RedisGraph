#ifndef _DPU_SET_POOL_H
#define _DPU_SET_POOL_H
#include "dpu_set_context.h"
int dpu_set_pool_init(int num_dpu_set);
dpu_set_context *dpu_set_apply();
void dpu_set_release(dpu_set_context *dpu_set);
void dpu_set_pool_destroy();
#endif
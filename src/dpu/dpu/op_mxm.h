#ifndef _OP_MXM_H
#define _OP_MXM_H
#include "global.h"
#include "merge.h"
#include "coo_matrix_dpu.h"
#include "gb_matrix_slice_dpu.h"

void mxm_prepare();
void mxm_prepare_transpose();
void mxm_tasklets_run(int transpose);
void mxm_merge(coo_matrix_dpu result);

#endif
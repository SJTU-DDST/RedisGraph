#ifndef _MXM_H
#define _MXM_H
#include <time.h>
#include "dpu_set_context.h"
#include "../util/gb_matrix.h"

dpu_set_context *mxm_gb_sparse_matrix(dpu_set_context *set, gb_matrix A, gb_matrix B, gb_matrix *C, gb_matrix *C_transpose);
#endif
#ifndef _MXM_H
#define _MXM_H
#include "rdpu.h"
#include "../util/COO_Matrix.h"
#include "../util/gb_matrix_slice.h"
#include "../util/gb_matrix.h"
#include "../util/heap.h"
#include "../util/heap_merge.h"
#include "../../../deps/GraphBLAS/Include/GraphBLAS.h"

/*
    grb_mxm:
    使用algorithm2,计算C=A*B
    其中，A_tranpose->sparsity_control=SPARSE;B->sparsity_control=SPARSE
    A_tranpose->is_csc=0,B->is_csc=0,C->is_csc=0,C_tranpose=0
    C->sparsity_control=SPARSE
*/
dpu_set_matrix_info *grb_mxm(dpu_set_matrix_info *set, gb_matrix A_tranpose, gb_matrix B ,gb_matrix* C ,gb_matrix* C_tranpose);

#endif
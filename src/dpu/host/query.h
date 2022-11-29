#ifndef _QUERY_H
#define _QUERY_H
#include <time.h>
#include "rdpu.h"
#include "../util/gb_matrix_slice.h"
#include "../util/heap.h"

/* grb_load:
    将矩阵预先存储到DPU中。
    其中，A->sparsity_control=SPARSE;A_tranpose为A的转置，A_tranpose->sparsity_control=SPARSE。
*/
dpu_set_matrix_info *grb_load(dpu_set_matrix_info *set, gb_matrix A ,gb_matrix A_tranpose);

/* grb_mxm_rowmerge:
    C=A*B
    其中，A,B,C->sparsity_control=SPARSE;
    现在，只考虑2种情况：
    1. B->matrix_id>0, 意味着B已经预先存储在DPU中，只需要将A切片
    2. B->matrix_id=0,  意味着A,B均没有存储在DPU中的情况
*/
dpu_set_matrix_info *grb_mxm_rowmerge(dpu_set_matrix_info *set, gb_matrix A, gb_matrix B, gb_matrix *C);

/* grb_add:
    C=A+B
    用于更新预先存储在DPU中的矩阵。
    其中，A->matrix_id>0,意味着B已经预先存储在DPU中。A->sparsity_control=SPARSE，B->sparsity_control=HYPERSPARSE。
*/
dpu_set_matrix_info *grb_add(dpu_set_matrix_info *set, gb_matrix A, gb_matrix B);

/* grb_sub:
    C=A-B
    用于更新预先存储在DPU中的矩阵。
    其中，A->matrix_id>0,意味着B已经预先存储在DPU中。A->sparsity_control=SPARSE，B->sparsity_control=HYPERSPARSE。
*/
dpu_set_matrix_info *grb_sub(dpu_set_matrix_info *set, gb_matrix A, gb_matrix B);

#endif
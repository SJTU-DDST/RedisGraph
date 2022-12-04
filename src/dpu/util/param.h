#ifndef __PARAM_H
#define __PARAM_H
#include <stdint.h>
#include "gb_matrix_slice.h"

typedef uint32_t opType;
#define MATRIX_MUL 1 // in C: bool
#define MATRIX_ADD 2 // in C: bool
#define MATRIX_SUB 3 // in C: bool
#define MATRIX_LOAD 3 // in C: bool

#define QUERY_PARAM_OFFSET_NUM 7
typedef struct query_param
{
    opType type;
    MatrixID matrix_id_A;
    MatrixID matrix_id_B; // if matrix_id_B >0, matrix_B is stored in dpu previously
    uint32_t param_offset[QUERY_PARAM_OFFSET_NUM];
} query_param;

// 0 - 1 : A->h
// 1 - 2 : B->h
// 2 - 3 : A->p
// 3 - 4 : B->p
// 4 - 5 : A->ix
// 5 - 6 : B->ix
#endif
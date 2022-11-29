#ifndef __GB_MATRIX_H
#define __GB_MATRIX_H
#include <stdint.h>
#include "type.h"

typedef uint32_t grb_type;
typedef int MatrixID;

#define GB_MATRIX_OK 0
#define GB_MATRIX_ERR 1

struct _gb_matrix
{
    grb_type type;   // the type of each numerical entry
    uint32_t is_csc; // true if stored by column, false if by row
    uint32_t sparsity_control;
    MatrixID matrix_id;
    int32_t plen ;          // size of A->p and A->h
    int32_t vlen ;          // length of each sparse vector
    int32_t vdim ;          // number of vectors in the matrix

    uint32_t h_size;  // exact size of A->h
    uint32_t p_size;  // exact size of A->p
    uint32_t ix_size; // exact size of indices and values

    /* data */
    int32_t *h; // pointers: p_size >= 8*(plen)
    int32_t *p; // pointers: p_size >= 8*(plen+1)
    int32_t *ix;    // [(indices,values),(indices,values),......]
};
/*
    对于矩阵:[0,1,0;
             0,0,0;
             1,0,0;
             0,0,1];
    type=BOOL;
    is_csc=0;
    sparsity_control=HYPERSPARSE;
    matrix_id=0
    plen=3;
    vlen=3;
    vdim=4;
    h_size=16;//预留存储，避免多次分配
    p_size=16;//预留存储，避免多次分配
    ix_size=16;//预留存储，避免多次分配
    h[16]={0,2,3};
    p[16]={0,1,2,3}
    ix[16]={1,0,2}
*/
typedef struct _gb_matrix *gb_matrix ;
#endif
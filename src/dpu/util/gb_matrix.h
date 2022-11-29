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
    int32_t *h; // pointers: p_size >= 8*(plen+1)
    int32_t *p; // pointers: p_size >= 8*(plen+1)
    int32_t *ix;    // [(indices,values),(indices,values),......]
};
typedef struct _gb_matrix *gb_matrix ;
#endif
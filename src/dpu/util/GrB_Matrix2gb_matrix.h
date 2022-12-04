#ifndef __GRB_MATRIX2GB_MATRIX_H
#define __GRB_MATRIX2GB_MATRIX_H

#include <stdlib.h>

#include "type.h"
#include "gb_matrix.h"
#include "../../../deps/GraphBLAS/Include/GraphBLAS.h"

gb_matrix GrB_Matrix2gb_matrix(GrB_Matrix grb)
{
    gb_matrix gb;

    gb = (gb_matrix)malloc(sizeof(struct _gb_matrix));

    if (grb->type == GrB_BOOL)
    {
        gb->type = BOOL;
    }
    else
    {
        return NULL;
    }
    gb->is_csc = grb->is_csc;

    if (grb->sparsity_control % GxB_FULL % GxB_BITMAP / GxB_SPARSE)
    {
        gb->sparsity_control = SPARSE;
    }
    else if (grb->sparsity_control % GxB_SPARSE)
    {
        gb->sparsity_control = HYPERSPARSE;
    }
    else
    {
        return NULL;
    }

    gb->matrix_id = 0;
    gb->plen = grb->nvec;
    gb->vlen = grb->vlen;
    gb->vdim = grb->vdim;
    gb->h = grb->h;
    gb->p = grb->p;
    gb->ix = grb->i;
    gb->h_size = grb->plen;
    gb->p_size = grb->plen + 1;
    gb->ix_size = grb->p[grb->plen];
    gb->nnz = grb->p[grb->plen];

    return gb;
}

void gb_matrix2GrB_Matrix(gb_matrix gb, GrB_Matrix grb)
{
    if ((!gb) || (!grb))
        return;
    if (gb->type == BOOL)
    {
        grb->type = GrB_BOOL;
    }
    else
    {
        return;
    }

    grb->is_csc = gb->is_csc;
    if (gb->sparsity_control == SPARSE)
    {
        grb->sparsity_control = GxB_SPARSE;
    }
    else if (gb->sparsity_control == HYPERSPARSE)
    {
        grb->sparsity_control = GxB_HYPERSPARSE;
    }
    else
    {
        return;
    }

    grb->plen = gb->plen;
    grb->vlen = gb->vlen;
    grb->vdim = gb->vdim;
    grb->nvec = gb->plen;
    grb->nvec_nonempty = gb->plen;
    if(grb->h)free(grb->h);
    grb->h = gb->h;
    if(grb->p)free(grb->p);
    grb->p = gb->p;
    if(grb->i)free(grb->i);
    grb->i = gb->ix;
    if(grb->x)free(grb->x);
    grb->h_size = gb->h_size * sizeof(int);
    grb->p_size = gb->p_size * sizeof(int);
    grb->i_size = gb->ix_size * sizeof(int);
    grb->x_size = 0;

    grb->nzombies = 0;
}

#endif
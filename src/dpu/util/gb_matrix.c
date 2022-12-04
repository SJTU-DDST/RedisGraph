#include "gb_matrix.h"
#include <string.h>
size_t gb_ix_elem_size(grb_type type)
{
    size_t size;
    if (type == BOOL)
        size = sizeof(int32_t);
    else
        size = sizeof(int32_t) + sizeof(int32_t);
    return size;
}
gb_matrix gb_sparse_matrix_create(grb_type type, uint32_t is_csc, uint32_t nrows,
                                  uint32_t ncols, uint32_t ix_size)
{
    gb_matrix gb;
    gb = (gb_matrix)malloc(sizeof(struct _gb_matrix));
    gb->type = type;
    gb->is_csc = is_csc;
    gb->sparsity_control = SPARSE;
    gb->matrix_id = 0;
    gb->h_size = 0;
    gb->h = NULL;

    if (is_csc)
    {
        gb->vlen = nrows;
        gb->vdim = ncols;
        gb->plen = ncols;
        gb->p_size = ncols + 1;
    }
    else
    {
        gb->vlen = ncols;
        gb->vdim = nrows;
        gb->plen = nrows;
        gb->p_size = nrows + 1;
    }
    gb->p = malloc(gb->p_size * gb_p_elem_size(type));
    int i;
    for (i = 0; i < gb->p_size; i++)
    {
        gb->p[i] = 0;
    }
    // memset(gb->p,0,gb->p_size);
    gb->ix = malloc(ix_size * gb_ix_elem_size(type));
    gb->ix_size = ix_size;
    gb->nnz = 0;
    return gb;
}
gb_matrix sorted_coo2gb_sparse(coo_matrix coo, uint32_t is_csc, uint32_t nrows, uint32_t ncols)
{
    if(!coo)
        return NULL;
    gb_matrix gb;
    gb = gb_sparse_matrix_create(coo->type, is_csc, nrows, ncols, coo->nnz);
    coo_matrix_iterator it;
    coo_matrix_iterator_init(&it, coo);
    coo_matrix_elem *elem;
    if (is_csc)
    {
        elem = coo_matrix_next_elem(&it);
        int32_t col;
        for (col = 0; col < ncols; col++)
        {
            gb->p[col + 1] = gb->p[col];
            if (!elem)
            {
                continue;
            }

            if (elem && elem->col != col)
            {
                continue;
            }

            while (elem && elem->col == col)
            {
                gb->p[col + 1]++;
                gb->ix[gb->nnz] = elem->row;
                gb->nnz++;
                elem = coo_matrix_next_elem(&it);
                /* code */
            }
        }
    }
    else
    {
        elem = coo_matrix_next_elem(&it);
        uint32_t row;
        for (row = 0; row < nrows; row++)
        {
            gb->p[row + 1] = gb->p[row];
            if (!elem)
            {
                continue;
            }

            if (elem && elem->row != row)
            {
                continue;
            }

            while (elem && elem->row == row)
            {
                gb->p[row + 1]++;
                gb->ix[gb->nnz] = elem->col;
                gb->nnz++;
                elem = coo_matrix_next_elem(&it);
                /* code */
            }
        }
    }
    return gb;
}

void gb_sparse_matrix_dump(const gb_matrix gb)
{
    if (gb->sparsity_control != SPARSE)
        return;
    if (gb->is_csc)
    {
        printf("-------------------\n");
        printf("CSC_Matrix:\n");
        uint32_t col;
        uint32_t p_ix;
        for (col = 0; col < gb->vdim; col++)
        {
            for (p_ix = gb->p[col]; p_ix < gb->p[col + 1]; p_ix++)
            {
                printf("col:%d,row:%d\n", col, gb->ix[p_ix]);
            }
        }
        printf("-------------------\n");
    }
    else
    {
        printf("-------------------\n");
        printf("CSR_Matrix:\n");
        uint32_t row;
        uint32_t p_ix;
        for (row = 0; row < gb->vdim; row++)
        {
            for (p_ix = gb->p[row]; p_ix < gb->p[row + 1]; p_ix++)
            {
                printf("row:%d,col:%d\n", row, gb->ix[p_ix]);
            }
        }
        printf("-------------------\n");
    }
}
void gb_matrix_test()
{
    gb_matrix gb;
    coo_matrix coo;

    coo = coo_matrix_create(BOOL, 16);
    coo_matrix_insert(coo, 0, 0);
    coo_matrix_insert(coo, 3, 3);
    coo_matrix_insert(coo, 5, 5);
    coo_matrix_insert(coo, 5, 6);
    coo_matrix_insert(coo, 5, 7);
    coo_matrix_dump(coo);

    gb = sorted_coo2gb_sparse(coo, 0, 8, 8);
    gb_sparse_matrix_dump(gb);
}
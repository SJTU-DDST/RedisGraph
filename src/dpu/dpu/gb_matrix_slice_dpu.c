#include "gb_matrix_slice_dpu.h"

void gb_sparse_matrix_slice_dpu_dump(const gb_matrix_slice_dpu *gbsd)
{
    if (gbsd->sparsity_control != SPARSE)
        return;
    if (gbsd->is_csc)
    {
        printf("-------------------\n");
        printf("CSC_MatrixSlice_DPU:\n");
        int32_t col;
        int32_t p_ix;
        for (col = 0; col < gbsd->p_size - 1; col++)
        {
            for (p_ix = gbsd->p[col]; p_ix < gbsd->p[col + 1]; p_ix++)
            {
                printf("col:%d,row:%d\n", col + gbsd->p_start, gbsd->ix[p_ix - gbsd->ix_start]);
            }
        }
        printf("-------------------\n");
    }
    else
    {
        printf("-------------------\n");
        printf("CSR_MatrixSlice_DPU:\n");
        int32_t row;
        int32_t p_ix;
        for (row = 0; row < gbsd->p_size - 1; row++)
        {
            for (p_ix = gbsd->p[row]; p_ix < gbsd->p[row + 1]; p_ix++)
            {
                printf("row:%d,col:%d\n", row + gbsd->p_start, gbsd->ix[p_ix - gbsd->ix_start]);
            }
        }
        printf("-------------------\n");
    }
}

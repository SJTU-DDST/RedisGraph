#include "gb_matrix_slice.h"

inline void gb_matrix_slice_head_init(gb_matrix_slice_head *head, const gb_matrix matrix)
{
    head->type = matrix->type;
    head->is_csc = matrix->is_csc;
    head->sparsity_control = matrix->sparsity_control;
    head->matrix_id = matrix->matrix_id;
}

inline void gb_matrix_slice_head_set_h(gb_matrix_slice_head* head,uint32_t h_start,uint32_t h_size){
    head->h_start=h_start;
    head->h_size=h_size;
}
inline void gb_matrix_slice_head_set_p(gb_matrix_slice_head* head,uint32_t p_start,uint32_t p_size){
    head->p_start=p_start;
    head->p_size=p_size;
}
inline void gb_matrix_slice_head_set_ix(gb_matrix_slice_head* head,uint32_t ix_start,uint32_t ix_size){
    head->ix_start=ix_start;
    head->ix_size=ix_size;
}

void gb_sparse_matrix_slice_dump(const gb_matrix_slice gbs)
{
    if (gbs->sparsity_control != SPARSE)
        return;
    if (gbs->is_csc)
    {
        printf("-------------------\n");
        printf("CSC_MatrixSlice:\n");
        int32_t col;
        int32_t p_ix;
        for (col = 0; col < gbs->p_size - 1; col++)
        {
            for (p_ix = gbs->p[col]; p_ix < gbs->p[col + 1]; p_ix++)
            {
                printf("col:%d,row:%d\n", col + gbs->p_start, gbs->ix[p_ix - gbs->ix_start]);
            }
        }
        printf("-------------------\n");
    }
    else
    {
        printf("-------------------\n");
        printf("CSR_MatrixSlice:\n");
        int32_t row;
        int32_t p_ix;
        for (row = 0; row < gbs->p_size - 1; row++)
        {
            for (p_ix = gbs->p[row]; p_ix < gbs->p[row + 1]; p_ix++)
            {
                printf("row:%d,col:%d\n", row + gbs->p_start, gbs->ix[p_ix - gbs->ix_start]);
            }
        }
        printf("-------------------\n");
    }
}

void gb_sparse_matrix_slice_head_dump(const gb_matrix_slice_head *gbsh)
{
    if (gbsh->sparsity_control != SPARSE)
        return;
    if (gbsh->is_csc)
    {
        printf("-------------------\n");
        printf("CSC_MatrixSlice_head:\n");
        printf("p_start:%u,p_size:%u,ix_start:%u,ix_size:%u\n", gbsh->p_start, gbsh->p_size, gbsh->ix_start, gbsh->ix_size);
        printf("-------------------\n");
    }
    else
    {
        printf("-------------------\n");
        printf("CSR_MatrixSlice_head:\n");
        printf("p_start:%u,p_size:%u,ix_start:%u,ix_size:%u\n", gbsh->p_start, gbsh->p_size, gbsh->ix_start, gbsh->ix_size);
        printf("-------------------\n");
    }
}
#include "dpu_push.h"
#include "load_balance.h"
#include "../util/align.h"

static void dpu_push_gb_matrix_slice_head(dpu_push_info *push_info, struct dpu_set_t dpu_set);
static void dpu_push_gb_matrix_pointers(dpu_push_info *push_info, struct dpu_set_t dpu_set, gb_matrix A, gb_matrix B);
static void dpu_push_gb_matrix_indexs_values(dpu_push_info *push_info, struct dpu_set_t dpu_set, gb_matrix A, gb_matrix B);
static void dpu_push_query_param(dpu_push_info *push_info, struct dpu_set_t dpu_set);
static void dpu_push_tasklets_work(load_balance_info *balance_info, struct dpu_set_t dpu_set);

dpu_push_info *dpu_push_info_init(dpu_push_info *push_info, uint32_t nr_dpus)
{
    if (!push_info)
        return push_info;
    push_info->nr_dpus = nr_dpus;
    push_info->Slice_head_A = malloc(sizeof(gb_matrix_slice_head) * nr_dpus);
    push_info->Slice_head_B = malloc(sizeof(gb_matrix_slice_head) * nr_dpus);
    push_info->query_parameter = malloc(sizeof(query_param));
    return push_info;
}
void dpu_push_info_free(dpu_push_info *push_info)
{
    if (!push_info)
        return;
    free(push_info->Slice_head_A);
    free(push_info->Slice_head_B);
    free(push_info->query_parameter);
}

void dpu_push_info_dump(dpu_push_info *push_info)
{
    printf("-------------------\n");
    printf("dpu_push_info:\n");
    printf("Slice_head_A:\n");
    int i;
    for (i = 0; i < push_info->nr_dpus; i++)
    {
        gb_sparse_matrix_slice_head_dump(&(push_info->Slice_head_A[i]));
    }
    printf("Slice_head_B:\n");
    for (i = 0; i < push_info->nr_dpus; i++)
    {
        gb_sparse_matrix_slice_head_dump(&(push_info->Slice_head_B[i]));
    }
    printf("query_parameter:\n");
    printf("matrix_id_A:%d\n", push_info->query_parameter->matrix_id_A);
    printf("matrix_id_B:%d\n", push_info->query_parameter->matrix_id_B);
    for (i = 0; i < QUERY_PARAM_OFFSET_NUM; i++)
    {
        printf("%u ", push_info->query_parameter->param_offset[i]);
    }
    printf("\n");
    printf("-------------------\n");
}

void dpu_push_mxm_gb_sparse_matrix_prepare(dpu_push_info *push_info, load_balance_info *balance_info,
                                           gb_matrix A, gb_matrix B)
{
    if ((A->vdim != B->vdim) || (A->sparsity_control != SPARSE) || (B->sparsity_control != SPARSE))
        return;

    int i;
    uint32_t max_p_size_A = 0;
    uint32_t max_ix_size_A = 0;
    uint32_t max_p_size_B = 0;
    uint32_t max_ix_size_B = 0;

    uint32_t start;
    uint32_t size;
    for (i = 0; i < push_info->nr_dpus; i++)
    {
        gb_matrix_slice_head_init(&(push_info->Slice_head_A[i]), A);
        gb_matrix_slice_head_init(&(push_info->Slice_head_B[i]), B);

        gb_matrix_slice_head_set_h(&(push_info->Slice_head_A[i]), 0, 0);
        gb_matrix_slice_head_set_h(&(push_info->Slice_head_B[i]), 0, 0);

        start = balance_info->row_split[i];
        size = balance_info->row_split[i + 1] - balance_info->row_split[i] + 1;
        gb_matrix_slice_head_set_p(&(push_info->Slice_head_A[i]), start, size);
        gb_matrix_slice_head_set_p(&(push_info->Slice_head_B[i]), start, size);
        if (size > max_p_size_A)
        {
            max_p_size_A = size;
        }
        if (size > max_p_size_B)
        {
            max_p_size_B = size;
        }

        start = A->p[balance_info->row_split[i]];
        size = A->p[balance_info->row_split[i + 1]] - A->p[balance_info->row_split[i]];
        gb_matrix_slice_head_set_ix(&(push_info->Slice_head_A[i]), start, size);
        if (size > max_ix_size_A)
        {
            max_ix_size_A = size;
        }

        start = B->p[balance_info->row_split[i]];
        size = B->p[balance_info->row_split[i + 1]] - B->p[balance_info->row_split[i]];
        gb_matrix_slice_head_set_ix(&(push_info->Slice_head_B[i]), start, size);
        if (size > max_ix_size_B)
        {
            max_ix_size_B = size;
        }
    }

    push_info->query_parameter->type = MATRIX_MUL;
    push_info->query_parameter->matrix_id_A = A->matrix_id;
    push_info->query_parameter->matrix_id_B = B->matrix_id;
    push_info->query_parameter->param_offset[0] = 0;           // A->h start addr
    push_info->query_parameter->param_offset[1] = 0;           // B->h start addr
    push_info->query_parameter->param_offset[2] = 0;           // A->p start addr
    size = gb_p_elem_size(A->type) * (max_p_size_A + 1); // A->p size+1
    push_info->query_parameter->param_offset[3] = push_info->query_parameter->param_offset[2] + align8(size);
    size = gb_p_elem_size(B->type) * (max_p_size_B + 1); // B->p size+1
    push_info->query_parameter->param_offset[4] = push_info->query_parameter->param_offset[3] + align8(size);
    size = gb_ix_elem_size(A->type) * max_ix_size_A; // A->ix size
    push_info->query_parameter->param_offset[5] = push_info->query_parameter->param_offset[4] + align8(size);
    size = gb_ix_elem_size(B->type) * max_ix_size_B; // A->ix size
    push_info->query_parameter->param_offset[6] = push_info->query_parameter->param_offset[5] + align8(size);

    return;
}

void dpu_push_mxm_gb_sparse_matrix(dpu_push_info *push_info, load_balance_info *balance_info, struct dpu_set_t dpu_set,
                                   gb_matrix A, gb_matrix B)
{
    dpu_push_gb_matrix_slice_head(push_info, dpu_set);
    dpu_push_gb_matrix_pointers(push_info, dpu_set, A, B);
    dpu_push_gb_matrix_indexs_values(push_info, dpu_set, A, B);
    dpu_push_query_param(push_info, dpu_set);
    dpu_push_tasklets_work(balance_info, dpu_set);
}

static void dpu_push_gb_matrix_slice_head(dpu_push_info *push_info, struct dpu_set_t dpu_set)
{
    struct dpu_set_t dpu;
    uint32_t each_dpu;
    DPU_FOREACH(dpu_set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &(push_info->Slice_head_A[each_dpu])));
    }

    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "GB_MatrixSlice_head_A", 0, align8(sizeof(gb_matrix_slice_head)), DPU_XFER_DEFAULT));

    DPU_FOREACH(dpu_set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &(push_info->Slice_head_B[each_dpu])));
    }
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "GB_MatrixSlice_head_B", 0, align8(sizeof(gb_matrix_slice_head)), DPU_XFER_DEFAULT));
}
static void dpu_push_gb_matrix_pointers(dpu_push_info *push_info, struct dpu_set_t dpu_set, gb_matrix A, gb_matrix B)
{
    struct dpu_set_t dpu;
    uint32_t each_dpu;
    DPU_FOREACH(dpu_set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, (void *)(A->p) + (push_info->Slice_head_A[each_dpu].p_start) * gb_p_elem_size(A->type)));
    }
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, push_info->query_parameter->param_offset[2],
                             push_info->query_parameter->param_offset[3] - push_info->query_parameter->param_offset[2], DPU_XFER_DEFAULT));

    DPU_FOREACH(dpu_set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, (void *)(B->p) + (push_info->Slice_head_B[each_dpu].p_start) * gb_p_elem_size(B->type)));
    }
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, push_info->query_parameter->param_offset[3],
                             push_info->query_parameter->param_offset[4] - push_info->query_parameter->param_offset[3], DPU_XFER_DEFAULT));
}
static void dpu_push_gb_matrix_indexs_values(dpu_push_info *push_info, struct dpu_set_t dpu_set, gb_matrix A, gb_matrix B)
{
    struct dpu_set_t dpu;
    uint32_t each_dpu;
    DPU_FOREACH(dpu_set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, (void *)(A->ix) + (push_info->Slice_head_A[each_dpu].ix_start) * gb_ix_elem_size(A->type)));
    }
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, push_info->query_parameter->param_offset[4],
                             push_info->query_parameter->param_offset[5] - push_info->query_parameter->param_offset[4], DPU_XFER_DEFAULT));

    DPU_FOREACH(dpu_set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, (void *)(B->ix) + (push_info->Slice_head_B[each_dpu].ix_start) * gb_ix_elem_size(B->type)));
    }
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, push_info->query_parameter->param_offset[5],
                             push_info->query_parameter->param_offset[6] - push_info->query_parameter->param_offset[5], DPU_XFER_DEFAULT));
}
static void dpu_push_query_param(dpu_push_info *push_info, struct dpu_set_t dpu_set)
{
    DPU_ASSERT(dpu_broadcast_to(dpu_set, "query_parameter", 0, push_info->query_parameter, align8(sizeof(query_param)), DPU_XFER_DEFAULT));
}
static void dpu_push_tasklets_work(load_balance_info *balance_info, struct dpu_set_t dpu_set)
{
    struct dpu_set_t dpu;
    uint32_t each_dpu;
    DPU_FOREACH(dpu_set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, balance_info->tasklets_work + balance_info->nr_slave_tasklets * each_dpu));
    }
    uint32_t length = sizeof(uint32_t) * (balance_info->nr_slave_tasklets + 1);
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "tasklets_work", 0, align8(length), DPU_XFER_DEFAULT));
}
#include "dpu_pull.h"

dpu_pull_info *dpu_pull_info_init(dpu_pull_info *pull_info, uint32_t nr_dpus)
{
    if (!pull_info)
        return NULL;
    pull_info->coo_results = malloc(sizeof(coo_matrix) * nr_dpus);
    pull_info->coo_results_transpose = malloc(sizeof(coo_matrix) * nr_dpus);
    pull_info->coo_results_num = nr_dpus;
    int i;
    for (i = 0; i < nr_dpus; i++)
    {
        pull_info->coo_results[i] = coo_matrix_create(DEFAULT, DEFAULT_COO_CAPACITY);
        pull_info->coo_results_transpose[i] = coo_matrix_create(DEFAULT, DEFAULT_COO_CAPACITY);
    }
    pull_info->per_coo_results_capacity = DEFAULT_COO_CAPACITY;
    return pull_info;
}
void dpu_pull_info_free(dpu_pull_info *pull_info)
{
    if (!pull_info)
        return;
    int i;
    for (i = 0; i < pull_info->coo_results_num; i++)
    {
        free(pull_info->coo_results[i]);
    }
    free(pull_info->coo_results);
}

void dpu_pull_info_dump(const dpu_pull_info *pull_info)
{
    printf("-------------------\n");
    printf("dpu_pull_info:\n");
    printf("per_coo_results_capacity:%d\n", pull_info->per_coo_results_capacity);
    int i;
    printf("coo_results:\n");
    for (i = 0; i < pull_info->coo_results_num; i++)
    {
        coo_matrix_dump(pull_info->coo_results[i]);
    }
    printf("\n");
    printf("-------------------\n");
}

void dpu_pull_coo_results(dpu_pull_info *pull_info, struct dpu_set_t dpu_set)
{
    struct dpu_set_t dpu;
    uint32_t each_dpu, length, max_capacity = 0;
    DPU_FOREACH(dpu_set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, pull_info->coo_results[each_dpu]));
    }
    length = sizeof(struct _coo_matrix);
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, COO_RESULT_OFFSET, align8(length), DPU_XFER_DEFAULT));
    CooType type = DEFAULT;
    if (pull_info->coo_results_num > 0)
    {
        type = pull_info->coo_results[0]->type;
    }

    for (int i = 0; i < pull_info->coo_results_num; i++)
    {
        if (max_capacity < pull_info->coo_results[i]->nnz)
        {
            max_capacity = pull_info->coo_results[i]->nnz;
        }
    }
    if (pull_info->per_coo_results_capacity < max_capacity)
    {
        for (int i = 0; i < pull_info->coo_results_num; i++)
        {
            free(pull_info->coo_results[i]);
            free(pull_info->coo_results_transpose[i]);
            pull_info->coo_results[i] = coo_matrix_create(type, align8(max_capacity));
            pull_info->coo_results_transpose[i] = coo_matrix_create(type, align8(max_capacity));
        }
        pull_info->per_coo_results_capacity = align8(max_capacity);
    }

    DPU_FOREACH(dpu_set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, pull_info->coo_results[each_dpu]));
    }
    length = sizeof(struct _coo_matrix) + coo_elem_size(type) * max_capacity;
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, COO_RESULT_OFFSET, align8(length), DPU_XFER_DEFAULT));
}

void dpu_pull_coo_results_transpose(dpu_pull_info *pull_info, struct dpu_set_t dpu_set)
{
    struct dpu_set_t dpu;
    uint32_t each_dpu, length, max_capacity = 0;
    DPU_FOREACH(dpu_set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, pull_info->coo_results_transpose[each_dpu]));
    }
    length = sizeof(struct _coo_matrix);
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, COO_RESULT_TRANSPOSE_OFFSET, align8(length), DPU_XFER_DEFAULT));
    CooType type = DEFAULT;
    if (pull_info->coo_results_num > 0)
    {
        type = pull_info->coo_results_transpose[0]->type;
    }

    for (int i = 0; i < pull_info->coo_results_num; i++)
    {
        if (max_capacity < pull_info->coo_results_transpose[i]->nnz)
        {
            max_capacity = pull_info->coo_results_transpose[i]->nnz;
        }
    }
    if (pull_info->per_coo_results_capacity < max_capacity)
    {
        for (int i = 0; i < pull_info->coo_results_num; i++)
        {
            free(pull_info->coo_results[i]);
            free(pull_info->coo_results_transpose[i]);
            pull_info->coo_results[i] = coo_matrix_create(type, align8(max_capacity));
            pull_info->coo_results_transpose[i] = coo_matrix_create(type, align8(max_capacity));
        }
        pull_info->per_coo_results_capacity = align8(max_capacity);
    }

    DPU_FOREACH(dpu_set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, pull_info->coo_results_transpose[each_dpu]));
    }
    length = sizeof(struct _coo_matrix) + coo_elem_size(type) * max_capacity;
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, COO_RESULT_TRANSPOSE_OFFSET, align8(length), DPU_XFER_DEFAULT));
}
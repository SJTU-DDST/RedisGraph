#include "op_mxm.h"
#include "../util/align.h"

static void mxm_coo_set_init(linear_mram_allocator *allocator, coo_matrix_dpu_set *coo_set,
                             uint32_t tasklet_work_start, uint32_t tasklet_work_end, int transpose);
static coo_matrix_dpu mxm_get_submatrix(linear_mram_allocator *allocator, coo_matrix_dpu coo, uint32_t ix_start_A,
                                        uint32_t ix_end_A, uint32_t ix_start_B, uint32_t ix_end_B, int transpose);

void mxm_prepare()
{
    // initial GB_MatrixSlice_A
    GB_MatrixSlice_A.type = GB_MatrixSlice_head_A.type;
    GB_MatrixSlice_A.is_csc = GB_MatrixSlice_head_A.is_csc;
    GB_MatrixSlice_A.sparsity_control = GB_MatrixSlice_head_A.sparsity_control;
    GB_MatrixSlice_A.matrix_id = GB_MatrixSlice_head_A.matrix_id;
    GB_MatrixSlice_A.h_start = GB_MatrixSlice_head_A.h_start;
    GB_MatrixSlice_A.h_size = GB_MatrixSlice_head_A.h_size;
    GB_MatrixSlice_A.p_start = GB_MatrixSlice_head_A.p_start;
    GB_MatrixSlice_A.p_size = GB_MatrixSlice_head_A.p_size;
    GB_MatrixSlice_A.ix_start = GB_MatrixSlice_head_A.ix_start;
    GB_MatrixSlice_A.ix_size = GB_MatrixSlice_head_A.ix_size;
    GB_MatrixSlice_A.h = (__mram_ptr int32_t *)(DPU_MRAM_HEAP_POINTER + query_parameter.param_offset[0]);
    GB_MatrixSlice_A.p = (__mram_ptr int32_t *)(DPU_MRAM_HEAP_POINTER + query_parameter.param_offset[2]);
    GB_MatrixSlice_A.ix = (__mram_ptr void *)(DPU_MRAM_HEAP_POINTER + query_parameter.param_offset[4]);

    // initial GB_MatrixSlice_B
    GB_MatrixSlice_B.type = GB_MatrixSlice_head_B.type;
    GB_MatrixSlice_B.is_csc = GB_MatrixSlice_head_B.is_csc;
    GB_MatrixSlice_B.p_start = GB_MatrixSlice_head_B.p_start;
    GB_MatrixSlice_B.p_size = GB_MatrixSlice_head_B.p_size;
    GB_MatrixSlice_B.ix_start = GB_MatrixSlice_head_B.ix_start;
    GB_MatrixSlice_B.ix_size = GB_MatrixSlice_head_B.ix_size;
    GB_MatrixSlice_A.h = (__mram_ptr int32_t *)(DPU_MRAM_HEAP_POINTER + query_parameter.param_offset[1]);
    GB_MatrixSlice_B.p = (__mram_ptr int32_t *)(DPU_MRAM_HEAP_POINTER + query_parameter.param_offset[3]);
    GB_MatrixSlice_B.ix = (__mram_ptr void *)(DPU_MRAM_HEAP_POINTER + query_parameter.param_offset[5]);
    // gb_sparse_matrix_slice_dpu_dump(&GB_MatrixSlice_A);
    // gb_sparse_matrix_slice_dpu_dump(&GB_MatrixSlice_B);
    int i;
    for (i = 0; i < NR_TASKLETS; i++)
    {
        heap_wram_init(&(heap_for_merge[i]), HEAP_MAX_CAPACITY, GB_MatrixSlice_A.type, 0);
    }
    for (i = 0; i < NR_SLAVE_TASKLETS; i++)
    {
        coo_result_slave[i] = NULL;
    }
}
void mxm_prepare_transpose()
{
    int i;
    for (i = 0; i < NR_TASKLETS; i++)
    {
        heap_wram_init(&(heap_for_merge[i]), HEAP_MAX_CAPACITY, GB_MatrixSlice_A.type, 1);
    }
    for (i = 0; i < NR_SLAVE_TASKLETS; i++)
    {
        coo_result_slave[i] = NULL;
    }
}
void mxm_tasklets_run(int transpose)
{
    uint32_t tasklet_id = me();
    if (tasklet_id == 0)
        return;
    uint32_t tasklet_work_start = tasklets_work[tasklet_id - 1];
    uint32_t tasklet_work_end = tasklets_work[tasklet_id];

    if (tasklet_work_end <= tasklet_work_start)
        return;

    coo_matrix_dpu_set coo_dpu_set;
    coo_matrix_dpu_set_init(allocators[tasklet_id - 1], &coo_dpu_set, tasklet_work_end - tasklet_work_start);

    mxm_coo_set_init(allocators[tasklet_id - 1], &coo_dpu_set, tasklet_work_start, tasklet_work_end, transpose);
    // if (tasklet_id == 1)
    // {
    //     coo_matrix_dpu_set_dump(&coo_dpu_set);
    // }
    while (coo_dpu_set.size > 1)
    {
        merge_coo_dpu_set(allocators[tasklet_id - 1], &(heap_for_merge[tasklet_id]), &coo_dpu_set);
    }
    // if (tasklet_id == 1)
    // {
    //     coo_matrix_dpu_set_dump(&coo_dpu_set);
    // }
    if (coo_dpu_set.size >= 1)
    {
        coo_result_slave[tasklet_id - 1] = coo_dpu_set.coo_set[0];
    }
    else
    {
        coo_result_slave[tasklet_id - 1] = NULL;
    }
    // if (tasklet_id == 1)
    // {
    //     coo_matrix_dpu_dump(coo_result_slave[tasklet_id - 1]);
    // }
}
void mxm_merge(coo_matrix_dpu result)
{
    uint32_t tasklet_id = me();
    coo_matrix_dpu_iterator it[NR_SLAVE_TASKLETS];
    uint32_t iterator_size = 0;
    uint32_t coo_result_capacity = 0;

    int i;
    for (i = 0; i < NR_SLAVE_TASKLETS; i++)
    {
        coo_matrix_dpu_iterator_init(&(it[iterator_size]), coo_result_slave[i]);
        iterator_size++;
    }

    for (i = 0; i < NR_SLAVE_TASKLETS; i++)
    {
        if (coo_result_slave[i])
        {
            coo_result_capacity += coo_result_slave[i]->nnz;
        }
    }
    result->type = GB_MatrixSlice_A.type;
    result->nnz = 0;
    result->capacity = coo_result_capacity;
    heap_merge(&(heap_for_merge[tasklet_id]), it, iterator_size, result);
}

static coo_matrix_dpu mxm_get_submatrix(linear_mram_allocator *allocator, coo_matrix_dpu coo, uint32_t ix_start_A,
                                        uint32_t ix_end_A, uint32_t ix_start_B, uint32_t ix_end_B, int transpose)
{
    if ((ix_start_A >= ix_end_A) || (ix_start_B >= ix_end_B))
        return NULL;

    coo = coo_matrix_dpu_create(allocator, GB_MatrixSlice_A.type, (ix_end_A - ix_start_A) * (ix_end_B - ix_start_B));
    uint32_t i, j;
    if (!transpose)
    {
        for (i = ix_start_A; i < ix_end_A; i++)
        {
            for (j = ix_start_B; j < ix_end_B; j++)
            {
                int32_t row = GB_MatrixSlice_A.ix[i - GB_MatrixSlice_A.ix_start];
                int32_t col = GB_MatrixSlice_B.ix[j - GB_MatrixSlice_B.ix_start];
                coo_matrix_elem elem;
                elem.row = row;
                elem.col = col;
                coo_matrix_dpu_insert_elem(coo, &elem);
                // coo_matrix_dpu_insert(coo, row, col, val);
            }
        }
    }
    else
    {
        for (j = ix_start_B; j < ix_end_B; j++)
        {
            for (i = ix_start_A; i < ix_end_A; i++)
            {
                int32_t col = GB_MatrixSlice_B.ix[j - GB_MatrixSlice_B.ix_start];
                int32_t row = GB_MatrixSlice_A.ix[i - GB_MatrixSlice_A.ix_start];
                coo_matrix_elem elem;
                elem.row = row;
                elem.col = col;
                coo_matrix_dpu_insert_elem(coo, &elem);
                // coo_matrix_dpu_insert(coo, row, col, val);
            }
        }
    }
    return coo;
}

static void mxm_coo_set_init(linear_mram_allocator *allocator, coo_matrix_dpu_set *coo_set,
                             uint32_t tasklet_work_start, uint32_t tasklet_work_end, int transpose)
{
    uint32_t ix_start_A;
    uint32_t ix_end_A;
    uint32_t ix_start_B;
    uint32_t ix_end_B;

    uint32_t row_index;
    for (row_index = tasklet_work_start; row_index < tasklet_work_end; row_index++)
    {
        ix_start_A = GB_MatrixSlice_A.p[row_index - GB_MatrixSlice_A.p_start];
        ix_end_A = GB_MatrixSlice_A.p[row_index + 1 - GB_MatrixSlice_A.p_start];
        ix_start_B = GB_MatrixSlice_B.p[row_index - GB_MatrixSlice_B.p_start];
        ix_end_B = GB_MatrixSlice_B.p[row_index + 1 - GB_MatrixSlice_B.p_start];
        if ((ix_start_A >= ix_end_A) || (ix_start_B >= ix_end_B))
            continue;
        coo_set->coo_set[coo_set->size] = mxm_get_submatrix(allocator, coo_set->coo_set[coo_set->size],
                                                            ix_start_A, ix_end_A, ix_start_B, ix_end_B, transpose);
        coo_set->size++;
    }
}
#include "mxm.h"
#include "dpu_set_context.h"

dpu_set_context *mxm_gb_sparse_matrix(dpu_set_context *set, gb_matrix A, gb_matrix B, gb_matrix *C, gb_matrix *C_transpose)
{
    if ((A->vdim != B->vdim) || (A->sparsity_control != SPARSE) || (B->sparsity_control != SPARSE))
        return set;

    if (!set)
        return set;

    clock_t begin, last, cur;

    load_balance_mxm_gb_sparse_matrix(&(set->balance_info), A, B);
    dpu_push_mxm_gb_sparse_matrix_prepare(&(set->push_info), &(set->balance_info), A, B);
    begin = clock();
    last = begin;
    cur = begin;

    dpu_push_mxm_gb_sparse_matrix(&(set->push_info), &(set->balance_info), set->dpu_set, A, B);
    last = cur;
    cur = clock();
    printf("DPU push takes %f ms, %ld cycles\n", 1e3 * (double)(cur - last) / CLOCKS_PER_SEC, cur - last);

    dpu_set_context_dpu_run(set);

    // dpu_set_context_log_read(set);
    last = cur;
    cur = clock();
    printf("DPU computation takes %f ms, %ld cycles\n", 1e3 * (double)(cur - last) / CLOCKS_PER_SEC, cur - last);

    dpu_pull_coo_results(&(set->pull_info), set->dpu_set);
    dpu_pull_coo_results_transpose(&(set->pull_info), set->dpu_set);

    // for (int i = 0; i < set->pull_info.coo_results_num; i++)
    // {
    //     coo_matrix_dump(set->pull_info.coo_results_transpose[i]);
    // }
    coo_matrix result;
    result = coo_matrix_merge(set->pull_info.coo_results, set->pull_info.coo_results_num, 0);

    *C = sorted_coo2gb_sparse(result, 0, A->vlen, B->vlen);
    result = coo_matrix_merge(set->pull_info.coo_results_transpose, set->pull_info.coo_results_num, 1);

    *C_transpose = sorted_coo2gb_sparse(result, 1, A->vlen, B->vlen);

    last = cur;
    cur = clock();
    printf("Pull result and merge takes %f ms, %ld cycles\n", 1e3 * (double)(cur - last) / CLOCKS_PER_SEC, cur - last);

    printf("Total time: %f ms, %ld cycles\n", 1e3 * (double)(cur - begin) / CLOCKS_PER_SEC, cur - begin);
    // mxm_dpu_log_read(set);
    return set;
}
#include "mxm.h"
#include "../util/heap.h"
#include "../util/heap_merge.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

// nr_rows = nr_cols
void test_diag(uint32_t nr_rows, uint32_t nr_cols, bool dump_result, bool dump_set);

int main()
{
    // test_diag(32,32,0,1);
    // test_diag(32*1024, 32*1024, 0, 0);
    // test_diag(128*1024, 128*1024, 0, 0);
    test_diag(128*1024, 128*1024, 0, 0); // n_rows: 500K, nnz: 1.5M

    // // test
    // heap_test();
    // coo_matrix_test();
    // gb_matrix_test();
}

void test_diag(uint32_t nr_rows, uint32_t nr_cols, bool dump_result, bool dump_set)
{
    dpu_set_context *set;

    coo_matrix cooA;
    cooA = coo_matrix_create(BOOL, nr_rows * 3);
    uint32_t row;
    uint32_t col;
    for (col = 0; col < nr_cols; col++)
    {
        if (col != 0)
        {
            coo_matrix_insert(cooA, col - 1, col);
        }
        coo_matrix_insert(cooA, col, col);
        if (col + 1 < nr_rows)
        {
            coo_matrix_insert(cooA, col + 1, col);
        }
    }

    coo_matrix cooB;
    cooB = coo_matrix_create(BOOL, nr_rows * 3);
    for (row = 0; row < nr_rows; row++)
    {
        if (row != 0)
        {
            coo_matrix_insert(cooB, row, row - 1);
        }
        coo_matrix_insert(cooB, row, row);
        if (row + 1 < nr_cols)
        {
            coo_matrix_insert(cooB, row, row + 1);
        }
    }

    gb_matrix cscA = sorted_coo2gb_sparse(cooA, 1, nr_rows, nr_cols);
    gb_matrix csrB = sorted_coo2gb_sparse(cooB, 0, nr_rows, nr_cols);
    gb_matrix csrC;
    gb_matrix cscC_transpose;

    set = dpu_set_context_create(cscA->vdim, cscA->type);
    set = mxm_gb_sparse_matrix(set, cscA, csrB, &csrC, &cscC_transpose);

    if (dump_result)
    {
        gb_sparse_matrix_dump(cscA);
        gb_sparse_matrix_dump(csrB);
        gb_sparse_matrix_dump(csrC);
        gb_sparse_matrix_dump(cscC_transpose);
    }
    if (dump_set)
    {
        dpu_set_context_dump(set);
    }
    dpu_set_context_free(set);

    // // test
    // heap_test();
    // coo_matrix_test();
    // gb_matrix_test();

    free(cooA);
    free(cooB);
    free(cscA->p);
    free(cscA->ix);
    free(csrB->p);
    free(csrB->ix);
    free(csrC->p);
    free(csrC->ix);
    free(cscA);
    free(csrB);
    free(csrC);
}
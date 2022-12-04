#include <barrier.h>

#include "global.h"
#include "op_mxm.h"
#include "perfcounter.h"

BARRIER_INIT(barrier1, NR_TASKLETS);
BARRIER_INIT(barrier2, NR_TASKLETS);
BARRIER_INIT(barrier3, NR_TASKLETS);
BARRIER_INIT(barrier4, NR_TASKLETS);

static void init_allocators()
{
    int i;
    for (i = 0; i < NR_SLAVE_TASKLETS; i++)
    {
        if (!allocators[i])
        {
            allocators[i] = mem_alloc(sizeof(linear_mram_allocator));
        }
        linear_mram_allocator_initial(allocators[i], tasklets_space_addr[i], TASKLET_SPACE_SIZE);
    }
}

int master()
{
    if (query_parameter.type == MATRIX_MUL)
    {
        // compute coo_result
        init_allocators();
        mxm_prepare();
        barrier_wait(&barrier1);
        barrier_wait(&barrier2);

        // perfcounter_t begin = perfcounter_config(COUNT_CYCLES, true);
        mxm_merge(coo_result);
        // printf("mxm_clean takes: %f ms \n", 1e3 * perfcounter_get() / CLOCKS_PER_SEC);

        // compute coo_result_tranpose
        init_allocators();
        mxm_prepare_transpose();
        barrier_wait(&barrier3);
        barrier_wait(&barrier4);

        mxm_merge(coo_result_tranpose);
        //coo_matrix_dpu_dump(coo_result_tranpose);
    }
}

/* Tasklets wait for the initialization to complete, then compute their
 * checksum. */
int slave()
{
    if (query_parameter.type == MATRIX_MUL)
    {
        barrier_wait(&barrier1);
        mxm_tasklets_run(0);
        barrier_wait(&barrier2);
        barrier_wait(&barrier3);
        mxm_tasklets_run(1);
        barrier_wait(&barrier4);
    }
}

int main()
{
    return me() == 0 ? master() : slave();
}
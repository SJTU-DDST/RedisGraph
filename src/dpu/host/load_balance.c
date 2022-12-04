#include "load_balance.h"

load_balance_info *load_balance_info_init(load_balance_info *balance_info, uint32_t vdim,
                                          uint32_t nr_dpus, uint32_t nr_slave_tasklets)
{
    if (!balance_info)
        return balance_info;
    balance_info->vdim = vdim;
    balance_info->nr_dpu = nr_dpus;
    balance_info->nr_slave_tasklets = nr_slave_tasklets;
    balance_info->sum_flop = 0;
    balance_info->flop_size = vdim;
    balance_info->flop = malloc(sizeof(uint32_t) * vdim);
    balance_info->tasklets_work_size = nr_dpus * nr_slave_tasklets + 1;
    balance_info->tasklets_work = malloc(sizeof(uint32_t) * balance_info->tasklets_work_size);
    balance_info->row_split_size = nr_dpus + 1;
    balance_info->row_split = malloc(sizeof(uint32_t) * balance_info->row_split_size);
}
void load_balance_info_free(load_balance_info *balance_info)
{
    if (!balance_info)
        return;
    free(balance_info->flop);
    free(balance_info->tasklets_work);
    free(balance_info->row_split);
}

load_balance_info *load_balance_info_reset(load_balance_info *balance_info, uint32_t vdim)
{
    if (!balance_info)
        return balance_info;
    balance_info->vdim = vdim;
    balance_info->sum_flop = 0;

    if (balance_info->flop_size < vdim)
    {
        free(balance_info->flop);
    }
    balance_info->flop_size = vdim;
    balance_info->flop = malloc(sizeof(uint32_t) * vdim);

    return balance_info;
}

void load_balance_info_dump(load_balance_info *balance_info)
{
    printf("-------------------\n");
    printf("load_balance_info:\n");
    printf("row_split:\n");
    int i;
    for (i = 0; i < balance_info->row_split_size; i++)
    {
        printf("%u ", balance_info->row_split[i]);
    }
    printf("\n");
    printf("tasklets_work:\n");
    for (i = 0; i < balance_info->tasklets_work_size; i++)
    {
        printf("%u ", balance_info->tasklets_work[i]);
    }
    printf("\n");
    printf("-------------------\n");
}

void load_balance_mxm_gb_sparse_matrix(load_balance_info *balance_info, gb_matrix A, gb_matrix B)
{
    if ((A->vdim != B->vdim) || (A->sparsity_control!=SPARSE) || (B->sparsity_control !=SPARSE))
        return;

    load_balance_info_reset(balance_info, A->vdim);

    for (int i = 0; i < balance_info->vdim; i++)
    {
        balance_info->flop[i] = (A->p[i + 1] - A->p[i]) * (B->p[i + 1] - B->p[i]);
        balance_info->sum_flop += balance_info->flop[i];
    }

    // Compute the matrix splits.
    uint32_t sum_flop = balance_info->sum_flop;
    uint32_t flop_per_tasklet_split = sum_flop / (balance_info->tasklets_work_size - 1);
    uint32_t curr_flop = 0;
    uint32_t row_start = 0;
    uint32_t tasklet_split_cnt = 0;
    uint32_t split_cnt = 0;
    uint32_t i;

    balance_info->tasklets_work[0] = row_start;
    for (i = 0; i < A->vdim; i++)
    {
        curr_flop += balance_info->flop[i];
        if (curr_flop >= flop_per_tasklet_split)
        {
            row_start = i + 1;
            if (tasklet_split_cnt + 1 < balance_info->tasklets_work_size)
            {
                tasklet_split_cnt++;
                balance_info->tasklets_work[tasklet_split_cnt] = row_start;
                curr_flop = 0;
            }
            else
            {
                break;
            }
        }
    }

    // Fill the last split with remaining elements
    if (curr_flop < flop_per_tasklet_split && tasklet_split_cnt + 1 < balance_info->tasklets_work_size)
    {
        tasklet_split_cnt++;
        balance_info->tasklets_work[tasklet_split_cnt] = A->vdim;
    }

    // If there are any remaining rows merge them in last partition
    if (tasklet_split_cnt >= balance_info->tasklets_work_size)
    {
        balance_info->tasklets_work[balance_info->tasklets_work_size - 1] = A->vdim;
    }

    // If there are remaining threads create empty partitions
    for (i = tasklet_split_cnt + 1; i < balance_info->tasklets_work_size; i++)
    {
        balance_info->tasklets_work[i] = A->vdim;
    }

    balance_info->row_split[0] = 0;
    for (i = 1; i < balance_info->row_split_size; i++)
    {
        balance_info->row_split[i] = balance_info->tasklets_work[i * balance_info->nr_slave_tasklets];
    }
}
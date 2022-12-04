#include "dpu_set_context.h"
#include <stdlib.h>

dpu_set_context *dpu_set_context_create(uint32_t vdim, grb_type type)
{
    dpu_set_context *set;
    set = malloc(sizeof(dpu_set_context));
    if (!set)
        return NULL;
    DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &(set->dpu_set)));
    DPU_ASSERT(dpu_load(set->dpu_set, DPU_BINARY, NULL));
    set->nr_dpus = NR_DPUS;
    set->nr_slave_tasklets = NR_SLAVE_TASKLETS;
    set->vdim = vdim;
    set->type = type;
    set->avail_flag = AVAILABLE;
    
    load_balance_info_init(&(set->balance_info), vdim, NR_DPUS, NR_SLAVE_TASKLETS);
    dpu_push_info_init(&(set->push_info), NR_DPUS);
    dpu_pull_info_init(&(set->pull_info), NR_DPUS);
    return set;
}

void dpu_set_context_free(dpu_set_context *set)
{
    if (!set)
        return;
    DPU_ASSERT(dpu_free(set->dpu_set));
    load_balance_info_free(&(set->balance_info));
    dpu_push_info_free(&(set->push_info));
    dpu_pull_info_free(&(set->pull_info));
    free(set);
}
dpu_set_context *dpu_set_context_reset(dpu_set_context *set, uint32_t vdim, grb_type type)
{
    if (!set)
    {
        set = dpu_set_context_create(vdim, type);
        return set;
    }
    set->type = type;
    set->vdim = vdim;

    load_balance_info_reset(&(set->balance_info), vdim);
    return set;
}

void dpu_set_context_dump(dpu_set_context *set)
{
    printf("-------------------\n");
    printf("dpu_set_context:\n");
    printf("nr_dpus:%u:\n", set->nr_dpus);
    printf("nr_slave_tasklets:%u:\n", set->nr_slave_tasklets);
    load_balance_info_dump(&(set->balance_info));
    dpu_push_info_dump(&(set->push_info));
    dpu_pull_info_dump(&(set->pull_info));
    printf("-------------------\n");
}

void dpu_set_context_dpu_run(dpu_set_context *set)
{
    DPU_ASSERT(dpu_launch(set->dpu_set, DPU_SYNCHRONOUS));
}

void dpu_set_context_log_read(dpu_set_context *set)
{
    struct dpu_set_t dpu;
    DPU_FOREACH(set->dpu_set, dpu)
    {
        DPU_ASSERT(dpu_log_read(dpu, stdout));
    }
}
#ifndef _RDPU_H
#define _RDPU_H
#include <dpu.h>
#include <assert.h>
#include <stdint.h>
#include "dpu_info.h"
#include "dpu_push.h"
#include "dpu_pull.h"
#include "load_balance.h"
#ifndef NR_DPUS
#define NR_DPUS 128
#endif

#define NR_SLAVE_TASKLETS (NR_TASKLETS - 1)
#define DEFAULT_COO_RESULTS_CAPACITY 1024
#ifndef DPU_BINARY
#define DPU_BINARY "./dpu_task"
#endif

#define NR_MATRIX_ROW ((uint32_t)(1024 * 128))
#define AVAILABLE 1
#define UNAVAILABLE 0

typedef struct _dpu_set_context
{
    struct dpu_set_t dpu_set;
    uint32_t nr_dpus;
    uint32_t nr_slave_tasklets;
    uint32_t vdim;
    uint32_t avail_flag;
    grb_type type;
    load_balance_info balance_info;
    dpu_push_info push_info;
    dpu_pull_info pull_info;
} dpu_set_context;

// dpu_set_context
dpu_set_context *dpu_set_context_create(uint32_t vdim, grb_type type);
void dpu_set_context_free(dpu_set_context *set);
dpu_set_context *dpu_set_context_reset(dpu_set_context *set, uint32_t vdim, grb_type type);
void dpu_set_context_dpu_run(dpu_set_context *set);
void dpu_set_context_log_read(dpu_set_context *set);
// dump
void dpu_set_context_dump(dpu_set_context *set);
#endif
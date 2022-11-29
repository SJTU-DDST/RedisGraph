#include "dpu_set_pool.h"
#include <stdio.h>
typedef struct dpu_set_pool_
{
    dpu_set_matrix_info **dpu_set_info;
    int num_dpu_set;
    volatile int num_dpu_set_avail;   /* dpu_set currently available   */
    pthread_mutex_t num_dpu_set_lock; /* used for dpu_set count etc */
} dpu_set_pool_;

typedef struct dpu_set_pool_ *dpu_set_pool;

static dpu_set_pool _dpu_set_pool = NULL; // dpu_set_pool

/* Initialise dpu_set_pool */
int dpu_set_pool_init(int num_dpu_set)
{

    if (num_dpu_set < 0)
    {
        num_dpu_set = 0;
    }

    /* Make new dpu_set_pool */
    _dpu_set_pool = malloc(sizeof(dpu_set_pool_));
    if (_dpu_set_pool == NULL)
    {
        fprintf(stderr, "dpu_set_pool_init(): Could not allocate memory for dpu_set_pool\n");
        return 0;
    }

    _dpu_set_pool->dpu_set_info = (dpu_set_matrix_info **)malloc(num_dpu_set * sizeof(dpu_set_matrix_info *));
    if (_dpu_set_pool->dpu_set_info == NULL)
    {
        fprintf(stderr, "dpu_set_pool_init(): Could not allocate memory for dpu_set_matrix_info\n");
        return 0;
    }
    _dpu_set_pool->num_dpu_set = num_dpu_set;
    _dpu_set_pool->num_dpu_set_avail = num_dpu_set;

    pthread_mutex_init(&(_dpu_set_pool->num_dpu_set_lock), NULL);

    /* dpu_set_matrix_info init */
    int i;
    for (i = 0; i < num_dpu_set; i++)
    {
        _dpu_set_pool->dpu_set_info[i] = dpu_set_matrix_info_init(_dpu_set_pool->dpu_set_info[i], 0, DEFAULT);
    }
    return num_dpu_set;
}
dpu_set_matrix_info *dpu_set_apply()
{
    // 申请一个dpu_set 资源
}
void dpu_set_release(dpu_set_matrix_info *dpu_set)
{
    // 释放dpu_set 资源
    // 可以在dpu_set_matrix_info中增加一个flag，表示是否被占用
}
void dpu_set_pool_destroy()
{
    // add code
}
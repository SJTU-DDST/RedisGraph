#include "dpu_set_pool.h"
#include <stdio.h>
#include <pthread.h>
typedef struct dpu_set_pool_
{
    dpu_set_context **dpu_set_info;
    int num_dpu_set;
    volatile int num_dpu_set_avail;   /* dpu_set currently available   */
    pthread_mutex_t num_dpu_set_lock; /* used for dpu_set count etc */
    pthread_cond_t num_dpu_set_cond;
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

    _dpu_set_pool->dpu_set_info = (dpu_set_context **)malloc(num_dpu_set * sizeof(dpu_set_context *));
    if (_dpu_set_pool->dpu_set_info == NULL)
    {
        fprintf(stderr, "dpu_set_pool_init(): Could not allocate memory for dpu_set_context\n");
        return 0;
    }
    _dpu_set_pool->num_dpu_set = num_dpu_set;
    _dpu_set_pool->num_dpu_set_avail = num_dpu_set;

    pthread_mutex_init(&(_dpu_set_pool->num_dpu_set_lock), NULL);
    pthread_cond_init(&(_dpu_set_pool->num_dpu_set_cond), NULL);

    /* dpu_set_context init */
    int i;
    for (i = 0; i < num_dpu_set; i++)
    {
        _dpu_set_pool->dpu_set_info[i] = dpu_set_context_create(0, DEFAULT);
    }
    return num_dpu_set;
}
dpu_set_context *dpu_set_apply()
{
    // 申请一个dpu_set 资源
    dpu_set_context* dpu_set_avail;
    pthread_mutex_lock(&(_dpu_set_pool->num_dpu_set_lock));
    while(_dpu_set_pool->num_dpu_set_avail<=0)
    {
        pthread_cond_wait(&(_dpu_set_pool->num_dpu_set_cond), &(_dpu_set_pool->num_dpu_set_lock));
    }
    _dpu_set_pool->num_dpu_set_avail--;
    int i;
    for(i=0;i<_dpu_set_pool->num_dpu_set;i++){
        if(_dpu_set_pool->dpu_set_info[i]->avail_flag == AVAILABLE){
            dpu_set_avail = _dpu_set_pool->dpu_set_info[i];
            dpu_set_avail -> avail_flag = UNAVAILABLE;
            pthread_mutex_unlock(&(_dpu_set_pool->num_dpu_set_lock));
            break;
        }
    }
    return dpu_set_avail;
}
void dpu_set_release(dpu_set_context *dpu_set)
{
    // 释放dpu_set 资源
    // 可以在dpu_set_context中增加一个flag，表示是否被占用
    pthread_mutex_lock(&(_dpu_set_pool->num_dpu_set_lock));
    _dpu_set_pool->num_dpu_set_avail++;
    dpu_set->avail_flag = AVAILABLE;
    //printf("%u, %u\n", dpu_set->avail_flag, _dpu_set_pool->dpu_set_info[0]->avail_flag);
    
    pthread_mutex_unlock(&(_dpu_set_pool->num_dpu_set_lock));
    pthread_cond_signal(&(_dpu_set_pool->num_dpu_set_cond));
    
}
void dpu_set_pool_destroy()
{
    // add code
    for(int i=0;i<_dpu_set_pool->num_dpu_set;i++)
    {
        if(_dpu_set_pool->dpu_set_info[i]->avail_flag == UNAVAILABLE)
        {
            fprintf(stderr, "dpu_set_pool_destroy(): Could not destory the dpu_set_pool as there is a dpu_set occupied\n");
            return;
        }
    }
    for(int i=0;i<_dpu_set_pool->num_dpu_set;i++)
    {
        dpu_set_context_free(_dpu_set_pool->dpu_set_info[i]);
    }
    free(_dpu_set_pool);
    pthread_mutex_destroy(&(_dpu_set_pool->num_dpu_set_lock));
    pthread_cond_destroy(&(_dpu_set_pool->num_dpu_set_cond));

}
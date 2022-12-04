#include "coo_matrix_dpu.h"

coo_matrix_dpu coo_matrix_dpu_create(linear_mram_allocator *allocator, CooType type, uint32_t capacity)
{
    coo_matrix_dpu coo;
    coo = linear_mram_alloc(allocator, sizeof(struct _coo_matrix) + coo_elem_size(type) * capacity);
    coo->type = type;
    coo->nnz = 0;
    coo->capacity = capacity;
    return coo;
}

coo_matrix_dpu_iterator *coo_matrix_dpu_iterator_init(coo_matrix_dpu_iterator *it, coo_matrix_dpu coo)
{
    it->index = 0;
    it->coo = coo;
    return it;
}
__mram_ptr coo_matrix_elem *coo_matrix_dpu_next_elem(coo_matrix_dpu_iterator *it)
{
    __mram_ptr coo_matrix_elem *elem;
    if (it->index >= it->coo->nnz)
    {
        return (__mram_ptr coo_matrix_elem *)NULL;
    }
    elem = &(it->coo->data[it->index]);
    it->index++;
    return elem;
}

int coo_matrix_dpu_insert(coo_matrix_dpu coo, uint32_t row, uint32_t col)
{
    if (coo->nnz >= coo->capacity)
        return COO_MATRIX_ERR;
    coo->data[coo->nnz].row = row;
    coo->data[coo->nnz].col = col;
    coo->nnz++;
    return COO_MATRIX_OK;
}

int coo_matrix_dpu_insert_elem(coo_matrix_dpu coo, coo_matrix_elem* elem)
{
    if (coo->nnz >= coo->capacity)
        return COO_MATRIX_ERR;
    coo->data[coo->nnz] = *elem;
    coo->nnz++;
    return COO_MATRIX_OK;
}

coo_matrix_dpu_set *coo_matrix_dpu_set_init(linear_mram_allocator *allocator, coo_matrix_dpu_set *set, uint32_t capacity)
{
    set->capacity = capacity;
    set->size = 0;
    set->coo_set = linear_mram_alloc(allocator, capacity * sizeof(coo_matrix_dpu));
    return set;
}
int coo_matrix_dpu_set_insert(coo_matrix_dpu_set *set, coo_matrix_dpu coo)
{
    if (set->size >= set->capacity)
    {
        return COO_MATRIX_ERR;
    }
    set->coo_set[set->size] = coo;
    set->size++;
    return COO_MATRIX_OK;
}
int coo_matrix_dpu_set_copy(coo_matrix_dpu_set *set_dst, coo_matrix_dpu_set *set_src)
{
    set_dst->capacity = set_src->capacity;
    set_dst->size = set_src->size;
    set_dst->coo_set = set_src->coo_set;
    return COO_MATRIX_OK;
}
int coo_matrix_dpu_set_clear(linear_mram_allocator *allocator, coo_matrix_dpu_set *set)
{
    if (set->coo_set)
    {
        uint32_t i;
        for (i = 0; i < set->size; i++)
        {
            linear_mram_free(allocator, set->coo_set[i]);
        }
        linear_mram_free(allocator, set->coo_set);
    }
    set->capacity = 0;
    set->size = 0;
    set->coo_set = (__mram_ptr coo_matrix_dpu *)NULL;
    return COO_MATRIX_OK;
}

void coo_matrix_dpu_dump(const coo_matrix_dpu coo)
{
    coo_matrix_dpu_iterator it;
    coo_matrix_dpu_iterator_init(&it, coo);
    __mram_ptr coo_matrix_elem *elem = coo_matrix_dpu_next_elem(&it);
    printf("-------------------\n");
    printf("COO_Matrix_DPU:\n");
    while (elem)
    {
        printf("row:%d,col:%d\n", elem->row, elem->col);
        elem = coo_matrix_dpu_next_elem(&it);
    }
    printf("-------------------\n");
}

void coo_matrix_dpu_set_dump(const coo_matrix_dpu_set *set)
{
    printf("-------------------\n");
    printf("COO_Matrix_DPU_SET:\n");
    printf("coo_matrix_dpu_set_size:%u\n", set->size);
    uint32_t i;
    for (i = 0; i < set->size; i++)
    {
        coo_matrix_dpu_dump(set->coo_set[i]);
    }
    printf("-------------------\n");
}
#include "coo_matrix.h"
#include "heap_merge.h"

coo_matrix coo_matrix_create(CooType type, uint32_t capacity)
{
    coo_matrix coo;
    coo = (coo_matrix)malloc(sizeof(struct _coo_matrix) + coo_elem_size(type) * capacity);
    coo->type = type;
    coo->nnz = 0;
    coo->capacity = capacity;
    return coo;
}
int coo_matrix_insert(coo_matrix coo, int32_t row, int32_t col)
{
    if (coo->nnz >= coo->capacity)
        return COO_MATRIX_ERR;

    uint32_t nnz = coo->nnz;
    coo->data[nnz].row = row;
    coo->data[nnz].col = col;
    coo->nnz++;
    return COO_MATRIX_OK;
}

coo_matrix_iterator *coo_matrix_iterator_init(coo_matrix_iterator *it, coo_matrix coo)
{
    it->index = 0;
    it->coo = coo;
    return it;
}
coo_matrix_elem *coo_matrix_next_elem(coo_matrix_iterator *it)
{
    coo_matrix_elem *elem;
    if (it->index >= it->coo->nnz)
    {
        return NULL;
    }
    elem = &(it->coo->data[it->index]);
    it->index++;
    return elem;
}

void coo_matrix_dump(const coo_matrix coo)
{
    coo_matrix_iterator it;
    coo_matrix_iterator_init(&it, coo);
    coo_matrix_elem *elem = coo_matrix_next_elem(&it);
    printf("-------------------\n");
    printf("COO_Matrix:\n");
    while (elem)
    {
        printf("row:%d,col:%d\n", elem->row, elem->col);
        elem = coo_matrix_next_elem(&it);
    }
    printf("-------------------\n");
}

coo_matrix coo_matrix_merge(const coo_matrix *coo_results, uint32_t num, int transpose)
{
    coo_matrix result;
    heap h;
    uint32_t capacity = 0;
    CooType type = DEFAULT;
    if (num > 0)
    {
        type = coo_results[0]->type;
    }
    else
    {
        return NULL;
    }
    int i;
    for (i = 0; i < num; i++)
    {
        if (coo_results[i])
        {
            capacity += coo_results[i]->nnz;
            if (coo_results[i]->type != type)
                return NULL;
        }
    }
    result = coo_matrix_create(type, capacity);

    coo_matrix_iterator *it = malloc(num * sizeof(coo_matrix_iterator));
    for (i = 0; i < num; i++)
    {
        coo_matrix_iterator_init(&(it[i]), coo_results[i]);
    }

    heap_init(&h, num, type, transpose);
    coo_merge(&h, it, num, result);
    free(it);
    return result;
}
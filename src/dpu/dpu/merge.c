#include "merge.h"

void merge_coo_dpu_set(linear_mram_allocator *allocator, heap *h, coo_matrix_dpu_set *coo_dpu_set)
{
    coo_matrix_dpu_set new_coo_dpu_set;
    uint32_t new_capacity = coo_dpu_set->size / h->capacity;
    if (coo_dpu_set->size % h->capacity)
        new_capacity++;

    coo_matrix_dpu_set_init(allocator, &new_coo_dpu_set, new_capacity);

    uint32_t i, j;
    // initial
    for (i = 0; i < new_coo_dpu_set.capacity; i++)
    {
        coo_matrix_dpu new_coo_dpu;

        uint32_t coo_capacity = 0;
        j = i * (h->capacity);
        while (j < (i + 1) * (h->capacity) && j < coo_dpu_set->size)
        {
            /* code */
            coo_capacity += coo_dpu_set->coo_set[j]->nnz;
            j++;
        }
        new_coo_dpu = coo_matrix_dpu_create(allocator, GB_MatrixSlice_A.type, coo_capacity);
        coo_matrix_dpu_set_insert(&new_coo_dpu_set, new_coo_dpu);
    }
    // merge
    coo_matrix_dpu_iterator it[HEAP_MAX_CAPACITY];
    uint32_t iterator_size;
    for (i = 0; i < new_coo_dpu_set.size; i++)
    {
        coo_matrix_dpu new_coo_dpu = new_coo_dpu_set.coo_set[i];

        j = i * (h->capacity);
        iterator_size = 0;
        while (j < (i + 1) * (h->capacity) && j < coo_dpu_set->size)
        {
            /* code */
            coo_matrix_dpu_iterator_init(&(it[iterator_size]), coo_dpu_set->coo_set[j]);
            j++;
            iterator_size++;
        }
        heap_merge(h, it, iterator_size, new_coo_dpu);
    }
    coo_matrix_dpu_set_clear(allocator, coo_dpu_set);
    coo_matrix_dpu_set_copy(coo_dpu_set, &(new_coo_dpu_set));
}

void heap_merge(heap *h, coo_matrix_dpu_iterator *iterator_set, uint32_t iterator_set_size, coo_matrix_dpu result)
{
    int32_t k;
    __dma_aligned heap_node node;
    heap_node *node_top;
    __mram_ptr coo_matrix_elem *elem_ptr;

    heap_clean(h);
    for (k = 0; k < iterator_set_size; k++)
    {
        elem_ptr = coo_matrix_dpu_next_elem(&(iterator_set[k]));
        if (elem_ptr)
        {
            node.row = elem_ptr->row;
            node.col = elem_ptr->col;
            node.belong_to = k;
            heap_insert(h, &node);
        }
    }
    while (!heap_empty(h))
    {
        /* code */
        k = heap_top_try_merge_equalnode(h);
        while (k >= 0)
        {
            /* code */
            elem_ptr = coo_matrix_dpu_next_elem(&(iterator_set[k]));
            if (elem_ptr)
            {
                node.row = elem_ptr->row;
                node.col = elem_ptr->col;
                node.belong_to = k;
                heap_insert(h, &node);
            }
            k = heap_top_try_merge_equalnode(h);
        }

        node_top = heap_top(h);
        if (node_top)
        {
            coo_matrix_elem elem;
            elem.row = node_top->row;
            elem.col = node_top->col;
            coo_matrix_dpu_insert_elem(result, &elem);
            k = node_top->belong_to;

            heap_remove(h);

            elem_ptr = coo_matrix_dpu_next_elem(&(iterator_set[k]));
            if (elem_ptr)
            {
                node.row = elem_ptr->row;
                node.col = elem_ptr->col;
                node.belong_to = k;
                heap_insert(h, &node);
            }
        }
    }
}
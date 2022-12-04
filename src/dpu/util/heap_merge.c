#include "heap_merge.h"

void coo_merge(heap *h, coo_matrix_iterator  *iterator_set, uint32_t iterator_set_size, coo_matrix result)
{
    int32_t k;
    heap_node node;
    heap_node *node_top;
    coo_matrix_elem *elem_ptr;

    heap_clean(h);
    for (k = 0; k < iterator_set_size; k++)
    {
        elem_ptr = coo_matrix_next_elem(&(iterator_set[k]));
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
            elem_ptr = coo_matrix_next_elem(&(iterator_set[k]));
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
            coo_matrix_insert(result, node_top->row, node_top->col);
            k = node_top->belong_to;

            heap_remove(h);

            elem_ptr = coo_matrix_next_elem(&(iterator_set[k]));
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
void coo_merge_test()
{
    coo_matrix coo[3];
    coo_matrix result;
    coo[0] = coo_matrix_create(BOOL, 4);
    coo[1] = coo_matrix_create(BOOL, 9);
    coo[2] = coo_matrix_create(BOOL, 9);
    result = coo_matrix_create(BOOL, 25);
    coo_matrix_insert(coo[0], 0, 0);
    coo_matrix_insert(coo[0], 0, 1);
    coo_matrix_insert(coo[0], 1, 0);
    coo_matrix_insert(coo[0], 1, 1);

    coo_matrix_insert(coo[1], 0, 0);
    coo_matrix_insert(coo[1], 0, 1);
    coo_matrix_insert(coo[1], 0, 2);
    coo_matrix_insert(coo[1], 1, 0);
    coo_matrix_insert(coo[1], 1, 1);
    coo_matrix_insert(coo[1], 1, 2);
    coo_matrix_insert(coo[1], 2, 0);
    coo_matrix_insert(coo[1], 2, 1);
    coo_matrix_insert(coo[1], 2, 2);

    coo_matrix_insert(coo[2], 1, 1);
    coo_matrix_insert(coo[2], 1, 2);
    coo_matrix_insert(coo[2], 1, 3);
    coo_matrix_insert(coo[2], 2, 1);
    coo_matrix_insert(coo[2], 2, 2);
    coo_matrix_insert(coo[2], 2, 3);
    coo_matrix_insert(coo[2], 3, 1);
    coo_matrix_insert(coo[2], 3, 2);
    coo_matrix_insert(coo[2], 3, 3);

    coo_matrix_iterator  it[3];

    coo_matrix_iterator_init(&(it[0]),coo[0]);
    coo_matrix_iterator_init(&(it[1]),coo[1]);
    coo_matrix_iterator_init(&(it[2]),coo[2]);

    heap h;
    heap_init(&h,16,BOOL,0);
    coo_merge(&h,it,3,result);
    coo_matrix_dump(result);
}
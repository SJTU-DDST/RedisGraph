#include "heap.h"
// 交换数组的元素arr[idx1]和arr[idx2]
static void swap(heap_node *arr, int idx1, int idx2)
{
    heap_node tmp = arr[idx1];
    arr[idx1] = arr[idx2];
    arr[idx2] = tmp;
}

int heap_node_compare(heap_node *a, heap_node *b)
{
    if (a->row > b->row)
    {
        return 1;
    }
    else if (a->row < b->row)
    {
        return -1;
    }
    else
    {
        if (a->col > b->col)
        {
            return 1;
        }
        else if (a->col < b->col)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}
int heap_node_compare_transpose(heap_node *a, heap_node *b)
{
    if (a->col > b->col)
    {
        return 1;
    }
    else if (a->col < b->col)
    {
        return -1;
    }
    else
    {
        if (a->row > b->row)
        {
            return 1;
        }
        else if (a->row < b->row)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}
static void exchange_from_top(heap *h, int pos)
{
    int i = pos;
    heap_node *heap = h->heap;
    while (true)
    {
        int min_pos = i;
        if (i * 2 <= h->size && h->node_compare_function(&(heap[i]), &(heap[i * 2])) > 0)
            min_pos = i * 2; //和左子节点比较
        if (i * 2 + 1 <= h->size && h->node_compare_function(&(heap[i]), &(heap[i * 2 + 1])) > 0 &&
            h->node_compare_function(&(heap[i * 2]), &(heap[i * 2 + 1])) > 0)
            min_pos = i * 2 + 1; //和右子节点比较
        if (min_pos == i)
            break;              // 不再发生交换, 当前位置就是最大位置
        swap(heap, i, min_pos); //将当前节点和子节点进行交换
        i = min_pos;            //将i设置为子节点的索引
    }
}
// 打印堆中的元素
void heap_dump(const heap *h)
{
    int size = h->size;
    heap_node *heap = h->heap;
    int i;
    printf("-------------------\n");
    printf("heap:\n");
    for (i = 1; i <= size; i++)
    {
        printf("row:%d,col:%d,belong_to:%u\n", heap[i].row, heap[i].col, heap[i].belong_to);
    }
    printf("-------------------\n");
}

heap *heap_init(heap *h, uint32_t capacity, heapType type, int transpose)
{
    h->type = type;
    h->capacity = capacity;
    h->size = 0;
    if (!transpose)
        h->node_compare_function = heap_node_compare;
    else
        h->node_compare_function = heap_node_compare_transpose;
    h->heap = (heap_node *)malloc(sizeof(heap_node) * (capacity + 1));
    return h;
}
heap *heap_clean(heap *h)
{
    h->size = 0;

    return h;
}
int heap_empty(heap *h)
{
    if (h->size <= 0)
        return 1;
    return 0;
}
//插入元素
int heap_insert(heap *h, heap_node *node)
{

    int size = h->size;
    int capacity = h->capacity;
    if (size >= capacity)
        return HEAP_ERR;

    // 在数组最后加入新的元素
    heap_node *heap = h->heap;
    heap[++size] = *node;
    h->size = size;

    int i = size; // 数组索引
    //堆化,
    while ((i / 2) > 0)
    {
        if (h->node_compare_function(&(heap[i]), &(heap[i / 2])) < 0)
        {
            swap(heap, i, i / 2); //交换父子节点
            i = i / 2;
        }
        else
        {
            break;
        }
    }
    return HEAP_OK;
}

int heap_top_try_merge_equalnode(heap *h)
{
    int ret;
    int i = 1;
    heap_node *heap = h->heap;
    if (h->size < 2 * i)
        return -1;
    if (h->node_compare_function(&(heap[i]), &(heap[2 * i])) == 0)
    {
        ret = heap[2 * i].belong_to;
        heap[2 * i] = heap[h->size];
        h->size--;
        exchange_from_top(h, 2 * i);
        return ret;
    }
    if (h->size < 2 * i + 1)
        return -1;
    if (h->node_compare_function(&(heap[i]), &(heap[2 * i + 1])) == 0)
    {
        ret = heap[2 * i + 1].belong_to;
        heap[2 * i + 1] = heap[h->size];
        h->size--;
        exchange_from_top(h, 2 * i + 1);
        return ret;
    }
    return -1;
}
heap_node *heap_top(heap *h)
{

    if (h->size < 1)
        return NULL;

    return &(h->heap[1]);
}
//删除顶部元素
int heap_remove(heap *h)
{

    if (h->size < 1)
        return HEAP_ERR;

    int size = h->size;
    heap_node *heap = h->heap;
    //用最后一个元素替代第一个元素
    heap[1] = heap[size];
    //删除最后一个元素
    size--;
    h->size = size;
    // 自上而下堆化
    int i = 1;
    exchange_from_top(h, i);
    return HEAP_OK;
}

int heap_test()
{
    heap h;
    heap_init(&h, 16, BOOL, 0);

    heap_node arr[] = {{2, 4, 1}, {2, 2, 1}, {2, 3, 1}, {2, 2, 1}, {2, 6, 1}, {2, 1, 1}};
    int i;
    for (i = 0; i < sizeof(arr) / sizeof(heap_node); i++)
    {
        heap_insert(&h, &(arr[i]));
    }

    heap_dump(&h);

    printf("--------\n");
    int belong_to = heap_top_try_merge_equalnode(&h);
    printf("belong to:%d\n", belong_to);
    heap_node *top = heap_top(&h);
    if (top)
        printf("top:%d,%d\n", top->row, top->col);
    heap_remove(&h);

    belong_to = heap_top_try_merge_equalnode(&h);
    printf("belong to:%d\n", belong_to);
    top = heap_top(&h);
    if (top)
        printf("top:%d,%d\n", top->row, top->col);
    heap_remove(&h);

    heap_dump(&h);
}
#include "list.h"
typedef bool (function)(struct list_elem*, int arg);

//初始化双向链表
void init_list(struct list *li)
{
    li->head.prev = (void*)0;
    li->head.next = &li->tail;
    li->tail.next = (void *)0;
    li->tail.prev = &li->head;
}

void insert_list_front(struct ListPtr *ptr, struct ListPtr *elem)
{
    enum InterruptStatus status = Off_interrupt();
    ptr->prev->next = elem;
    elem->next = ptr;
    elem->prev = ptr->prev;
    ptr->prev = elem;
    set_InterruptStatus(status);
}
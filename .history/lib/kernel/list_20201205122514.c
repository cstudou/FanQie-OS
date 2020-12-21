#include "list.h"
//typedef bool (function)(struct list_elem*, int arg);

//初始化双向链表
void init_list(struct list *li)
{
    li->head.prev = (void*)0;
    li->head.next = &li->tail;
    li->tail.next = (void *)0;
    li->tail.prev = &li->head;
}

//在ptr之前插入
void list_insert_front(struct ListPtr *ptr, struct ListPtr *elem)
{
    enum InterruptStatus status = Off_interrupt();
    ptr->prev->next = elem;
    elem->next = ptr;
    elem->prev = ptr->prev;
    ptr->prev = elem;
    set_InterruptStatus(status);
}

void list_push_back(struct list *li, struct ListPtr *elem)
{
    list_insert_front(&li->tail, elem);
}
void list_push_frong(struct list *li, struct ListPtr *elem)
{
    list_insert_front(li->head.next, elem);
}

//删除元素elem
void list_remove(struct ListPtr *elem)
{
    enum InterruptStatus status = Off_interrupt();
    if(elem->prev == (void *)0)
    {
        elem->prev->next = elem->next->prev = (void *)0;
    }
    else
    {
        elem->prev->next = elem->next;
        elem->next->prev = elem->prev;
    }
    set_InterruptStatus(status);
}

int list_find(struct list *li, struct ListPtr *elem)
{
    struct ListPtr temp = li->head->next;
    while(elem != &li->tail)
    {
        if(elem == li->head.next)
        {
            return true;
        }

    }
}
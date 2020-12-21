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

uint8_t list_find(struct list *li, struct ListPtr *elem)
{
    struct ListPtr *temp = li->head.next;
    while(elem != &li->tail)
    {
        if(elem == temp)
        {
            return true;
        }
        temp = temp->next;

    }
    return false;
}

uint8_t list_empty(struct list *li)
{
    if(li->head.next == &li->tail)
    {
        return true;
    }
    return false;
}
//遍历列表内所有元素，逐个判断是否有符合条件的元素，把列表中每个元素都传给function
//arg判断elem是否符合条件
struct ListPtr* list_traversal(struct list *li, void (*function)(void *), int arg)
{
    if(list_empty(li))
    {
        return (void *)0;
    }
    struct ListPtr *temp = li->head.next;
}
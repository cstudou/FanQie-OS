#include "List.h"


//初始化双向链表
void list_init(struct List *li)
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

void list_push_back(struct List *li, struct ListPtr *elem)
{
    list_insert_front(&li->tail, elem);
}
void list_push_front(struct List *li, struct ListPtr *elem)
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

uint8_t list_find(struct List *li, struct ListPtr *elem)
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

uint8_t list_empty(struct List *li)
{
    if(li->head.next == &li->tail)
    {
        return true;
    }
    return false;
}
//遍历列表内所有元素，逐个判断是否有符合条件的元素，把列表中每个元素都传给function
//arg判断elem是否符合条件
struct ListPtr* list_traversal(struct List *li, int (*function)(struct ListPtr *, int), int arg)
{
    if(list_empty(li))
    {
        return (void *)0;
    }

    struct ListPtr *temp = li->head.next;
    //遍历所有元素
    while(temp != &li->tail)
    {   
        if(function(temp, arg))
        {
            return temp;
        }
        temp = temp->next;
    }
    return (void *)0;
}

uint32_t list_size(struct List *li)
{
    uint32_t count = 0;
    struct ListPtr *temp = li->head.next;
    while(temp != &li->tail)
    {
        temp = temp->next;
        count ++;
    }
    return count;
}
#ifndef __FANQIE_List_H__
#define __FANQIE_List_H__

#include "../../kernel/interrupt.h"
#define false 0
#define true 1
//双向链表
//结构 head---元素---tail
struct ListPtr
{
    struct ListPtr *prev;
    struct ListPtr *next;
};
struct List
{
    //头
    struct ListPtr head;
    //尾
    struct ListPtr tail;
};

//初始化双向链表
void list_init(struct List *li);

//在ptr之前插入
void list_insert_front(struct ListPtr *ptr, struct ListPtr *elem);

void list_push_back(struct List *li, struct ListPtr *elem);
void list_push_front(struct List *li, struct ListPtr *elem);

//删除元素elem
void list_remove(struct ListPtr *elem);

uint8_t list_find(struct List *li, struct ListPtr *elem);

uint8_t list_empty(struct List *li);
//遍历列表内所有元素，逐个判断是否有符合条件的元素，把列表中每个元素都传给function
//arg判断elem是否符合条件
struct ListPtr* list_traversal(struct List *li, int (*function)(struct ListPtr *, int), int arg);

uint32_t list_size(struct List *li);
#endif
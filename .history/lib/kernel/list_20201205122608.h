#ifndef __FANQIE_LIST_H__
#define __FANQIE_LIST_H__

#include "../../kernel/interrupt.h"
#define false 0
#define true 1
//双向链表
struct ListPtr
{
    struct ListPtr *prev;
    struct ListPtr *next;
};
struct list
{
    //头
    struct ListPtr head;
    //尾
    struct ListPtr tail;
};
#endif
#ifndef __FANQIE_LIST_H__
#define __FANQIE_LIST_H__

//双向链表
struct list
{
    //头
    struct list *head_prev;
    struct list *head_next;
    //尾
    struct list *tail_prev;
    struct list *tail_next;
};
#endif
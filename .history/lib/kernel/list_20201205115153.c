#include "list.h"
typedef bool (function)(struct list_elem*, int arg);

void init_list(struct list *li)
{
    li->head.prev = NULL;
}
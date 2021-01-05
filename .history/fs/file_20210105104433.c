#include "file.h"
#define MAX_FILE_NUM 32;

struct File file_table[MAX_FILE_NUM];

uint32_t get_slot()
{
    //标准输入、输出、错误---0，1，2
    uint32_t index = 3;
    for(; index<32; ++index)
    {
        if(file_table[index].inode == (void*)0)
        {

        }
    }
}
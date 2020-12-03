#ifndef __FANQIE_STRING_H__
#define __FANQIE_STRING_H__

#include "stdint.h"
#include "../../kernel/assert.h"
#define NULL ((void*)0)
#define bool int
void memset(void *start, uint8_t data, uint32_t size);

//把数据从souce拷贝到dist，拷贝size个
void memcpy(void *dist, const void *source, uint32_t size);

//比较a,b的size字节 相等返回0 a>b返回1 否则返回-1
int8_t memcmp(const void *a, const void *b, uint32_t size);


char *strcpy(char *begin, const char *source);

uint32_t strlen(const char *begin);

int8_t strcmp(const char *a, const char *b);

//查找str中首次出现ch的地址,从左到右
char *strchr(const char *str, const uint8_t ch);

char *strrchr(const char *str, const uint8_t ch);

char *strcat(char *dist, char *source);

//在字符串中查找字符出现次数
uint32_t strchrs(const char *str, const uint8_t ch);
#endif
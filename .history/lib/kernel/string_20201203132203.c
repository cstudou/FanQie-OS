#include "string.h"


void memset(void *start, uint8_t data, uint32_t size)
{
    ASSERT(start != NULL);
    uint8_t *begin = (uint8_t*)(start);
    int i=0;
    for(; i<size; ++i)
    {
        *begin = data;
        begin ++;
    }
}

//把数据从souce拷贝到dist，拷贝size个
void memcpy(void *dist, const void *source, uint32_t size)
{
    ASSERT((dist != NULL) && (source != NULL));
    uint8_t *begin = (uint8_t *)(dist);
    const uint8_t *head = (const uint8_t *)(source);
    int i = 0;
    for(; i<size; ++i)
    {
        *begin++ = *head++;
    }
}

//比较a,b的size字节 相等返回0 a>b返回1 否则返回-1
int8_t memcmp(const void *a, const void *b, uint32_t size)
{
    ASSERT((a != NULL) && (b != NULL));
    const uint8_t *source = (const uint8_t *)(a);
    const uint8_t *head = (const uint8_t *)(b);
    int i=0;
    for(; i<size; i++)
    {
        if(*source != *head)
        {
            return *source > *head ? 1 : -1;
        }
        source ++;
        head ++;
    }
    return 0;
}


char *strcpy(char *begin, const char *source)
{
    ASSERT((begin != NULL) && (source != NULL));
    char *ret = begin;
    while(*source != '\0')
    {
        *begin ++ = *source ++;
    }
    *begin = '\0';
    return ret;
}

uint32_t strlen(const char *begin)
{
    ASSERT(begin != NULL);
    uint32_t len = 0;
    while(*begin != '\0')
    {
        begin++;
        len++;
    }
    return len;
}

int8_t strcmp(const char *a, const char *b)
{
    ASSERT((a != NULL) && (b != NULL));
    uint32_t len = strlen(a);
    while(len && *a == *b)
    {
        a++;
        b++;
    }
    return *a < *b ? 1 : *a > *b;   // *a == *b返回0
}

//查找str中首次出现ch的地址,从左到右
char *strchr(const char *str, const uint8_t ch)
{
    ASSERT(str != NULL);
    while(*str != '\0')
    {
        if(*str == ch)
        {
            return (char *)str;
        }
        str ++;
    }
    return NULL;
}

char *strrchr(const char *str, const uint8_t ch)
{
    ASSERT(str != NULL);
    uint32_t len = strlen(str);
    while(len-- > 0)
    {
        if(str[len] == ch)
        {
            return (char *)(str+len);
        }
    }
    return NULL;
}

char *strcat(char *dist, char *source)
{
    ASSERT((dist != NULL) && (source != NULL));
    char *head = dist + strlen(dist);
    while(*source != '\0')
    {
        *head++ = *source++;
    }
    *head = '\0';
    return dist;
}

//在字符串中查找字符出现次数
uint32_t strchrs(const char *str, const uint8_t ch)
{
    ASSERT(str != NULL);
    uint32_t len = 0;
    while(*str != '\0')
    {
        if(*str == ch)
        {
            len ++;
        }
        str ++;
    }
    return len;
}
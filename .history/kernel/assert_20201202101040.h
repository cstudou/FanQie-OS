#ifndef __FANQIE_ASSERT_H__
#define __FANQIE_ASSERT_H__

//打印调试信息
void debug_message(const char *filename, int line, const char *func, const char * condition);

#define DEBUG(...) debug_message(__FILE__, __LINE__, __FUNC__, __VA_ARGS__)

#ifdef NDEBUG
    #define assert(ARG) 0
#else
    #define assert(ARG) if(!ARG) { DEBUG(#ARG); } //#让编译器将宏的参数转化为字符串字面量
#endif

#endif

#ifndef __SHELL_H
  #define __SHELL_H


#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*******************************************************
  登记命令函数
    参数 命令关键字、命令函数、命令帮助 
*******************************************************/  
//SHELL模块初始化
extern void shellInit(void);

typedef void(* CMD_FUNC)(char * p0, char * p1, char * p2, char * p3);

extern void shellRegCmd(const char * key, CMD_FUNC func, const char * help);

// 通过串口打印数据，字符串长度不能超过255个字节。
extern void shellPrint(const char *fmt, ...);

// 通过串口输出数据，数据长度不能超过255个字节。
extern void shellOut(const unsigned char *data, unsigned long len);


#if defined(__cplusplus)
}
#endif 

#endif



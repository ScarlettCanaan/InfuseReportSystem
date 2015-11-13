
#ifndef __RFLABEL_H
#define __RFLABEL_H

// 初始化 
extern void rfidInit(void);

// 每秒执行一次 
extern void rfidTick(void);

// 往后一页
extern void rfidPageNext(void); 

// 往前一页
extern void rfidPagePrev(void);

// 测试开始
extern void rfidTestStart(int com);

// 测试结束
extern void rfidTestStop(void);

// 下一页
extern void rfidNextPage(void);

// 前一页
extern void rfidPrevPage(void);

#endif






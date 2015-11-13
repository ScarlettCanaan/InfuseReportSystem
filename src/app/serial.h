#ifndef _DS_WIN_SERIAL_H
#define _DS_WIN_SERIAL_H

#define BAUDRATE_4800     1
#define BAUDRATE_9600     2
#define BAUDRATE_38400    3
#define BAUDRATE_57600    4
#define BAUDRATE_115200   5
#define BAUDRATE_230400   6

extern BOOL openCom(int com, int baudrate);
extern void closeCom(void);
extern void comThreadCreate(void);
extern void comRead(unsigned char * data, unsigned long len);
extern void comWrite(unsigned char * data, unsigned long len);

// 检查有哪几个串口可用
// 返回：32BIT，BIT0——31分别表示COM1——COM32是否存在。
//       BIT为0：该串口不存在，BIT为1：该串口存在      
extern unsigned long comCheck(void);

#endif

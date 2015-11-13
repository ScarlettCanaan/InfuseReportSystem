/*************************************************************************
 *
*    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2008
 *
 *    File name   : drv_glcd.h
 *    Description : Graphical LCD driver include file
 *
 *    History :
 *    1. Date        : 6, March 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *
 *    $Revision: 24636 $
 *
 *    @Modify: NXP MCU Application Team - NguyenCao
 *    @Date: 04. March. 2011
 **************************************************************************/
//#include "lpc_types.h"

#ifndef __LCD_H
#define __LCD_H

#define LCD_COLOR_BLACK        0x00000000  // 黑色
#define LCD_COLOR_WHITE        0x00FFFFFF  // 白色
#define LCD_COLOR_GRAY         0x007F7F7F  // 灰色（50%灰度）
#define LCD_COLOR_DARK_GRAY    0x003F3F3F  // 深灰色（75%灰度）
#define LCD_COLOR_LIGHT_GRAY   0x00C3C3C3  // 浅灰色（25%灰度）
#define LCD_COLOR_RED          0x000000FF  // 红色
#define LCD_COLOR_DARK_RED     0x0000007F  // 深红色（酒红色）
#define LCD_COLOR_LIGHT_RED    0x007F7FFF  // 浅红色（粉红色）
#define LCD_COLOR_GREEN        0x0000FF00  // 绿色
#define LCD_COLOR_DARK_GREEN   0x00007F00  // 深绿色
#define LCD_COLOR_LIGHT_GREEN  0x007FFF7F  // 浅绿色
#define LCD_COLOR_BLUE         0x00FF0000  // 蓝色
#define LCD_COLOR_DARK_BLUE    0x007F0000  // 深蓝色（海蓝色）
#define LCD_COLOR_LIGHT        0x00FF7F7F  // 浅蓝色（天蓝色）
#define LCD_COLOR_YELLOW       0x0000FFFF  // 黄色
#define LCD_COLOR_DART_YELLOW  0x00007F7F  // 深黄色（橄榄色）
#define LCD_COLOR_LIGHT_YELLOW 0x007FFFFF  // 浅黄色
#define LCD_COLOR_PURPLE       0x00FF00FF  // 紫色
#define LCD_COLOR_DART_PURPLE  0x007F007F  // 深紫色
#define LCD_COLOR_LIGHT_PURPLE 0x00FF7FFF  // 浅紫色
#define LCD_COLOR_CYAN         0x00FFFF00  // 青色
#define LCD_COLOR_DARK_CYAN    0x007F7F00  // 深青色
#define LCD_COLOR_LIGHT_CYAN   0x00FFFF7F  // 浅青色（天青色）
#define LCD_COLOR_ORANGE       0x00007FFF  // 橙色
#define LCD_COLOR_GOLD         0x0000D7FF  // 金色

// 对LCD进行初始化
// 入参 xSize,ySize: LCD屏幕的大小
extern void LCD_init(int xSize, int ySize);

// 设置显存的地址，地址空间为xSize*ySize*2字节（RGB565模式）
extern void LCD_displayMemorySet(unsigned short * addr);

// 只有在LCD显存设定以后，LCD控制器才能使用
extern void LCD_enable(void);

// LCD背光控制
// 入参 on：是否打开  1：打开  0：关闭
extern void LCD_backLightCtl(int on);

#endif

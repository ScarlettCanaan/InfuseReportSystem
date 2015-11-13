
#ifndef GUI_DEMO_MAIN_H
#define GUI_DEMO_MAIN_H


#define GUI_EVENT_TICK        0x0001   // GUI的TICK时钟事件
#define GUI_EVENT_TOUCH_ISR   0x0002   // 触摸屏中断事件

extern unsigned long g_eventFlagGui;

extern void guiDemoInit(void);

extern void eventHandleGui(void);

#define DEMO_TYPE_SIZE_43 0x01   // 4.3
#define DEMO_TYPE_SIZE_70 0x02   // 7.0

#define DEMO_TYPE   DEMO_TYPE_SIZE_43

#if (DEMO_TYPE == DEMO_TYPE_SIZE_43)

#define LCD_X_SIZE  480
#define LCD_Y_SIZE  272

#else

#define LCD_X_SIZE  800
#define LCD_Y_SIZE  480
				
#endif
#define DEMO_X_SIZE  480
#define DEMO_Y_SIZE  272	
#endif


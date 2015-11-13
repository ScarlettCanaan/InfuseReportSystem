/****************************************
*
*  dropDown demo
*  dropDown
*
*****************************************/
//#include <LPC177x_8x.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "DsFat.h"
#include "dsGUI.h"

extern void demoCheckBoxCreate(void);
extern void demoListBoxCreate(void);

extern int g_fontChinese16, g_fontEnglish24;

#define DEMO_MEM_TMP_SIZE  0x100000
unsigned char g_comNum;
unsigned char g_comIndex[32];

int g_hDropDownComSelect,g_hButtonTest, g_hButtonNextPage,g_hButtonPrevPage, g_hLableStatus, g_hLableCardNum;

#define PAGE_LINE_NUM  6

int g_hLableIndex[PAGE_LINE_NUM], g_hLableId[PAGE_LINE_NUM], g_hLablePow[PAGE_LINE_NUM], 
    g_hLableSigle[PAGE_LINE_NUM], g_hLableOnline[PAGE_LINE_NUM];

int g_testIsOn;

void guiPageCB(int ctlHandle, int msgType)
{
    int dropDownIndex;
    WM_STRING wmString;

    if (ctlHandle == g_hButtonTest)
    {
        if (msgType == WM_MSG_TYPE_RELEASE)
        {
            if (g_testIsOn)
            {
                g_testIsOn = 0;
    
                wmString.fontId = g_fontChinese16;
                wmString.color = WM_COLOR_BLACK;
                wmString.str = (const  char *)"启动测试";
                
                WM_ButtonSetText(g_hButtonTest, &wmString);
                
                rfidTestStop();
            }
            else
            {
                g_testIsOn = 1;
    
                wmString.fontId = g_fontChinese16;
                wmString.color = WM_COLOR_BLACK;
                wmString.str = (const  char *)"停止测试";
                
                WM_ButtonSetText(g_hButtonTest, &wmString);
                
                dropDownIndex = WM_DropDownGet(g_hDropDownComSelect);
                
                rfidTestStart(g_comIndex[dropDownIndex]);
            }
        }
        
        return;
    }
    
    if (ctlHandle == g_hButtonPrevPage)
    {
        if (msgType == WM_MSG_TYPE_RELEASE)
        {
            rfidPrevPage();
        }
        
        return;
    }
    
    if (ctlHandle == g_hButtonNextPage)
    {
        if (msgType == WM_MSG_TYPE_RELEASE)
        {
            rfidNextPage();
        }
        
        return;
    }
}

void pageCreate(void)
{								
    WM_STRING wmString;
    unsigned long comMask;
    int i, handle;
    char comStr[12];

    g_testIsOn = 0;

    comMask = comCheck();

    g_comNum = 0;
    
    for (i = 0; i < 32; i++)
    {
        if (comMask & (1ul << i))
        {
            g_comIndex[g_comNum] = i+1;
            g_comNum++;
        }
    }
    

    WM_PageNew(WM_COLOR_WHITE, 0, guiPageCB);
    
    wmString.fontId = g_fontChinese16;
    wmString.color = WM_COLOR_BLACK;
    wmString.str = (const  char *)"请选择串口：";
    WM_StringDisp(20, 12, &wmString);
    
    g_hDropDownComSelect = WM_DropDownAdd(120, 10, 80, 85, WM_COLOR_LIGHT_GRAY, WM_COLOR_BLACK, GUI_FONT_ENG_16);
    
    for (i = 0; i < g_comNum; i++)
    {
        sprintf(comStr, "COM%d", g_comIndex[i]);
        WM_DropDownItemAdd(g_hDropDownComSelect,(const  char *)comStr);
    }

    wmString.fontId = g_fontChinese16;
    wmString.color = WM_COLOR_BLACK;
    wmString.str = (const  char *)"启动测试";
    
    g_hButtonTest = WM_ButtonAdd(220, 10, 80, 20);
    WM_ButtonSetText(g_hButtonTest, &wmString);

    wmString.fontId = g_fontChinese16;
    wmString.color = WM_COLOR_BLACK;
    wmString.str = (const  char *)"当前报警个数：";
    WM_StringDisp(320, 12, &wmString);

    g_hLableCardNum = WM_LabelAdd(427, 10, 33, 20, WM_COLOR_LIGHT_GRAY, WM_COLOR_BLACK, g_fontChinese16, 0x0002);

    // 构画出表格的底框
	WM_ClearByColor(20, 40, 440, 180, WM_COLOR_GRAY);

    handle = WM_LabelAdd(25, 45, 53, 20, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    WM_LabelSet(handle, "序号");

    handle = WM_LabelAdd(80, 45, 103, 20, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    WM_LabelSet(handle, "报警床位");
   
    handle = WM_LabelAdd(185, 45, 83, 20, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    WM_LabelSet(handle, "是否欠压");
   
    handle = WM_LabelAdd(270, 45, 83, 20, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    WM_LabelSet(handle, "信号质量");

    handle = WM_LabelAdd(355, 45, 100, 20, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    WM_LabelSet(handle, "在线时间");

//int g_hLableIndex[PAGE_LINE_NUM], g_hLableId[PAGE_LINE_NUM], g_hLablePow[PAGE_LINE_NUM], 
//    g_hLableSigle[PAGE_LINE_NUM], g_hLableOnline[PAGE_LINE_NUM];

    g_hLableIndex[0] = WM_LabelAdd(25, 70, 53, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableIndex[1] = WM_LabelAdd(25, 95, 53, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableIndex[2] = WM_LabelAdd(25, 120, 53, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableIndex[3] = WM_LabelAdd(25, 145, 53, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableIndex[4] = WM_LabelAdd(25, 170, 53, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableIndex[5] = WM_LabelAdd(25, 195, 53, 20, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);

    g_hLableId[0] = WM_LabelAdd(80, 70, 103, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableId[1] = WM_LabelAdd(80, 95, 103, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableId[2] = WM_LabelAdd(80, 120, 103, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableId[3] = WM_LabelAdd(80, 145, 103, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableId[4] = WM_LabelAdd(80, 170, 103, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableId[5] = WM_LabelAdd(80, 195, 103, 20, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
   
    g_hLablePow[0] = WM_LabelAdd(185, 70, 83, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLablePow[1] = WM_LabelAdd(185, 95, 83, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLablePow[2] = WM_LabelAdd(185, 120, 83, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLablePow[3] = WM_LabelAdd(185, 145, 83, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLablePow[4] = WM_LabelAdd(185, 170, 83, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLablePow[5] = WM_LabelAdd(185, 195, 83, 20, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
   
    g_hLableSigle[0] = WM_LabelAdd(270, 70, 83, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableSigle[1] = WM_LabelAdd(270, 95, 83, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableSigle[2] = WM_LabelAdd(270, 120, 83, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableSigle[3] = WM_LabelAdd(270, 145, 83, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableSigle[4] = WM_LabelAdd(270, 170, 83, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableSigle[5] = WM_LabelAdd(270, 195, 83, 20, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
   
    g_hLableOnline[0] = WM_LabelAdd(355, 70, 100, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableOnline[1] = WM_LabelAdd(355, 95, 100, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableOnline[2] = WM_LabelAdd(355, 120, 100, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableOnline[3] = WM_LabelAdd(355, 145, 100, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableOnline[4] = WM_LabelAdd(355, 170, 100, 23, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    g_hLableOnline[5] = WM_LabelAdd(355, 195, 100, 20, WM_COLOR_WHITE, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
   
   
    wmString.fontId = g_fontChinese16;
    wmString.color = WM_COLOR_BLACK;
    wmString.str = (const  char *)"前一页";
    
    g_hButtonPrevPage = WM_ButtonAdd(310, 223, 60, 20);
    WM_ButtonSetText(g_hButtonPrevPage,&wmString);
    
    wmString.fontId = g_fontChinese16;
    wmString.color = WM_COLOR_BLACK;
    wmString.str = (const  char *)"后一页";
    
    g_hButtonNextPage = WM_ButtonAdd(400, 223, 60, 20);
    WM_ButtonSetText(g_hButtonNextPage,&wmString);

    //g_hLableStatus = WM_LabelAdd(0, 262, 480, 22, WM_COLOR_LIGHT_GRAY, WM_COLOR_BLACK, g_fontChinese16, 0x0001);
    g_hLableStatus = WM_LabelAdd(0, 250, 480, 22, WM_COLOR_LIGHT_GRAY, WM_COLOR_BLACK, g_fontChinese16, 0x0002);
    WM_LabelSet(g_hLableStatus, "北京师范大学珠海分校 www.wlwchina.com.cn");

    WM_PageActive();
}

void pageUpdataLine(int line, int index, unsigned long id, unsigned char lossPower, int single, int keepSecond)
{
    char labelStr[30];
    
    if (line >= PAGE_LINE_NUM)
    {
        return;
    }

    sprintf(labelStr, "%d", index+1);
    WM_LabelSet(g_hLableIndex[line], labelStr);

    sprintf(labelStr, "%d", id);
    WM_LabelSet(g_hLableId[line], labelStr);

    if (lossPower)
    {
        WM_LabelSet(g_hLablePow[line], "欠压");
    }
    else
    {
        WM_LabelSet(g_hLablePow[line], "正常");
    }

    sprintf(labelStr, "%d%%", single);
    WM_LabelSet(g_hLableSigle[line], labelStr);

    sprintf(labelStr, "%d秒", keepSecond);
    WM_LabelSet(g_hLableOnline[line], labelStr);
}

void pageClearLine(int line)
{
    if (line >= PAGE_LINE_NUM)
    {
        return;
    }

    WM_LabelSet(g_hLableIndex[line], "");
    WM_LabelSet(g_hLableId[line], "");
    WM_LabelSet(g_hLablePow[line], "");
    WM_LabelSet(g_hLableSigle[line], "");
    WM_LabelSet(g_hLableOnline[line], "");
}

void pageSetCardNum(int num)
{
    char labelStr[20];
    
    sprintf(labelStr, "%d", num);
    WM_LabelSet(g_hLableCardNum, labelStr);
}


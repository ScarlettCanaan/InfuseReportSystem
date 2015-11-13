
#include <windows.h>
#include "serial.h"
#include "shell.h"
#include "rfLabel.h"

extern void pageSetCardNum(int num);

typedef struct
{
    unsigned long id;
    unsigned char lossPower;
    unsigned long keepSecond;
    unsigned char msgCount[10];  // 最近10秒内接收到消息数量 
}T_LABLE;

#define TALBE_NUM    200
T_LABLE  g_label[TALBE_NUM];

#define LINE_PER_PAGE   6   // 每页6行 

int g_labelNum, g_pageNo;

int g_rfidTestOn;

void rfidRcvMsg(unsigned char msg[])
{
    unsigned long id;
    unsigned char lossPower;
    int i;

    if ((msg[0] ^ msg[1] ^ msg[2] ^ msg[3] ^ msg[4] ^ msg[5]) != msg[6])
    {
        shellPrint("check error ");
        return;
    }

    id = (unsigned long)msg[5] 
      | ((unsigned long)msg[4] << 8)
      | ((unsigned long)msg[3] << 16)
      | ((unsigned long)msg[2] << 24);

    lossPower = msg[1] & 0x80 ?  1 : 0;

    for (i = 0; i < g_labelNum; i++)
    {
        if (g_label[i].id == id)
        {
            g_label[i].lossPower = lossPower;
            g_label[i].msgCount[9]++;
            
            return;
        }
    }
    
    if (g_labelNum < TALBE_NUM)
    {
        g_label[g_labelNum].id = id;
        g_label[g_labelNum].lossPower = lossPower;
        
        for (i = 0;  i < 10; i++)
        {
            g_label[g_labelNum].msgCount[i] = 2;
        }
        
        g_label[g_labelNum].keepSecond = 0;

        g_labelNum++;
    }
}

void comRead(unsigned char * data, unsigned long len)
{
    static unsigned char msg[7];
    static unsigned char msgLen = 0;
    int i;
    
    for (i = 0; i < len; i++)
    {
        if (msgLen < 5)
        {
            if (data[i] != 0xFF)
            {
                msgLen = 0;
                shellPrint("NOT FF %d ", i);
                continue;
            }
            else
            {
                msgLen++;
            }
        }
        else
        {
            msg[msgLen-5] = data[i];
            msgLen++;
            
            if (msgLen >= 12)
            {
                rfidRcvMsg(msg);
                
                msgLen = 0;
            }
        }
    }
}

void rfidInit(void)
{
    g_labelNum = 0;
    g_pageNo = 0; 
}

void rfidUpdataLine(int index, int line)
{
    int i, single;
    
    single = g_label[index].msgCount[0];
    
    for (i = 1; i < 10; i++)
    {
        single += g_label[index].msgCount[i];
    }
    
    single = single*100/20;  // 正常情况下，10秒会收到超过20个数据包

    pageUpdataLine(line, index, g_label[index].id, g_label[index].lossPower, 
                   single > 100 ? 100:single, g_label[index].keepSecond);
}

// 刷新当前页面 
void rfidUpdataPage(void)
{
    int line, index;
    
    // 刷新当前页面 
    pageSetCardNum(g_labelNum);    

    index = g_pageNo * LINE_PER_PAGE;
    
    for (line = 0; line < LINE_PER_PAGE; line++)
    {
        if (index < g_labelNum)
        {
            rfidUpdataLine(index, line);
        }
        else
        {
            pageClearLine(line);
        }
        
        index++;
    }

}

// 每秒执行一次 
void rfidTick(void)
{
    int i, j;

    if (!g_rfidTestOn)
    {
        return;
    }
    
    rfidUpdataPage(); 

    // 更新一下标签的状态    
    for (i = 0; i < g_labelNum; i++)
    {
        // 删除最早的一秒中消息包数量
        for (j = 0; j < 9; j++)
        {
            g_label[i].msgCount[j] = g_label[i].msgCount[j+1];
        }
        
        g_label[i].msgCount[9] = 0;
        
        for (j = 0; j < 9; j++)
        {
            if (g_label[i].msgCount[j])
            {
                break;
            }
        }
        
        if (j >= 9)
        {
            // 已经有9秒没有消息了，停止计数 
            g_label[i].keepSecond = 0;
        }
        else
        {
            g_label[i].keepSecond++;
        }
    }
}

void rfidTestStart(int com)
{
    int i;

    g_labelNum = 0;
    g_pageNo = 0;

    pageSetCardNum(0);    

    for (i = 0; i < LINE_PER_PAGE; i++)
    {
        pageClearLine(i);
    }
    
    openCom(com, BAUDRATE_9600);

    g_rfidTestOn = 1;
}

void rfidTestStop(void)
{
    g_rfidTestOn = 0;

    closeCom();
}

void rfidNextPage(void)
{
    if ((g_pageNo + 1) * LINE_PER_PAGE < g_labelNum)
    {
        g_pageNo++;
        
        rfidUpdataPage();
    }
}

void rfidPrevPage(void)
{
    if (g_pageNo)
    {
        g_pageNo--;

        rfidUpdataPage();
    }
}



//#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "serial.h"

HANDLE g_hWinCom = INVALID_HANDLE_VALUE;  //全局变量，串口句柄
HANDLE g_comMutex = NULL; //互斥

#define COM_READ_BUF_SIZE  1024

unsigned char g_comReadBuf[COM_READ_BUF_SIZE];
unsigned long g_comReadBufLen = 0;

BOOL openCom(int com, int baudrate)
{
    char comStr[20];
    
    unsigned long rate;
    //bool ret;
    
    if (g_hWinCom != INVALID_HANDLE_VALUE)
    {
        closeCom();
    }
    
    WaitForSingleObject(g_comMutex, INFINITE);
    
    sprintf(comStr, "\\\\.\\COM%d", com);
    
    g_hWinCom = CreateFile(comStr, GENERIC_READ|GENERIC_WRITE,
		            0,NULL,OPEN_EXISTING,
				    FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
				    NULL);

    if (g_hWinCom == INVALID_HANDLE_VALUE)
    {
        ReleaseMutex(g_comMutex);
        return FALSE;
    }
 
        // Set the event mask.
    //SetCommMask(g_hWinCom, EV_RXCHAR/* | EV_RX80FULL | EV_TXEMPTY*/);

    SetupComm(g_hWinCom, 2048, 2048);   //设置输入输出缓存区

    COMMTIMEOUTS TimeOuts;

    //设定读超时
    TimeOuts.ReadIntervalTimeout=1000;
    TimeOuts.ReadTotalTimeoutMultiplier=0;
    TimeOuts.ReadTotalTimeoutConstant=0;
    //在读一次输入缓冲区的内容后读操作就立即返回，
    //而不管是否读入了要求的字符。

    //设定写超时
    TimeOuts.WriteTotalTimeoutMultiplier=100;
    TimeOuts.WriteTotalTimeoutConstant=500;

    SetCommTimeouts(g_hWinCom, &TimeOuts); //设置超时
    
    switch(baudrate)
    {
        case BAUDRATE_4800:
            rate = 4800;
            break;

        case BAUDRATE_9600:
            rate = 9600;
            break;

        case BAUDRATE_38400:
            rate = 38400;
            break;

        case BAUDRATE_57600:
            rate = 57600;
            break;

        case BAUDRATE_115200:
            rate = 115200;
            break;

        case BAUDRATE_230400:
            rate = 230400;
            break;

        default:
            rate = 9600;
            break;
        
    }

    DCB dcb;

    GetCommState(g_hWinCom, &dcb);

    dcb.BaudRate=rate; //波特率为9600

    dcb.ByteSize=8; //每个字节有8位

    dcb.Parity=NOPARITY; //无奇偶校验位

    dcb.StopBits=ONESTOPBIT; //1个停止位

    SetCommState(g_hWinCom, &dcb);

    PurgeComm(g_hWinCom, PURGE_TXCLEAR|PURGE_RXCLEAR);
    
    g_comReadBufLen = 0;
    
    ReleaseMutex(g_comMutex);
    
    return TRUE;
}

void comWrite(unsigned char * data, unsigned long len)
{
        //异步写串口	
	COMSTAT ComStat;
	DWORD  dwErrorFlags;
	BOOL   bWriteStat;

    OVERLAPPED osWrite; 
    memset(&osWrite,0,sizeof(OVERLAPPED)); 
    osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

	if(INVALID_HANDLE_VALUE == g_hWinCom)
	{
	    return;
	}
	
	WaitForSingleObject(g_comMutex, INFINITE);
	
	ClearCommError(g_hWinCom, &dwErrorFlags,&ComStat);	

    bWriteStat=WriteFile(g_hWinCom, data, len, &len, &osWrite);
    	
	if(!bWriteStat) 
    { 
        if(GetLastError()==ERROR_IO_PENDING) 
        { 
            WaitForSingleObject(osWrite.hEvent, 1000); 
            //GetOverlappedResult(g_hCom, &osWrite, &dwErrorFlags, TRUE );
        } 
    }
	
	ReleaseMutex(g_comMutex);
    //PurgeComm(g_hCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);	
}

void closeCom()
{
    if (g_hWinCom == INVALID_HANDLE_VALUE)
    {
        return;
    }

    WaitForSingleObject(g_comMutex, INFINITE);

    CloseHandle(g_hWinCom);
 
    g_hWinCom = INVALID_HANDLE_VALUE;

    ReleaseMutex(g_comMutex);
}

DWORD WINAPI ThreadWinCom(LPVOID pram)
{
    OVERLAPPED os; 

    //DWORD dwMask,dwTrans, dwError;

    //COMSTAT comStat;

    BOOL bReadStat;

    HANDLE event;
	COMSTAT ComStat;
	DWORD  dwErrorFlags;

    unsigned long len, rdLen;

    event = CreateEvent(NULL,TRUE,FALSE,NULL);

    memset(&os,0,sizeof(OVERLAPPED));    
    os.hEvent= event;

    g_comMutex = CreateMutex(NULL,FALSE,NULL); 
    
    g_comReadBufLen = 0;

    while(1)
    {
        if (g_hWinCom == INVALID_HANDLE_VALUE)
        {
            Sleep(1);
            continue;
        }

        //memset(&os,0,sizeof(OVERLAPPED));    
        //os.hEvent= event;
        WaitForSingleObject(g_comMutex, INFINITE);
        rdLen = 0;
        
        //len = 1024;

        ClearCommError(g_hWinCom, &dwErrorFlags, &ComStat);
        
        if (!ComStat.cbInQue)
        {
           ReleaseMutex(g_comMutex);
           Sleep(1);
           continue;
        }

        len = (ComStat.cbInQue >= COM_READ_BUF_SIZE) ? COM_READ_BUF_SIZE : ComStat.cbInQue;
/*        
        if(!WaitCommEvent(g_hWinCom,&rdLen,&os))
        {
            if(GetLastError() != ERROR_IO_PENDING )
            {
                //ReleaseMutex(g_comMutex);
                ClearCommError(g_hWinCom, &dwErrorFlags, &ComStat);
                Sleep(1);
                continue;
            }
        }
        
        ClearCommError(g_hWinCom, &dwErrorFlags, &ComStat);
*/
        bReadStat = ReadFile(g_hWinCom, g_comReadBuf, len, &rdLen, &os);
        
        
        if (!bReadStat)
        {
            if(GetLastError() != ERROR_IO_PENDING )
            {
                //ReleaseMutex(g_comMutex);
                ReleaseMutex(g_comMutex);
                //shellPrint("[E] ");
                Sleep(1);
                continue;
            }
            
            //WaitForSingleObject(event, INFINITE);
            
            //PurgeComm(g_hWinCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
            //PurgeComm(g_hWinCom, PURGE_RXCLEAR);
             
            GetOverlappedResult(g_hWinCom,&os, &rdLen, TRUE);
            //GetOverlappedResult(g_hCom, &os, &dwTrans, TRUE );
        }
        else
        {
             //;
        }
        
        //
        //PurgeComm(g_hWinCom, PURGE_RXCLEAR);
        //PurgeComm(g_hWinCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

        //g_comReadBufLen += rdLen;
        
        if (rdLen)
        {
            //WDM_read(g_comReadBuf, g_comReadBufLen);
            comRead(g_comReadBuf, rdLen); 
            g_comReadBufLen = 0;
        }
        
        ReleaseMutex(g_comMutex);

    }
}

void comThreadCreate(void)
{
    CreateThread(NULL, 0, ThreadWinCom, 0, 0, 0);
}

// 检查有哪几个串口可用
// 返回：32BIT，BIT0——31分别表示COM1——COM32是否存在。
//       BIT为0：该串口不存在，BIT为1：该串口存在      
unsigned long comCheck(void)
{
    unsigned long mask;
    int i;
    HANDLE handle;
    char comStr[12];
	
    mask = 0;
	
    for (i = 0; i < 32; i++)
    {
        sprintf(comStr, "\\\\.\\COM%d", i+1);

        handle = CreateFile(comStr, 0, 0,NULL, OPEN_EXISTING, 0, NULL);
				    
        if (handle != INVALID_HANDLE_VALUE)
        {
            mask |= (1ul << i);
            
            CloseHandle(handle);
        }
    }
	
    return mask;
}





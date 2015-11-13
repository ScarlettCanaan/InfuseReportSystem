
#include <windows.h>
#include <stdio.h>

#include "shell.h"


extern void telnetInit(unsigned short port);
extern void telnetSend(const char * buf, int len);

extern int vsprintf(char * /*s*/, const char * /*format*/, va_list /*arg*/);

#define KEY_BACK_CTL   0x08
#define KEY_TAB        0x09
#define KEY_BACK_CHAR  0x7F

#define MAX_USERNAME_LEN 15
#define MAX_PASSWORD_LEN 15

#define SHELL_STATE_CHECK_USERNAME     1
#define SHELL_STATE_CHECK_PASSWORD     2
#define SHELL_STATE_ACTIVE             3

char g_shellUserName[MAX_USERNAME_LEN+1];
char g_shellPassword[MAX_PASSWORD_LEN+1];

#define SHELL_COMMAND_MAX_LEN 128

char g_shellCommand[SHELL_COMMAND_MAX_LEN+1];
DWORD g_shellCommandLen = 0;  // 键盘输入的命令长度(可以用退格键删除)

typedef struct
{
    char * addr;
    char len;
}SHELL_WORD;
 
typedef struct
{
    SHELL_WORD word[5];
    char number; 
}SHELL_SENTENCE;

SHELL_SENTENCE sentence;
typedef struct
{
    const char * key;
    CMD_FUNC func;
    const char * help;
}TCmd;
  
#define SHELL_CMD_MAX 20
TCmd g_cmd[SHELL_CMD_MAX];

WORD g_cmdNum = 0;	
//LED闪烁任务

void shellSnd(const char * data, unsigned long len)
{
    telnetSend(data, len);
}
			
/*******************************************************
  登记命令函数 
*******************************************************/  
void shellRegCmd(const char * key, CMD_FUNC func, const char * help)
{
    TCmd cmd;
    
    if (g_cmdNum >= SHELL_CMD_MAX)
    {
        return;
    }
    
    cmd.key = key;
    cmd.func = func;
    cmd.help = help;
    
    g_cmd[g_cmdNum] = cmd;
    g_cmdNum++;
}


/*******************************************************
  分析用户输入的命令 
*******************************************************/  
void shellAnalyseCommand(char * command, short cmdLen)
{
    short i, begin;
    char state; // 0：表示正在查换单词头  1：正在查询单词尾
    char * key;
    TCmd *pCmd;
    
    sentence.number = 0;
    
    // 将整个句子分解为各个单词
    
    state = 0;
    for (i = 0; i < cmdLen; i++)
    {
        if (state)
        {
            // 正在查询单词尾
            if (command[i] == ' ')
            {
                // 发现空格，则单词已结束。
                if (sentence.number < 5)
                {
                    sentence.word[sentence.number].addr = command+begin;
                    sentence.word[sentence.number].len = i - begin;

                    sentence.number++;
                }              
                command[i] = '\0'; // 将空格更换为截止符，便于后面的字符串比较。                
                state = 0;
            }
        }
        else
        {
            // 查询单词的开始
            if (command[i] != ' ')
            {
                begin = i;                
                state = 1;
            }
        }
    }
    
    if (state)
    {
          // 最后一个单词               
         if (sentence.number < 5)
         {
             sentence.word[sentence.number].addr = command+begin;
             sentence.word[sentence.number].len = i - begin + 1;
             sentence.number++;
         }
         
         command[i+1] = '\0';
    }

    if (sentence.number == 0)
    {
        return;
    }

    key = sentence.word[0].addr;
	
  //响应命令功能  
    if (((key[0] == 'H') || (key[0] == 'h'))
     && ((key[1] == 'E') || (key[1] == 'e'))
     && ((key[2] == 'L') || (key[2] == 'l'))
     && ((key[3] == 'P') || (key[3] == 'p')))
    {
        shellSnd("\r\n---------- help ------------\r\n", 32);
        for (i = 0; i < g_cmdNum; i++)
        {
            pCmd = &(g_cmd[i]);

            shellSnd(pCmd->key, strlen(pCmd->key));
            shellSnd(" -- ", 4);
            shellSnd(pCmd->help, strlen(pCmd->help));
            shellSnd("\r\n", 2);            
        }
        return;
    }

    if (((key[0] == 'B') || (key[0] == 'b'))
     && ((key[1] == 'Y') || (key[1] == 'y'))
     && ((key[2] == 'E') || (key[2] == 'e')))
    {
//        telnetClose();														//c  b   x 
        return;
    }

    for (i = 0; i < g_cmdNum; i++)
    {
        pCmd = &(g_cmd[i]);
        if (strcmp(pCmd->key, key) == 0)
        {
            // 查到命令
            switch (sentence.number)
            {
                case 1:
                    pCmd->func(0, 0, 0, 0);
                    break;
                case 2:
                    pCmd->func(sentence.word[1].addr, 0, 0, 0);
                    break;
                case 3:
                    pCmd->func(sentence.word[1].addr, sentence.word[2].addr, 0, 0);
                    break;
                case 4:
                    pCmd->func(sentence.word[1].addr, sentence.word[2].addr, 
					           sentence.word[3].addr, 0); 
                    break;
                default:
                    pCmd->func(sentence.word[1].addr, sentence.word[2].addr,
                               sentence.word[3].addr, sentence.word[4].addr);
                    break;
            }
            break;
        }
    }								

    if (i == g_cmdNum)
    {
        shellSnd("\r\nCan not find this command!\r\n", 30);
    }
}		 

void shellRcv(const char * data, unsigned long len)
{
    DWORD i;
    char ch;

    for (i = 0; i < len; i++)
    {
        switch (*(data+i))
        {
        case '\r':
            // 执行命令
            //shellSnd("\r\n", 2);

            shellAnalyseCommand(g_shellCommand, g_shellCommandLen);
            g_shellCommandLen = 0;

            shellSnd("->", 2);
            break;

        case '\n':
            break;

        case KEY_BACK_CTL:
        case KEY_BACK_CHAR:
            if (g_shellCommandLen)
            {
                g_shellCommandLen--;
                g_shellCommand[g_shellCommandLen] = 0;

                ch = KEY_BACK_CTL;
                shellSnd(&ch, 1);
                ch = ' ';
                shellSnd(&ch, 1);
                ch = KEY_BACK_CTL;
                shellSnd(&ch, 1);
            }
            break;
        case KEY_TAB:
            if (g_shellCommandLen >= (SHELL_COMMAND_MAX_LEN-4))
            {
                return;
            }
        
            g_shellCommand[g_shellCommandLen] = ' ';
            g_shellCommandLen++;
            g_shellCommand[g_shellCommandLen] = ' ';
            g_shellCommandLen++;
            g_shellCommand[g_shellCommandLen] = ' ';
            g_shellCommandLen++;
            g_shellCommand[g_shellCommandLen] = ' ';
            g_shellCommandLen++;
            g_shellCommand[g_shellCommandLen] = '\0';

            shellSnd("    ", 4);
            break;
            
        default:
            if (g_shellCommandLen >= (SHELL_COMMAND_MAX_LEN-1))
            {
                return;
            }

            g_shellCommand[g_shellCommandLen] = data[i];
            g_shellCommandLen++;
            g_shellCommand[g_shellCommandLen] = '\0';

            // 进行回显
            //shellSnd(data+i, 1);
        }
    }
}

void shellInit(void)
{	
    telnetInit(2500);
    g_shellCommandLen = 0;
    g_cmdNum = 0;
    
    strcpy(g_shellUserName, "admin");
    strcpy(g_shellPassword, "admin");
    
    //shellRegCmd("paint", cmdLcdPait, "paint start...");
}

char g_uartPrintStr[256];

void shellPrint(const char *fmt, ...)
{
    va_list ap;
    int i;
	
    va_start(ap, fmt);
    vsprintf((char *)g_uartPrintStr, fmt, ap);
    
    va_end(ap);

    for (i = 0; i < 128; i++)
    {
        if (g_uartPrintStr[i] == '\0')
        {
             break;
        }
    }

    shellSnd(g_uartPrintStr , i);
}

//// 通过串口输出数据，数据长度不能超过255个字节。
//void shellOut(const char *data, unsigned long len)
//{
//    shellSnd(data, len);
//}
//
 




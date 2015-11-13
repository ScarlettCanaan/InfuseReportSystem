#include "WindowHead.h"
#include "shell.h"

#define LCD_X_SIZE  480
#define LCD_Y_SIZE  272

unsigned long g_lcdImage[LCD_Y_SIZE][LCD_X_SIZE];

extern HWND g_hWnd;

// 将更新区域内容显示到LCD显存上
void WM_Paint(unsigned char * pageBaseAddr, int x, int y, int xSize, int ySize)
{
    unsigned long i,j;
    unsigned long color;
    unsigned char * bmp;
    BITMAPINFO bmi;
    RECT rect;

    for(i = y; i < y + ySize; i++)
    {
        for (j = x; j < x + xSize; j++)
        {
            g_lcdImage[i][j] = *(((unsigned long*)pageBaseAddr) + i*480 + j);
        }
    }

    rect.left = x;
    rect.top = y;
    rect.right = x  + xSize;
    rect.bottom = y + ySize;

    InvalidateRect(g_hWnd, &rect, TRUE);   
}

void lcdPaint(HDC hDc)
{
    BITMAPINFO bmi;

    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmi.bmiHeader.biWidth = LCD_X_SIZE;
    bmi.bmiHeader.biHeight = -LCD_Y_SIZE;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    	
    SetDIBitsToDevice(hDc, 0, 0, LCD_X_SIZE,  LCD_Y_SIZE, 0, 0, 0, LCD_Y_SIZE, g_lcdImage, &bmi, DIB_RGB_COLORS);
}



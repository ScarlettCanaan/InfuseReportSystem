#include <windows.h>
#include "shell.h"
#include "lcd.h"
#include "rfLabel.h"

extern void eventHandleGui(void);
extern void guiDemoInit(void);
extern void LCD_init(int xSize, int ySize);
extern void LCD_show(void);
extern void lcdPaint(HDC g_hDc);

HWND g_hWnd;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char szClassName[ ] = "WindowsApp";

HINSTANCE g_instance;

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    g_instance = hThisInstance;

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "医疗输液液位自动报警系统",       /* Title Text */
           WS_SYSMENU, //WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           480 + 5,             /* The programs width */
           272 + 28,        /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nFunsterStil);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

unsigned char g_touchIsOn;
unsigned short g_touchX, g_touchY;
/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    short x, y;
	PAINTSTRUCT ps;
	HDC hdc;

    switch (message)                  /* handle the messages */
    {
		case WM_CREATE:
            g_hWnd = hwnd;
            
			SetTimer(hwnd, 1, 5, 0);
			SetTimer(hwnd, 2, 1000, 0);
			
            //shellInit(); 
            
            g_touchIsOn = 0;
            
            HBRUSH brush = (HBRUSH) GetStockObject(HOLLOW_BRUSH);
            SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)(HBRUSH)brush); 
            guiDemoInit();

			break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
		case WM_TIMER:
             switch (wParam)
             {
                 case 1:
                      eventHandleGui();                     
                      break;
                 case 2:
                      rfidTick();
                     break;
                 default:
                      break;
             }
            break;
        case WM_MOUSEMOVE:           
                g_touchX = (unsigned short)LOWORD(lParam);  // horizontal position of cursor
                g_touchY = (unsigned short)HIWORD(lParam);  // vertical position of cursor
                break;
        case WM_LBUTTONDOWN:
                g_touchIsOn = 1;
                break;

        case WM_LBUTTONUP:
        case WM_KILLFOCUS:
                g_touchIsOn = 0;
                break;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            lcdPaint(hdc);

            EndPaint(hwnd, &ps);
			break;
 
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

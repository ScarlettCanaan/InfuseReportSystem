
#include <Winsock2.h>

void * threadTelnet(void * pParam);

extern void shellRcv(const char * data, unsigned long len);

SOCKET g_telnetListnSock, g_telnetConnectSock;

// SOCKET初始化，主要为加载WINSOCKET库。 
void telnetInit(unsigned short port)
{
	WSADATA	wsaData;
    struct sockaddr_in addr;

    g_telnetListnSock = INVALID_SOCKET;
    g_telnetConnectSock = INVALID_SOCKET;

	int winSockVer = 0x0202;		//表示 Winsock version 2.2

	if(WSAStartup(winSockVer, &wsaData) != 0)
	{
		return;
	}

	if(LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
	{
		WSACleanup( );
		return;
	}

    g_telnetListnSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (g_telnetListnSock == INVALID_SOCKET)
    {
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(g_telnetListnSock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        closesocket(g_telnetListnSock);

        return;
    }

    // 同一时刻只能接收十个客户端的接入请求。 
    if (listen(g_telnetListnSock, 10) == SOCKET_ERROR)
    {
        closesocket(g_telnetListnSock);
        return;
    }
    
     // 创建TELNET线程 
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)threadTelnet, 0, 0, 0);
}

// SOCKET监听线程 
void * threadTelnet(void * pParam)
{
	SOCKET sock; 
    struct sockaddr_in addr;
    int optVal;
    int addrLen;
    char * buf;

    while (1)
    {
        addrLen = sizeof(addr);
        sock = accept(g_telnetListnSock, (struct sockaddr *)&addr, &addrLen);

        optVal = 1;
        setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&optVal, sizeof(optVal));

        optVal = 256*1024;  // set send buffer size 256K
        setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&optVal, sizeof(optVal) );

        optVal = 256*1024;  // set receive buffer size 256K
        setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&optVal, sizeof(optVal) );

        if (g_telnetConnectSock != INVALID_SOCKET)
        {
		    closesocket(g_telnetConnectSock);
	    }
	    
	    g_telnetConnectSock = sock;

        buf = (char *)malloc(2048);

        while (1)
        {
            // recv函数会把线程挂起 
            int len = recv(g_telnetConnectSock, buf, 2048, 0);

            if (len <= 0)
            {
                // TELNET连接中断 

    		    closesocket(g_telnetConnectSock);
    		    
    		    g_telnetConnectSock = INVALID_SOCKET;
    		    
    		    break;
			}

            shellRcv(buf, len);
	    }
	    
        free(buf);
    }
}

void telnetSend(const char * buf, int len)
{
    if (g_telnetConnectSock != INVALID_SOCKET)
    {
        send(g_telnetConnectSock, buf, len, 0);
    }
}









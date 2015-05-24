// TCPServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>   // Needed for _wtoi
#include "TCPServerSocket.h"
#include "conio.h"


int g_iNumberConnected;
int g_iMaxConnect;
int g_iNumberUser;
int _tmain(int argc, _TCHAR* argv[])
{
	// Khai bao bien
	WSADATA		wsaData = {0};
	int			iRet = 0, iRetReturn = 0;
	SOCKET		sockServer= INVALID_SOCKET;
	CHAR		szIpAddress[20] = {0};
	int			iPort = 1080;
	addrinfo	*result = NULL;
	SOCKADDR_IN serverAddr = {0};
	int*		pintClientSocket;
	SOCKADDR_IN clientAddr = {0};
	int			iAddr_size = sizeof(SOCKADDR);

	g_iNumberConnected = 0;
	if (!IsReadInfoServer())
	{
		return 0;
	}
	printf("Bai Tap Lon Mon: Cac he thong phan tan.");
	printf("\nServer version 1.0");
	printf("\n------------------------------------------");
	iRet = CreateServerSocket(&wsaData, &sockServer);
	if (iRet == 1)
	{
		printf("\nKhoi tao socket khong thanh cong!");
		iRetReturn = 1;
		goto RETURN;
	}
	// bind and listen 
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(iPort);
	serverAddr.sin_addr.s_addr = INADDR_ANY ;

	if (bind(sockServer, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
	{
		printf("\nCo loi!");
		iRetReturn = 1;
		goto RETURN;
	}
	if (listen(sockServer, g_iNumberConnected))
	{
		printf("\nCo loi khi lang nghe ket noi!");
		iRetReturn = 1;
		goto RETURN;
	}

	while(true)
	{
		printf("\nLang nghe ket noi:");
		pintClientSocket = (int*)malloc(sizeof(int));
		if((*pintClientSocket = accept( sockServer, (SOCKADDR*)&clientAddr, &iAddr_size))== INVALID_SOCKET )
		{
			printf("\nLoi nhan ket noi!");
			continue;
		}
		printf("\nNhan ket noi tu: %s",inet_ntoa(clientAddr.sin_addr));
		CreateThread(NULL, NULL, &ConnectFromClient, (void*)pintClientSocket, NULL, NULL);
	}


RETURN:
	return 0;
}


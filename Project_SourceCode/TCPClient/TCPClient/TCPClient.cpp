// TCPClient.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>   // Needed for _wtoi
#include "SocketConnect.h"
#include "conio.h"


int _tmain(int argc, _TCHAR* argv[])
{
	// Khai bao bien
	WSADATA		wsaData = {0};
	int			iRet = 0, iRetReturn = 0;
	SOCKET		sockClient = INVALID_SOCKET;
	CHAR		szIpAddress[20] = {0};
	//CHAR		*szSenbuffer;
	int			iPort = 0, iConnected = 0, iTypeMessage = 0;
	addrinfo	*result = NULL;
	SOCKADDR_IN serverAddr = {0};


	// Khoi tao socket
	printf("Bai Tap Lon Mon: Cac he thong phan tan.");
	printf("\nClient version 1.0");
	printf("\n===========================================================================");
	iRet = CreateSocket(&wsaData, &sockClient);
	if (iRet == 1)
	{
		printf("\nKhoi tao socket khong thanh cong!");
		iRetReturn = 1;
		goto RETURN;
	}
	printf("\nNhap dia chi Ip ban muon ket noi:\n-/");
	scanf("%12s", szIpAddress);
	/*printf("Dia chi IP: %s", szIpAddress);*/
	
	printf("\nNhap cong ket noi:\n-/");
	scanf_s("%d", &iPort);
	/*printf("\nCong ket noi: %d", iPort);*/

	// Connect len server
	serverAddr.sin_addr.s_addr = inet_addr(szIpAddress);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(iPort);

	printf("\n\nDang ket noi voi server: %s %d", szIpAddress, iPort);
	if (connect(sockClient , (struct sockaddr *)&serverAddr , sizeof(serverAddr)) < 0)
	{
		printf("\nKet noi len server that bai!");
		iRetReturn = 1;
		goto RETURN;
	}
	printf("\nKet noi thanh cong!\n");
	iConnected = 1;
	//Gui yeu cau len server.
	while (iConnected == 1)
	{
		iRet = SendMessageToServer(sockClient, &iConnected, &iTypeMessage);
		if (iRet == 0 && iTypeMessage == 0)
		{
			printf("\nLenh cua ban khong dung cu phap!");
		}
		if (iRet == 0 && iTypeMessage != 0 && iTypeMessage != HELP)
		{
			// Nhan phan hoi tu server
			RecvMessage(sockClient, &iConnected, &iTypeMessage);
		}
	}
	// Ghi Log

	// Dong ket noi toi server.

RETURN:
	_getch();
	return iRetReturn;
}


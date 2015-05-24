#include "stdafx.h"
#include "windows.h"
#include "SocketConnect.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////////
// Ham khoi tao socket
// Return:	0 khoi tao socket thanh cong
//			1 Khoi tao socket khong thanh cong
//////////////////////////////////////////////////////////////////////////
int CreateSocket(WSADATA *wsaData, SOCKET *client)
{
	int iRet = 0;
	iRet = WSAStartup(MAKEWORD(2, 2), wsaData);
	if (iRet != 0) {
		printf("WSAStartup failed: %d\n", iRet);
		return 1;
	}	

	*client = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (*client == INVALID_SOCKET)
	{
		return 1;
	}

	return 0;
}
int ConnectToServer(LPCSTR szIpAddress, LPCSTR szPort, int iPort)
{
	return 0;
}

int SendMessageToServer(SOCKET socketClient, int *iConnected, int *iTypeMessage)
{
	//Command strLenh = {0};
	char szBufferSend[1024] = {0};
	DWORD dwBuffLen = 0;

	*iTypeMessage = 0;
	if (*iConnected != 1)
	{
		return 1;
	}

	printf("\n===========================================================================");
	printf("\nLenh thao tac voi server: \n-/");
	fflush(stdin);
	ZeroMemory(szBufferSend, BUFFER_LEN);
	gets_s(szBufferSend);
	dwBuffLen = strlen(szBufferSend);
	if (dwBuffLen == 4 && (strcmp(szBufferSend, "quit") == 0))
	{
		*iConnected = 0;
		printf("Ngat ket noi toi server!");
		return 1;
	}

	//-----------------------------------------------------------
	// Xac dinh lenh nguoi dung yeu cau

	// lenh hien thi thu muc hien thoi
	if (strcmp(szBufferSend, "directory") == 0)
	{
		*iTypeMessage = CURRENT_DERECTORY;
	}
	// lenh in danh sach thu muc, file trong thu muc hien thoi
	if (strstr(szBufferSend, "listview") != 0)
	{
		*iTypeMessage = VIEW_LIST_FOLDER_FILE;
	}
	// lenh xoa file
	if (strstr(szBufferSend, "DeleteFile ") != 0)
	{
		*iTypeMessage = DELETE_FILE;
	}
	// lenh xoa thu muc
	if (strstr(szBufferSend, "DeleteFolder ") != 0)
	{
		*iTypeMessage = DELETE_FOLDER;
	}
	// lenh di chuyen file
	if (strstr(szBufferSend, "MoveFile ") != 0)
	{
		*iTypeMessage = MOVE_FILE;
	}
	// lenh di chuyen thu muc
	if (strstr(szBufferSend, "MoveFolder ") != 0)
	{
		*iTypeMessage = MOVE_FOLDER;
	}
	// lenh lay thoi gia he thong
	if (strstr(szBufferSend, "GetTime") != 0)
	{
		*iTypeMessage = GET_TIME;
	}
	// lenh lay File ve tu server
	if (strstr(szBufferSend, "GetFile ") != 0)
	{
		*iTypeMessage = GET_FILE;
	}

	// Dang Nhap
	if (strstr(szBufferSend, "DangNhap ") != 0)
	{
		*iTypeMessage = DANGNHAP;
	}
	if (dwBuffLen == 4 && (strcmp(szBufferSend, "help") == 0))
	{
		*iTypeMessage = HELP ;
		HuongDan();
		return 0;
	}
	
	// Lenh UpFile len server
	if (strstr(szBufferSend, "UpLoad ") != 0)
	{
		*iTypeMessage = UPFILE;
	}
	//-----------------------------------------------------------
	// Sen du lieu len server
	if (*iTypeMessage == 0)
	{
		return 0;
	}
	if (send(socketClient, szBufferSend, (int)dwBuffLen, 0) == SOCKET_ERROR)
	{
		printf("Khong the gui yeu cau len server!");
		return 1;
	}
	return 0;
}

void HuongDan()
{
	printf("\n Cac lenh thao tac voi server:");
	printf("\n===========================================================================");
	printf("\n|    Lenh thao tac                   ||              Y nghia              |");
	printf("\n===========================================================================");
	printf("\n| DangNhap \"UserName\" \"Password\"     ||- Dang nhap vao server             |");
	printf("\n--------------------------------------------------------------------------");
	printf("\n| directory                          ||- Nhan duong dan thu muc hien tai. |");
	printf("\n--------------------------------------------------------------------------");
	printf("\n| listview                           ||- Xem danh sach cac file va thu muc|");
	printf("\n|                                    ||  trong thu muc hien tai           |");
	printf("\n--------------------------------------------------------------------------");
	printf("\n| DeleteFile \"PathFile\"              ||- Xoa file voi duong dan PathFile  |");
	printf("\n--------------------------------------------------------------------------");
	printf("\n| DeleteFolder \"PathFolder\"          ||- Xoa file voi duong dan PathFolder|");
	printf("\n--------------------------------------------------------------------------");
	printf("\n| MoveFile \"a\" \"b\"                   ||- Chuyen file a sang file b        |");
	printf("\n--------------------------------------------------------------------------");
	printf("\n| MoveFolder \"a\" \"b\"                 ||- Chuyen thu muc a sang thu muc b  |");
	printf("\n--------------------------------------------------------------------------");
	printf("\n| GetTime \"Opt\"                      ||- Lay thoi gian server.            |");
	printf("\n|                                    || Opt = local => Lay thoi gian Local|");
	printf("\n|                                    || Opt = system => Lay thoi gian UTC |");
	printf("\n--------------------------------------------------------------------------");
	printf("\n| GetFile \"a\"                        ||- Lay file a tu server ve may      |");
	printf("\n--------------------------------------------------------------------------");
	printf("\n| UpLoad \"a\"                         ||- Upload file len server           |");
	printf("\n--------------------------------------------------------------------------");
	printf("\n| help                               ||- Xem huong dan su dung            |");
	printf("\n===========================================================================");

}
int RecvMessage(SOCKET socketClient, int *iConnected, int *iTypeMessage)
{
	CHAR szServerRep[BUFFER_LEN] = {0};
	int iServerRepSize = 0;
	if (*iConnected == 1 && *iTypeMessage == HELP)
	{
		return 1;
	}
	if (*iTypeMessage == GET_FILE)
	{
		if (IsGetFileFromSerVer(socketClient))
		{
			printf("\nServer phan hoi lenh Get file :\n Thanh cong! \n");
			return 0;
		}

		printf("\nServer phan hoi lenh Get file :\n Co Loi! \n");
		return 1;	
	}
	else if (*iTypeMessage == UPFILE)
	{
		if (IsUpFileToServer(socketClient))
		{
			printf("\nServer phan hoi lenh Up file :\n Thanh cong! \n");
			return 0;
		}
		printf("\nServer phan hoi lenh Up file :\n Co Loi! \n");
		return 1;	
	}
	else
	{
		iServerRepSize = recv(socketClient ,szServerRep , BUFFER_LEN , NULL);
		if (iServerRepSize == SOCKET_ERROR)
		{
			printf("Co loi khi nhan du lieu tu server!");
			return 1;
		}
		PrintMessage(szServerRep, *iTypeMessage);
	}
	return 0;
}

int PrintMessage(LPCSTR szBuff, int iTypeMessage)
{
	switch(iTypeMessage)
	{ 
		case CURRENT_DERECTORY:
			printf("\nServer phan hoi:\n %s \n", szBuff);
			break;
		case VIEW_LIST_FOLDER_FILE:
			printf("\nServer phan hoi danh sach folder va file :\n %s \n", szBuff);
			break;
		case DELETE_FILE:
			printf("\nServer phan hoi lenh xoa file :\n %s \n", szBuff);
			break;
		case DELETE_FOLDER:
			printf("\nServer phan hoi lenh xoa folder :\n %s \n", szBuff);
			break;
		case MOVE_FILE:
			printf("\nServer phan hoi lenh move file :\n %s \n", szBuff);
			break;
		case MOVE_FOLDER:	
			printf("\nServer phan hoi lenh move folder :\n %s \n", szBuff);
			break;
		case GET_TIME:
			printf("\nServer phan hoi lenh get time :\n %s \n", szBuff);
			break;
		case DANGNHAP:
			printf("\nServer phan hoi lenh Dang Nhap :\n %s \n", szBuff);
			break;
		default:
			break;
	}
	return 0;
}
BOOL IsGetFileFromSerVer(SOCKET socketclient)
{
	char szFileName[MAX_PATH] = {0};
	char szBuffer[BUFFER_LEN] = {0};
	HANDLE hFile = INVALID_HANDLE_VALUE;
	BOOL bFinal = FALSE;
	if (recv(socketclient, szBuffer, BUFFER_LEN, NULL) == SOCKET_ERROR) 
		return FALSE;
	if (strstr(szBuffer, "GetFileStart ") == 0)
		return FALSE;

	if (!IsGetFileNameFromBuff(szBuffer, szFileName))
		return FALSE;
	hFile = CreateFileA(szFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	do 
	{
		DWORD dwNumberByteToWrite = 0;
		DWORD dwWritten = 0;
		dwNumberByteToWrite = recv(socketclient, szBuffer, BUFFER_LEN, NULL);
		if (dwNumberByteToWrite == SOCKET_ERROR)
		{
			return FALSE;
		}
		
		if (strstr(szBuffer, "GetFileEnd ") != 0)
		{
			CloseHandle(hFile);
			return TRUE;
		}
		WriteFile(hFile, (LPVOID)szBuffer, dwNumberByteToWrite,&dwWritten, NULL);

	} while (1);
	return TRUE;
}

BOOL IsUpFileToServer(SOCKET socketclient)
{
	char szFileName[MAX_PATH] = {0};
	char szBuffer[BUFFER_LEN] = {0};
	HANDLE hFile = INVALID_HANDLE_VALUE;
	BOOL bFinal = FALSE;
	if (recv(socketclient, szBuffer, BUFFER_LEN, NULL) == SOCKET_ERROR) 
		return FALSE;
	if (strstr(szBuffer, "Accept! ") == 0)
		return FALSE;
	if (!IsGetFileNameFromBuff(szBuffer, szFileName))
	{
		return FALSE;
	}
	if (!IsGetFile(socketclient, szFileName))
	{
		return FALSE;
	}
	if (recv(socketclient, szBuffer, BUFFER_LEN, NULL) == SOCKET_ERROR) 
		return FALSE;
	if (strstr(szBuffer, "Success!") == 0)
		return FALSE;
	return TRUE;
}

BOOL IsGetFile(SOCKET s, char* szFilePath)
{
	DWORD dwFileSizeLow = 0, dwFileSizeHigh = 0;
	char szBuffer[BUFFER_LEN] = {0};
	HANDLE hFile = INVALID_HANDLE_VALUE;
	char szFileName[MAX_PATH] = {0};
	DWORD dwSizeEmpty = 0;
	DWORD dwNumberByteReadOK = 0;

	dwFileSizeLow = GetFileSize(szFilePath, &dwFileSizeHigh);
	if (dwFileSizeHigh != 0)
	{
		strcpy_s(szBuffer, 1024, "Kich thuoc file qua lon!");
		send(s, szBuffer, strlen(szBuffer), NULL);
		return FALSE;
	}

	//Bat dau qua trinh doc file va gui ve client.
	hFile = CreateFile(szFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	if (!IsGetFileNameFromPath(szFilePath, szFileName))
	{
		return FALSE;
	}
	sprintf_s(szBuffer, BUFFER_LEN, "GetFileStart \"%s\"", szFileName);
	if (send(s, szBuffer, strlen(szBuffer), NULL) == SOCKET_ERROR)
	{
		return FALSE;
	}
	dwSizeEmpty = BUFFER_LEN;
	do 
	{
		ZeroMemory(szBuffer, BUFFER_LEN);
		ReadFile(hFile, szBuffer, dwSizeEmpty, &dwNumberByteReadOK, NULL);
		if (send(s, szBuffer, strlen(szBuffer), NULL) == SOCKET_ERROR)
		{
			return FALSE;
		}
	} while (dwNumberByteReadOK == dwSizeEmpty);
	sprintf_s(szBuffer, BUFFER_LEN, "GetFileEnd \"%s\"", szFileName);
	if (send(s, szBuffer, strlen(szBuffer), NULL) == SOCKET_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL IsGetFileNameFromPath(char* szPath, char* szFileName)
{
	DWORD dwPathLen = strlen(szPath);
	DWORD dwChiSo = dwPathLen ;
	char cKyTu = 0x2f; // ky tu "/"
	char cTemp = szPath[dwChiSo - 1];
	char szTemFile[MAX_PATH] = {0};
	DWORD dwTemFileLen = 0;

	while(dwChiSo > 0 && cTemp != cKyTu)
	{
		szTemFile[dwPathLen - dwChiSo] = cTemp;
		dwChiSo--;
		cTemp = szPath[dwChiSo -1];
	}

	dwTemFileLen = strlen(szTemFile);
	for (DWORD i = dwTemFileLen; i > 0; i--)
	{
		szFileName[dwTemFileLen - i] = szTemFile[i - 1];
	}
	return TRUE;
}

int  CreateLog()
{
	return 0;
}

BOOL IsGetFileNameFromBuff(char* szBuffer, char* szFileName)
{
	DWORD i = 0;
	DWORD dwBufflen = 0;
	char cNhayKep = 0x22; //"\""
	dwBufflen = strlen(szBuffer);
	for (i = 0; i < dwBufflen; i++)
	{
		char cTemp = szBuffer[i];
		if (cTemp == cNhayKep)
		{
			DWORD j = i + 1;
			cTemp = szBuffer[j];
			while(j < dwBufflen && cTemp != cNhayKep)
			{
				szFileName[j-i-1] = cTemp;
				j++;
				cTemp = szBuffer[j];
			}
			if (j == dwBufflen)
			{
				return FALSE;
			}
			break;

		}
	}
	if (i == dwBufflen)
	{
		return FALSE;
	}
	return TRUE;
}
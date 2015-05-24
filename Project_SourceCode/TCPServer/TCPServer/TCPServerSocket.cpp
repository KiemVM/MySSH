#include "stdafx.h"
#include "windows.h"
#include "TCPServerSocket.h"
#include "ThucHienLenhTrenServer.h"
#include "iostream"

//Luu y: chua kiem soat loi ham send trong ham XuLyYeuCauClient

extern int g_iNumberConnected;
extern int g_iMaxConnect;
extern int g_iNumberUser;
UserInfo g_userLogin[MAX_USER];
BOOL g_bConnect[MAX_USER];
int CreateServerSocket(WSADATA *wsaData, SOCKET *server)
{
	int iRet = 0;

	iRet = WSAStartup(MAKEWORD(2, 2), wsaData);
	if (iRet != 0) {
		printf("WSAStartup failed: %d\n", iRet);
		return 1;
	}	

	*server = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (*server == INVALID_SOCKET)
	{
		return 1;
	}
	return 0;
}

DWORD WINAPI ConnectFromClient(void* lp)
{
	int *iClient = (int*)lp;
	char szBuffer[BUFFER_LEN] = {0};
	int iBufferLen = 1024;
	int iByteNumber = 0, iRet = 0, iDangNhap = 0;
	int iID = 0;
	int i = 0;
	
	if (g_iNumberConnected == g_iMaxConnect)
	{
		printf("\n So ket noi da dat nguong, khong tiep nhan them yeu cau!");
	}
	g_iNumberConnected ++;
	for (i = 0; i < g_iMaxConnect; i++)
	{
		if (g_bConnect[i] == FALSE)
		{
			g_bConnect[i] = TRUE;
			break;
		}
	}
	iID = i;
	printf("\nCap phat duong ket noi so %d cho Client", i);
	printf("\nSo ket noi hien tai: %d", g_iNumberConnected);
	while(1)
	{
		ZeroMemory(szBuffer, BUFFER_LEN);
		if((iByteNumber = recv(*iClient, szBuffer, iBufferLen, 0))==SOCKET_ERROR)
		{
			printf("\nCo loi trong qua trinh nhan du lieu!");
			break;
		}
		iRet = XuLyYeuCauClient(*iClient, szBuffer, &iDangNhap);
		if (iRet == 0)
		{
			break;
			
		}
	}
	g_iNumberConnected --;
	printf("\nKet thuc mot ket noi so: %d.\n Giai phong duong ket noi so: %d.\nSo ket noi con lai %d", iID, iID, g_iNumberConnected);
	free(iClient);
	return 0;
}
int XuLyYeuCauClient(SOCKET socketClient, CHAR *szYeuCau, int* iDangNhap)
{
	CHAR szBuffer[BUFFER_LEN] = {0};
	CHAR szFileName[MAX_PATH] = {0};
	DWORD dwRet = 0, dwLenYeuCau = 0;
	int iReturn = 0;
	
	if (g_iNumberConnected > g_iMaxConnect)
	{
		strcpy_s(szBuffer, BUFFER_LEN, "Server dang co nhieu nguoi truy cap, ban vui long truy cap lai sau!");
		iReturn = 0;
		goto RETURN_XYLY;
	}
	if (strstr(szYeuCau, "DangNhap") != 0)
	{
		int iRet = DangNhap(szYeuCau);
		if (iRet == 1)
		{
			*iDangNhap = 1;
			strcpy_s(szBuffer, BUFFER_LEN,"Dang Nhap thanh cong!");
			iReturn = 1;
			goto RETURN_XYLY;
		}
		else if (iRet == 2)
		{
			*iDangNhap = 1;
			strcpy_s(szBuffer, BUFFER_LEN,"Dang Nhap thanh cong! \nChu y: Co nguoi dang nhap tai khoan nay truoc ban!");
			iReturn = 1;
			goto RETURN_XYLY;
		}
		else
		{
			*iDangNhap = 0;
		}
		strcpy_s(szBuffer, BUFFER_LEN,"Dang Nhap khong thanh cong!");
		iReturn = 2;
		goto RETURN_XYLY;
	}
	if (*iDangNhap == 0)
	{
		strcpy_s(szBuffer, BUFFER_LEN, "Ban phai dang nhap: DangNhap \"UserName\" \"Pass\"");
		iReturn = 2;
		goto RETURN_XYLY;
	}
	//////////////////////////////////////////////////////////////////////////
	// lenh hien thi thu muc hien thoi
	if (strcmp(szYeuCau, "directory") == 0)
	{
		dwRet = GetCurrentDirectoryA(MAX_PATH, szBuffer);
		if (dwRet == 0)
		{
			strcpy_s(szBuffer, BUFFER_LEN,"ERROR From server!");
			iReturn = 2;
			goto RETURN_XYLY;
		}
		iReturn = 1;
		goto RETURN_XYLY;
	}

	//////////////////////////////////////////////////////////////////////////
	// lenh in danh sach thu muc, file trong thu muc hien thoi
	if (strstr(szYeuCau, "listview") != 0)
	{
		DuyetThuMucHienThoi(szBuffer,BUFFER_LEN, 0);
		iReturn = 1;
		goto RETURN_XYLY;
	}

	//////////////////////////////////////////////////////////////////////////
	// lenh xoa file
	if (strstr(szYeuCau, "DeleteFile ") != 0)
	{
		if(!IsGetFileNameFromBuff(szYeuCau, szFileName)) 
			goto LOI_XOA_FILE;
		if (IsXoaFile(szFileName))
		{
			strcpy_s(szBuffer, BUFFER_LEN, "Xoa file thanh cong!");
			iReturn = 1;
			goto RETURN_XYLY;
		}
LOI_XOA_FILE:
		strcpy_s(szBuffer, BUFFER_LEN, "Xoa file khong thanh cong!");
		iReturn = 2;
		goto RETURN_XYLY;
		
	}

	//////////////////////////////////////////////////////////////////////////
	// lenh xoa thu muc
	if (strstr(szYeuCau, "DeleteFolder ") != 0)
	{
		//Loc lay FileDir
		if (!IsGetFileNameFromBuff(szYeuCau, szFileName))
		{
			goto LOI_XOA_FOLDER;
		}
		if (IsDeleteDir(szFileName))
		{
			strcpy_s(szBuffer, BUFFER_LEN, "Xoa thu muc thanh cong!");
			iReturn = 1;
			goto RETURN_XYLY;
		}
LOI_XOA_FOLDER:
		strcpy_s(szBuffer, BUFFER_LEN, "Xoa thu muc khong thanh cong!");
		iReturn = 2;
		goto RETURN_XYLY;
	}

	//////////////////////////////////////////////////////////////////////////
	// lenh di chuyen file
	if (strstr(szYeuCau, "MoveFile ") != 0)
	{
		char* szDestFile = NULL;
		if (!IsGetFileNameFromBuff(szYeuCau, szFileName))
		{
			goto LOI_CHUYEN_FILE;
		}
		// Loc lay ten file can chuyen va noi chuyen toi
		szDestFile = new(std::nothrow) char[MAX_PATH];
		if (szDestFile == NULL)
		{
			goto LOI_CHUYEN_FILE;
		}
		ZeroMemory(szDestFile, MAX_PATH);
		if (!IsGetFileNameFromBuff(szYeuCau + strlen("MoveFile") + strlen(szFileName) + 4, szDestFile))
		{
			goto LOI_CHUYEN_FILE;
		}
		if (GetFileAttributes(szFileName) == FILE_ATTRIBUTE_DIRECTORY)
			goto LOI_CHUYEN_FILE;
		if (IsMoveFile(szFileName, szDestFile))
		{
			strcpy_s(szBuffer, BUFFER_LEN, "Chuyen file thanh cong!");
			iReturn = 1;
			delete[] szDestFile;
			goto RETURN_XYLY;
		}
LOI_CHUYEN_FILE:
		strcpy_s(szBuffer, BUFFER_LEN, "Chuyen file khong thanh cong!");
		iReturn = 2;
		if (szDestFile != NULL)
		{
			delete[] szDestFile;
		}
		goto RETURN_XYLY;
	}

	//////////////////////////////////////////////////////////////////////////
	// lenh di chuyen thu muc
	if (strstr(szYeuCau, "MoveFolder ") != 0)
	{
		char* szDestFile = NULL;
		if (!IsGetFileNameFromBuff(szYeuCau, szFileName))
		{
			goto LOI_CHUYEN_FOLDER;
		}
		// Loc lay ten file can chuyen va noi chuyen toi
		szDestFile = new(std::nothrow) char[MAX_PATH];
		if (szDestFile == NULL)
		{
			goto LOI_CHUYEN_FOLDER;
		}
		ZeroMemory(szDestFile, MAX_PATH);
		if (!IsGetFileNameFromBuff(szYeuCau + strlen("MoveFolder") + strlen(szFileName) + 4, szDestFile))
		{
			goto LOI_CHUYEN_FOLDER;
		}
		if (GetFileAttributes(szFileName) != FILE_ATTRIBUTE_DIRECTORY)
			goto LOI_CHUYEN_FOLDER;
		if (szDestFile == NULL)
		{
			goto LOI_CHUYEN_FOLDER;
		}
		if (IsMoveFile(szFileName, szDestFile))
		{
			strcpy_s(szBuffer, BUFFER_LEN, "Chuyen thu muc thanh cong!");
			iReturn = 1;
			delete[] szDestFile;
			goto RETURN_XYLY;
		}
LOI_CHUYEN_FOLDER:
		strcpy_s(szBuffer, BUFFER_LEN, "Chuyen thu muc khong thanh cong!");
		iReturn = 2;
		if (szDestFile != NULL)
		{
			delete[] szDestFile;
		}
		goto RETURN_XYLY;
	}

	//////////////////////////////////////////////////////////////////////////
	// lenh lay thoi gian he thong
	if (strstr(szYeuCau, "GetTime") != 0)
	{
		int iThamSo = 0;
		if (strlen(szYeuCau) == strlen("GetTime"))
		{
			iThamSo = LOCAL_TIME;
		}
		else
		{
			if (IsGetFileNameFromBuff(szYeuCau, szFileName)) // Filename is opt, this funtion to get opt, don't get filename :)
			{
				if (!strcmp(szFileName, "local"))
				{
					iThamSo = LOCAL_TIME;
				}
				else if (!strcmp(szFileName, "system"))
				{
					iThamSo = SYSTEM_TIME;
				}
			}
		}
		if (iThamSo == 0)
		{
			goto LOI_GET_TIME;
		}
		// Lay tham so truyen vao: local - system(UTC)
		if (IsGetTime(szBuffer, 1024, iThamSo))
		{
			iReturn = 1;
			goto RETURN_XYLY;
		}
LOI_GET_TIME:
		strcpy_s(szBuffer, BUFFER_LEN, "Lay thoi gian khong thanh cong!");
		iReturn = 2;
		goto RETURN_XYLY;
		
	}

	//////////////////////////////////////////////////////////////////////////
	// lenh lay File ve tu server
	if (strstr(szYeuCau, "GetFile ") != 0)
	{
		// Tach lay ten file muon nhan tu server
		if (!IsGetFileNameFromBuff(szYeuCau, szFileName))
		{
			strcpy_s(szBuffer, BUFFER_LEN, "Lay file khong thanh cong!");
			iReturn = 2;
			goto RETURN_XYLY;
		}
		if (IsGetFile(socketClient, szFileName))
		{
			return 1;
		}
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// Lenh Upload file len server
	if (strstr(szYeuCau, "UpLoad ") != 0)
	{
		// Tach lay ten file muon nhan tu server
		if (!IsGetFileNameFromBuff(szYeuCau, szFileName))
		{
			strcpy_s(szBuffer, BUFFER_LEN, "Up file khong thanh cong!");
			iReturn = 2;
			goto RETURN_XYLY;
		}
		if (IsGetFileFromClient(socketClient, szFileName))
		{
			return 1;
		}
		return 0;
	}
RETURN_XYLY:
	if(send(socketClient, szBuffer, strlen(szBuffer), NULL) == SOCKET_ERROR)
	{
		iReturn = 0;
	}
	return iReturn;
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
//////////////////////////////////////////////////////////////////////////
// 0: Khong hop le
// 1: User hop le, chua dang nhap
// 2: User hop le, dang dang nhap
//////////////////////////////////////////////////////////////////////////
int DangNhap(char* szBuffer)
{
	char szUser[MAX_PATH] = {0};
	char szPass[MAX_PATH] = {0};
	if ((strstr(szBuffer, "DangNhap ") == 0))
	{
		return FALSE;
	}
	if (!IsGetFileNameFromBuff(szBuffer, szUser))
	{
		return 0;
	}
	if (!IsGetFileNameFromBuff(szBuffer + strlen("DangNhap") + strlen(szUser) + 3, szPass))
	{
		return 0;
	}
	for (int i = 0; i < g_iNumberUser; i++)
	{
		if (strcmp(szPass, g_userLogin[i].szPassword) != 0)
		{
			continue;
		}
		if (strcmp(szUser, g_userLogin[i].szUserName) != 0)
		{
			continue;
		}
		if (g_userLogin[i].bLogin == FALSE)
		{
			g_userLogin[i].bLogin = TRUE;
			return 1;
		}
		else 
			return 2;
	}
	
	return 0;
}

BOOL IsReadInfoServer()
{
	// read config.ini
	char szFilePath[MAX_PATH];
	FILE *f;


	if (!GetModuleFileName(NULL, szFilePath, MAX_PATH)) return FALSE;
	strrchr(szFilePath, '\\')[1] = 0;

	strcat_s(szFilePath, MAX_PATH, "config.ini");

	f = fopen(szFilePath, "r");
	if (f <= 0)
	{
		printf("Cannot open config file!");
		return FALSE;
	}

	// Read Number Max connect
	fgets(szFilePath, MAX_PATH, f);
	while ( (strlen(szFilePath) > 0) &&
		((szFilePath[strlen(szFilePath) - 1] == 13) || 
		(szFilePath[strlen(szFilePath) - 1] == 10)))	
		szFilePath[strlen(szFilePath) - 1] = 0;
	g_iMaxConnect = atoi(szFilePath);
	if (g_iMaxConnect > 10)
	{
		return FALSE;
	}

	// Read number user;
	fgets(szFilePath, MAX_PATH, f);
	while ( (strlen(szFilePath) > 0) &&
		((szFilePath[strlen(szFilePath) - 1] == 13) || 
		(szFilePath[strlen(szFilePath) - 1] == 10)))	
		szFilePath[strlen(szFilePath) - 1] = 0;
	g_iNumberUser = atoi(szFilePath);
	if (g_iNumberUser > MAX_USER)
	{
		return FALSE;
	}

	for (int i = 0; i < g_iNumberUser; i++)
	{
		fgets(szFilePath, MAX_PATH, f);
		while ( (strlen(szFilePath) > 0) &&
			((szFilePath[strlen(szFilePath) - 1] == 13) || 
			(szFilePath[strlen(szFilePath) - 1] == 10)))	
			szFilePath[strlen(szFilePath) - 1] = 0;
		strcpy_s(g_userLogin[i].szUserName, 20, szFilePath);

		fgets(szFilePath, MAX_PATH, f);
		while ( (strlen(szFilePath) > 0) &&
			((szFilePath[strlen(szFilePath) - 1] == 13) || 
			(szFilePath[strlen(szFilePath) - 1] == 10)))	
			szFilePath[strlen(szFilePath) - 1] = 0;
		strcpy_s(g_userLogin[i].szPassword, 20, szFilePath);

		g_userLogin[i].bLogin = FALSE;
	}
	for (int i = 0; i < MAX_CONNECT; i++)
	{
		g_bConnect[i] = FALSE;
	}
	fclose(f);
	return TRUE;
	
}
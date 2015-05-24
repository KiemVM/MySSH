//  [3/15/2015 KiemVM]
#include "stdafx.h"
#include "windows.h"
#include "ThucHienLenhTrenServer.h"
#include "TCPServerSocket.h"
#include "iostream"

int DuyetThuMucHienThoi(char* szBuff,int iBuffLen, int iOpt)
{
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA FindFileData = {0};
	LARGE_INTEGER liFileSize = {0};
	char szFileLen[10] = {0};
	char szFileName[MAX_PATH] = {0};
	DWORD dwError = 0;
	char szDir[MAX_PATH] = {0};

	strcat_s(szDir, MAX_PATH, "*.*");
	hFindFile = FindFirstFile(szDir, &FindFileData);
	if (hFindFile == INVALID_HANDLE_VALUE)
	{
		strcpy_s(szBuff, iBuffLen, "Khong tim thay file nao!");
		return 1;
	}
	do 
	{
		if (!strcmp(FindFileData.cFileName, ".") || !strcmp(FindFileData.cFileName, ".."))
			continue;
		strcat_s(szBuff, iBuffLen,"\n");
		strcat_s(szBuff, iBuffLen,FindFileData.cFileName);
		if (iOpt == 1)
		{
			strcat_s(szBuff, iBuffLen,"\t");
			if (liFileSize.LowPart != 0)
			{
				sprintf_s(szFileLen, 10, "%d", liFileSize.HighPart );
			}
			sprintf_s(szFileLen, 10, "%d", liFileSize.LowPart );
			strcat_s(szBuff, iBuffLen, szFileLen);
			strcat_s(szBuff, iBuffLen, "\n");
		}
		

	} while (FindNextFile(hFindFile, &FindFileData));
	strcat_s(szBuff, iBuffLen, "\n");
	FindClose(hFindFile);
	return 0;
}

BOOL IsXoaFile(char* szFilePath)
{
	if (szFilePath == NULL)
	{
		return FALSE;
	}
	if (!DeleteFile(szFilePath))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL IsDeleteDir(char* szPath)
{
	HANDLE          hFile;                       // Handle to directory
	char			szFilePath[MAX_PATH] = {0};                 // Filepath
	char			szPattern[MAX_PATH] = {0};                  // Pattern
	WIN32_FIND_DATA FileInformation;             // File information

	strcpy_s(szPattern, MAX_PATH, szPath);
	strcat_s(szPattern, MAX_PATH, "\\*.*");
	hFile = FindFirstFile(szPattern, &FileInformation);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(FileInformation.cFileName[0] != '.')
			{
				ZeroMemory(szFilePath, MAX_PATH);
				strcpy_s(szFilePath, MAX_PATH, szPath);
				strcat_s(szFilePath, MAX_PATH, "\\");
				strcat_s(szFilePath, MAX_PATH, FileInformation.cFileName);
				if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
						// Delete subdirectory
					 if (!IsDeleteDir(szFilePath))
					 {
						 return FALSE;
					 }
				}
				else
				{
					// Set file attributes
					if(SetFileAttributes(szFilePath,
						FILE_ATTRIBUTE_NORMAL) == FALSE)
						return FALSE;

					// Delete file
					if(!DeleteFile(szFilePath))
						return FALSE;
				}
			}
		} while(FindNextFile(hFile, &FileInformation));

		// Close handle
		FindClose(hFile);
	}
	// Set directory attributes
	if(!SetFileAttributes(szPath,
		FILE_ATTRIBUTE_NORMAL))
		return FALSE;

	// Delete directory
	if(!RemoveDirectory(szPath))
		return FALSE;
		
	return TRUE;
}

BOOL IsDeleteAllFileInDir(char* szPath)
{

	return TRUE;
}

BOOL IsMoveFile(char* szSourceFile, char* szDestFile)
{
	if (!MoveFileEx(szSourceFile, szDestFile, MOVEFILE_COPY_ALLOWED))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL IsGetTime(char* szBuf, int iBufLen, int iOpt)
{
	SYSTEMTIME time = {0};

	if (iOpt == LOCAL_TIME)
	{
		GetLocalTime(&time);
		sprintf_s(szBuf, iBufLen, "Thoi gian Local yyyy//mm//dd : hh//mm//ss - %04d//%02d//%02d : %02d//%02d//%02d \n",
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	}
	else if (iOpt == SYSTEM_TIME)
	{
		GetSystemTime(&time);
		sprintf_s(szBuf, iBufLen, "Thoi gian System yyyy//mm//dd : hh//mm//ss - %04d//%02d//%02d : %02d//%02d//%02d \n",
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	}
	else
	{
		return FALSE;
	}
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

BOOL IsGetFileFromClient(SOCKET s, char* szFilePath)
{
	char szFileName[MAX_PATH] = {0};
	char szBuffer[BUFFER_LEN] = {0};
	HANDLE hFile = INVALID_HANDLE_VALUE;
	BOOL bFinal = FALSE;

	if (!IsGetFileNameFromPath(szFilePath, szFileName))
	{
		return FALSE;
	}
	sprintf_s(szBuffer, BUFFER_LEN, "Accept! \"%s\"", szFilePath);
	if (send(s, szBuffer, strlen(szBuffer), NULL) == SOCKET_ERROR)
	{
		return FALSE;
	}
	ZeroMemory(szBuffer, BUFFER_LEN);
	if (recv(s, szBuffer, BUFFER_LEN, NULL) == SOCKET_ERROR) 
		return FALSE;
	if (strstr(szBuffer, "GetFileStart ") == 0)
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
		dwNumberByteToWrite = recv(s, szBuffer, BUFFER_LEN, NULL);
		if (dwNumberByteToWrite == SOCKET_ERROR)
		{
			return FALSE;
		}

		if (strstr(szBuffer, "GetFileEnd ") != 0)
		{
			CloseHandle(hFile);
			ZeroMemory(szBuffer, BUFFER_LEN);
			sprintf_s(szBuffer, BUFFER_LEN, "Success!");
			if (send(s, szBuffer, strlen(szBuffer), NULL) == SOCKET_ERROR)
			{
				return FALSE;
			}
			return TRUE;
		}
		WriteFile(hFile, (LPVOID)szBuffer, dwNumberByteToWrite,&dwWritten, NULL);

	} while (1);
	return TRUE;
}

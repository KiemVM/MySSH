#include "windows.h"

#define		CURRENT_DERECTORY		1
#define		VIEW_LIST_FOLDER_FILE	2
#define		DELETE_FILE				3
#define		DELETE_FOLDER			4
#define		MOVE_FOLDER				5
#define		MOVE_FILE				6
#define		GET_TIME				7
#define		GET_FILE				8
#define		HELP					9
#define		DANGNHAP				10
#define		UPFILE					11

#define		BUFFER_LEN				1024

struct Command
{
	char szLenh[10];
	char szOpt1[3];
	char szOpt2[MAX_PATH];
	char szOpt3[MAX_PATH];
	int	 iNumberOpt;
};

int CreateSocket(WSADATA *wsaData, SOCKET *client);
int ConnectToServer(LPCSTR szIpAddress, LPCSTR szPort, int iPort);
int SendMessageToServer(SOCKET socketClient, int *iConnected, int *iTypeMessage);
int RecvMessage(SOCKET socketClient, int *iConnected, int *iTypeMessage);
void HuongDan();
int PrintMessage(LPCSTR szBuff, int iTypeMessage);
int  CreateLog();
BOOL IsGetFileFromSerVer(SOCKET socketclient);
BOOL IsGetFileNameFromBuff(char* szBuffer, char* szFileName);
BOOL IsUpFileToServer(SOCKET socketclient);
BOOL IsGetFile(SOCKET s, char* szFilePath);
BOOL IsGetFileNameFromPath(char* szPath, char* szFileName);

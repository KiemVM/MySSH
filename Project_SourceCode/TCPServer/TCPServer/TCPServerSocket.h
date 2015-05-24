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

#define		LOCAL_TIME				20
#define		SYSTEM_TIME				21

#define		BUFFER_LEN				1024

#define		USER_NAME				"Test"
#define		PASSWORD				"123456"

#define		MAX_USER				10
#define		MAX_CONNECT				10

struct UserInfo
{
	char szUserName[20];
	char szPassword[20];
	BOOL bLogin;
};

int CreateServerSocket(WSADATA *wsaData, SOCKET *server);
DWORD WINAPI ConnectFromClient(void* lp);
int XuLyYeuCauClient(SOCKET socketClient, CHAR *szYeuCau, int* iDangNhap);
void HuongDan();
int  CreateLog();
BOOL IsGetFileNameFromBuff(char* szBuffer, char* szFileName);
int DangNhap(char* szBuffer);
BOOL IsReadInfoServer();

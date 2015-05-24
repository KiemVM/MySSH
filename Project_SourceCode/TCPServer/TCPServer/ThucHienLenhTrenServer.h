
int DuyetThuMucHienThoi(char* szBuff, int iBuffLen, int iOpt);
BOOL IsXoaFile(char* szFilePath);
BOOL IsDeleteDir(char* szPath);
BOOL IsDeleteAllFileInDir(char* szPath);
BOOL IsMoveFile(char* szSourceFile, char* szDestFile);
BOOL IsGetTime(char* szBuf, int iBufLen, int iOpt);
BOOL IsGetFile(SOCKET s, char* szFilePath);
BOOL IsGetFileNameFromPath(char* szPath, char* szFileName);
BOOL IsGetFileFromClient(SOCKET s, char* szFilePath);

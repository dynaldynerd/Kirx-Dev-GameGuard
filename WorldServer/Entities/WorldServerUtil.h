#pragma once

#include "IdaCompat.h"

class CWnd;
class CMerchant;

unsigned int GetLoopTime();
int GetCurrentDay();
int GetCurDay();
bool GetDateTimeStr(char *szTime);
unsigned int GetKorLocalTime();
int GetCurrentHour();
int GetCurrentMin();
int GetCurrentSec();
int GetItemTableCode(const char *psItemCode);
int ParsingCommandA(char *pszSrc, int nMaxWordNum, char **ppszDst, int nMaxWordSize);
void WriteServerStartHistory(const char *format, ...);
void clear_file(const char *directory, int keepCount);
int MyMessageBox(const char *title, const char *format, ...);
int MyCrtDebugReportHook(int reportType, char *message, int *returnValue);
void ServerProgramExit(const char *source, int reason);
CMerchant *FindEmptyNPC(CMerchant *pNPC, int nMax);
void NetTrace(const char *fmt, ...);

extern CWnd *g_pFrame;

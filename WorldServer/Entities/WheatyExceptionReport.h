#pragma once

#include "IdaCompat.h"

#include <DbgHelp.h>
#if __has_include(<cvconst.h>)
#include <cvconst.h>
#else
enum BasicType
{
  btNoType = 0,
  btVoid = 1,
  btChar = 2,
  btWChar = 3,
  btInt = 6,
  btUInt = 7,
  btFloat = 8,
  btBCD = 9,
  btBool = 10,
  btLong = 13,
  btULong = 14,
  btCurrency = 25,
  btDate = 26,
  btVariant = 27,
  btComplex = 28,
  btBit = 29,
  btBSTR = 30,
  btHresult = 31,
};
#endif

class WheatyExceptionReport
{
public:
  WheatyExceptionReport();
  ~WheatyExceptionReport();

  void SetRunDialog(int bRun);
  void SetLogName(const char *pszLogName);
  void SetDescription(const char *pszDescription);
  void SetFtpConnection(
    const char *pszFtpIp,
    unsigned int nFtpPort,
    const char *pszFtpId,
    const char *pszFtpPwd,
    const char *pszFtpDirectory);

  static int __cdecl printf(const char *format, ...);
  static bool GetDisplayInfo(int nDeviceIndex, char *lpszDeviceInfo, char *lpszMonitorInfo);
  static BasicType GetBasicType(ULONG typeIndex, DWORD64 modBase);
  static char *FormatOutputValue(
    char *pszCurrBuffer,
    BasicType basicType,
    unsigned __int64 length,
    const void *pAddress);
  static char FormatSymbolValue(
    SYMBOL_INFO *pSym,
    STACKFRAME64 *sf,
    char *pszBuffer,
    unsigned int cbBuffer);
  static BOOL CALLBACK EnumerateSymbolsCallback(
    SYMBOL_INFO *pSymInfo,
    ULONG SymbolSize,
    void *UserContext);
  static char *DumpTypeIndex(
    char *pszCurrBuffer,
    DWORD64 modBase,
    ULONG dwTypeIndex,
    unsigned int nestingLevel,
    unsigned __int64 offset,
    bool *bHandled);
  static void WriteStackDetails(CONTEXT *pContext, bool bWriteVariables);
  static void GenerateExceptionReport(EXCEPTION_POINTERS *pExceptionInfo);
  static const char *GetExceptionString(DWORD dwCode);
  static const char *GetOsName(unsigned int dwPlatformId, unsigned int dwMajorVersion, unsigned int dwMinorVersion);
  static const char *GetOsVersion();
  static bool GetLogicalAddress(
    void *addr,
    char *szModule,
    DWORD len,
    unsigned int *section,
    unsigned int *offset);
  static long __stdcall WheatyUnhandledExceptionFilter(EXCEPTION_POINTERS *pExceptionInfo);

  static int m_bRunDialog;
  static char m_szFtpPwd[255];
  static char m_szLogFileName[260];
  static int m_bPDBExist;
  static HANDLE m_hReportFile;
  static char m_szFtpIp[20];
  static unsigned int m_nFtpPort;
  static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
  static char m_szDescription[10240];
  static char m_szFtpId[255];
  static HANDLE m_hProcess;
  static char m_szLogName[2048];
  static SYSTEMTIME m_st;
  static char m_szFtpDirectory[260];
};

extern WheatyExceptionReport g_WheatyExceptionReport;

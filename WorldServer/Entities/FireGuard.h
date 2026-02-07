#pragma once

#include "IdaCompat.h"

#include <windows.h>

#include "CCRFG_SEND_BUFFER.h"
#include "CLogFile.h"

struct _CCRFG_CALLBACK;
struct _CCRFG_RECV_BUFFER;

extern CLogFile g_FgLogFile;
extern _CCRFG_SEND_BUFFER g_FGSendData;
extern _CCRFG_RECV_BUFFER g_FGRecvData;
extern _CCRFG_CALLBACK g_pfnCallBack;

__int64 __fastcall AddEnvVariable(const char *lpName, char *lpValue, int nValueLen);
int __fastcall CcrFgCallback(
  int lCallbackCode,
  void *hUserContext,
  void *pCallbackParameter,
  int nParameterSize,
  void *pReservedParameter);

extern "C" int __fastcall _CcrFG_rs_Initialize(
  int (__fastcall *pfunc)(int, void *, void *, int, void *),
  unsigned __int8 *pszWorldName,
  unsigned int nWorldNameLen);

extern "C" void __fastcall _CcrFG_rs_Uninitialize();

extern "C" int __fastcall _CcrFG_rs_EncryptPacket(void *context, unsigned __int8 *buffer, int size);
extern "C" int __fastcall _CcrFG_rs_DecryptPacket(void *context, unsigned __int8 *buffer, int size);
extern "C" unsigned int __fastcall _CcrFG_rs_GetLastError();

extern "C" void *__fastcall _CcrFG_rs_CreateUserContext(
  unsigned int dwAccountSerial,
  unsigned __int8 *pszAccount,
  unsigned int nAccountLen,
  unsigned __int8 *pszCharName,
  unsigned int nCharNameLen,
  unsigned int dwIP,
  void *pWorldId);

extern "C" void __fastcall _CcrFG_rs_CloseUserContext(void **pContext);

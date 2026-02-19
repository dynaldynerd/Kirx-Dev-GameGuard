#pragma once

#include "IdaCompat.h"

class CNetProcess;
class CNetWorking;

struct _CCRFG_SEND_BUFFER
{
  CNetProcess *pTargetProc;
  CNetWorking *pNetwork;
  unsigned __int16 wMsgSize;
  char sSendBuffer[20000];
};

struct _CCRFG_RECV_BUFFER
{
  unsigned __int16 wMsgSize;
  char *pRecvBuffer;
};

struct _CCRFG_CALLBACK
{
  int (__fastcall *pfunc)(int, void *, void *, int, void *);
};

extern _CCRFG_SEND_BUFFER g_FGSendData;
extern _CCRFG_RECV_BUFFER g_FGRecvData;
extern _CCRFG_CALLBACK g_pfnCallBack;


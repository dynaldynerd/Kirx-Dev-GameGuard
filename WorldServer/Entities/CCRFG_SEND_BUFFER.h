#pragma once

#include "IdaCompat.h"

class CNetProcess;
class CNetWorking;

struct __declspec(align(8)) _CCRFG_SEND_BUFFER
{
  CNetProcess *pTargetProc;
  CNetWorking *pNetwork;
  unsigned __int16 wMsgSize;
  char sSendBuffer[20000];
};

extern _CCRFG_SEND_BUFFER g_FGSendData;

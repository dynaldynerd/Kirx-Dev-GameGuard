#pragma once

#include "IdaCompat.h"

struct _MSG_HEADER;

class __cppobj INationGameGuardSystem
{
public:
  virtual ~INationGameGuardSystem() = default;
  virtual void OnConnectSession(int n) = 0;
  virtual bool OnCheckSession_FirstVerify(int n) = 0;
  virtual void OnDisConnectSession(int n) = 0;
  virtual void OnLoopSession(int n) = 0;
  virtual void OnLoop() = 0;
  virtual bool IsActive() = 0;
  virtual bool IsInit() = 0;
  virtual bool RecvClientLine(int n, _MSG_HEADER *pMsgHeader, char *pMsg) = 0;
};

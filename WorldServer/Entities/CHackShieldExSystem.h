#pragma once

#include "IdaCompat.h"

#include "CMyTimer.h"
#include "INationGameGuardSystem.h"

class HACKSHEILD_PARAM_ANTICP;

class  CHackShieldExSystem : public INationGameGuardSystem
{
public:
  CHackShieldExSystem();
  ~CHackShieldExSystem() override;

  HACKSHEILD_PARAM_ANTICP *GetParam(unsigned int n);
  char Init_struct(bool bActive);
  bool IsActive() override;
  bool IsInit() override;
  bool OnCheckSession_FirstVerify(int n) override;
  void OnConnectSession(int n) override;
  void OnDisConnectSession(int n) override;
  void OnLoop() override;
  void OnLoopSession(int n) override;
  bool RecvClientLine(int n, _MSG_HEADER *pMsgHeader, char *pMsg) override;

  HACKSHEILD_PARAM_ANTICP **m_ppNodeArray;
  bool m_bInit;
  bool m_bActive;
  unsigned int m_dwCurrentCheckIndex;
  CMyTimer m_tmLoopTime;
};


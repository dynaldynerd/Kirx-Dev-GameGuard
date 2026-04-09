#pragma once

#include "IdaCompat.h"

#include "CMyTimer.h"
#include "INationGameGuardSystem.h"

class BASE_NPROTECT_PARAM;
class NPROTECT_PARAM_VER_30;

class CnProtectSystem : public INationGameGuardSystem
{
public:
  CnProtectSystem();
  ~CnProtectSystem() override;

  BASE_NPROTECT_PARAM *GetParam(unsigned int n);
  template <typename T>
  char Init(bool bActive, bool bFirstChecker);
  bool IsActive() override;
  bool IsInit() override;
  bool OnCheckSession_FirstVerify(int n) override;
  void OnConnectSession(int n) override;
  void OnDisConnectSession(int n) override;
  void OnLoop() override;
  void OnLoopSession(int n) override;
  bool RecvClientLine(int n, _MSG_HEADER *pMsgHeader, char *pMsg) override;

  BASE_NPROTECT_PARAM **m_ppNodeArray;
  bool m_bInit;
  bool m_bActive;
  CMyTimer m_tmLoopTime;
  unsigned int m_dwCurrentCheckIndex;
};

static_assert(sizeof(CnProtectSystem) == 0x38);

template <>
char CnProtectSystem::Init<NPROTECT_PARAM_VER_30>(bool bActive, bool bFirstChecker);

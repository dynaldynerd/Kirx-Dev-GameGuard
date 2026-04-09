#pragma once

#include "IdaCompat.h"

#include "CCSAuth3.h"

class CnProtectSystem;
struct _npr_msg_res_qry_clzo;

class BASE_NPROTECT_PARAM
{
public:
  virtual bool IsLogPass() = 0;
  virtual bool OnCheckSession_FirstVerify(int n) = 0;
  virtual void OnConnect(int n) = 0;
  virtual void OnLoop() = 0;
  virtual void OnDisConnect() = 0;
  virtual bool OnRecvSession(
    CnProtectSystem *mgr,
    int n,
    unsigned __int8 byProtocol,
    unsigned __int64 tSize,
    char *pMsg) = 0;
};

static_assert(sizeof(BASE_NPROTECT_PARAM) == 8);

class NPROTECT_PARAM_VER_30 : public BASE_NPROTECT_PARAM
{
public:
  NPROTECT_PARAM_VER_30();

  void Init();
  void Close();
  void CheckClient(bool bFirstChecker);
  void Kick(unsigned __int8 byReason, unsigned int dwRet);
  bool OnRecvSession_Request_Start(int nIndex);
  bool OnRecvSession_Response_Qry(unsigned __int64 tSize, _npr_msg_res_qry_clzo *pMsg);
  bool IsLogPass() override;
  bool OnCheckSession_FirstVerify(int n) override;
  void OnConnect(int nIndex) override;
  void OnLoop() override;
  void OnDisConnect() override;
  bool OnRecvSession(
    CnProtectSystem *mgr,
    int nIndex,
    unsigned __int8 byProtocol,
    unsigned __int64 tSize,
    char *pMsg) override;

  static bool ms_bFirstChecker;

  CCSAuth3 m_CSAuth;
  int m_nSocketIndex;
  unsigned int m_dwLastSyncQryTime;
  unsigned __int8 m_byVerifyState;
  unsigned __int8 m_byVerChangeWarnningCount;
};

static_assert(sizeof(NPROTECT_PARAM_VER_30) == 0x2018);

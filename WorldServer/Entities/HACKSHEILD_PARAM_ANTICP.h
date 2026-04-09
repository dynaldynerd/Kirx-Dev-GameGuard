#pragma once

#include "IdaCompat.h"

#include "AHNHS_TRANS_BUFFER.h"

class CHackShieldExSystem;
struct _hs_msg_res_qry_clzo;

class BASE_HACKSHEILD_PARAM
{
public:
  virtual bool IsLogPass() = 0;
  virtual bool OnCheckSession_FirstVerify(int n) = 0;
  virtual void OnConnect(int n) = 0;
  virtual void OnLoop() = 0;
  virtual void OnDisConnect() = 0;
  virtual bool OnRecvSession(
    CHackShieldExSystem *mgr,
    int n,
    unsigned __int8 byProtocol,
    unsigned __int64 tSize,
    char *pMsg) = 0;
};

static_assert(sizeof(BASE_HACKSHEILD_PARAM) == 8);

class HACKSHEILD_PARAM_ANTICPX_5381 : public BASE_HACKSHEILD_PARAM
{
public:
  HACKSHEILD_PARAM_ANTICPX_5381();

  void Init();
  void CheckClient(bool bFirstChecker);
  void Kick(unsigned __int8 byReason, unsigned int dwRet);
  bool OnRecvSession_First_MakeRequest(int nIndex);
  bool OnRecvSession_VerifyResponse(unsigned __int64 tSize, _hs_msg_res_qry_clzo *pMsg);
  bool IsLogPass() override;
  bool OnCheckSession_FirstVerify(int n) override;
  void OnConnect(int nIndex) override;
  void OnDisConnect() override;
  void OnLoop() override;
  bool OnRecvSession(
    CHackShieldExSystem *mgr,
    int nIndex,
    unsigned __int8 byProtocol,
    unsigned __int64 tSize,
    char *pMsg) override;

  static unsigned __int64 ms_hServer;

  int m_nSocketIndex;
  unsigned int m_dwLastSyncQryTime;
  unsigned __int8 m_byVerifyState;
  unsigned __int64 m_hClient;
  _AHNHS_TRANS_BUFFER m_stRequestBuf;
};

static_assert(sizeof(HACKSHEILD_PARAM_ANTICPX_5381) == 0x1B8);

unsigned __int64 AhnHS_CreateServerObject(const char *hsbPath);
void AhnHS_CloseServerHandle(unsigned __int64 hServer);
unsigned __int64 AhnHS_CreateClientObject(unsigned __int64 hServer);
void AhnHS_CloseClientHandle(unsigned __int64 hClient);
unsigned int AhnHS_MakeRequest(unsigned __int64 hClient, _AHNHS_TRANS_BUFFER *pRequestBuffer);
unsigned int AhnHS_VerifyResponseEx(
  unsigned __int64 hClient,
  const void *pResponse,
  unsigned int nLength,
  unsigned int *pErrorCode);


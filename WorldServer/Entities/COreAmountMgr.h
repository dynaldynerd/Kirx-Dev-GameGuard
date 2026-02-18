#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include <Windows.h>

/* 5395 */
class  COreAmountMgr
{
public:
  static COreAmountMgr *Instance();
  COreAmountMgr();
  ~COreAmountMgr();
  void Release();
  bool LoadINI();
  bool IsINIFileChanged();
  bool ReLoad();
  void InitRemainOreAmount(unsigned int dwRemain, unsigned int dwInit);
  void InitTransferOre(int nTransfer, int nInit);
  unsigned int GetRemainOre();
  unsigned int GetTotalOre();
  unsigned int GetOreTransferAmount();
  unsigned __int8 GetOreTransferCount();
  unsigned __int8 GetDepositRate();
  float *GetMultipleRate();
  void UpdateDepositeRate();
  void UpdateForce();
  void IncreaseOreAmount();
  void IncreaseOreCount();
  void DecreaseOre(unsigned int dwAlt);
  void SetOreTransferAmount();
  void InsertOreLog(unsigned __int8 byType);
  void Loop();
  void LoopSubProcSendInform();
  bool CheatOreAmount(unsigned int dwTot, unsigned int dwRemain);
  bool IsOreRemain();

  static COreAmountMgr *m_pInstance;

  int m_bChangeRemainRate;
  unsigned int m_dwTotalOreSet;
  unsigned int m_dwMinOreSet;
  float m_fMultipleRate[7][3];
  unsigned int m_dwTotalOreAmount;
  unsigned int m_dwRemainOreAmount;
  unsigned int m_uiProccessIndex;
  unsigned __int8 m_byDepositRate;
  _FILETIME m_ftWrite;
  unsigned int m_dwOreTransferAmount;
  unsigned __int8 m_byOreTransferCount;
  bool m_bCheckExhOreLog;
  CLogFile m_logOreAmount;
};

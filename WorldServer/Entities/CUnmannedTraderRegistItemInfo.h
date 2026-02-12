#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderItemState.h"

class CLogFile;
struct _TRADE_DB_BASE;

class __cppobj CUnmannedTraderRegistItemInfo
{
public:
  CUnmannedTraderRegistItemInfo();
  void Clear();
  bool Set(
    unsigned __int16 wInx,
    unsigned __int8 byInvenIndex,
    unsigned int uiInx,
    _TRADE_DB_BASE *kInfo,
    CLogFile *pkLogger);
  void RegistItem(
    unsigned int dwRegistSerial,
    unsigned __int16 wItemSerial,
    unsigned int dwETSerialNumber,
    unsigned int dwPrice,
    unsigned __int8 bySellTurm,
    unsigned __int8 byTableCode,
    unsigned __int16 wItemIndex,
    unsigned __int8 byStorageIndex,
    unsigned __int64 dwD,
    unsigned int dwU,
    bool bInserted);
  void RepriceItem(unsigned int dwPrice);
  void ReRegistItem(unsigned int dwPrice);
  void SellComplete(
    unsigned int dwPrice,
    unsigned int dwBuyerSerial,
    unsigned int dwTax,
    __int64 tResultTime,
    const char *wszBuyerName,
    const char *szBuyerAccount);
  unsigned __int8 SellWaitItem(unsigned __int16 wInx, CLogFile *pkLogger, __int64 tResultTime, unsigned __int8 *byStorageInx);
  void ClearBuyerInfo();
  void ClearRegist();
  void ClearToWaitState();
  bool IsEmpty();
  bool IsRegist();
  bool IsSellWait();
  bool IsSellUpdateWait();
  bool IsOverRegistTime();
  unsigned __int16 GetItemIndex();
  unsigned __int16 GetItemSerial();
  unsigned int GetBuyerSerial();
  unsigned __int8 GetTableCode();
  CUnmannedTraderItemState::STATE GetState();
  __int64 *GetStartTimePtr();
  __int64 GetStartTime();
  __int64 GetResultTime();
  __int64 GetLeftSec();
  unsigned __int8 GetSellTurm();
  unsigned int GetPrice();
  unsigned int GetTax();
  unsigned int GetETSerial();
  unsigned int GetU();
  unsigned __int64 GetD();
  unsigned __int8 GetStorageIndex();
  unsigned int GetRegistSerial();
  bool SetState(unsigned __int8 byState);
  void SetOverRegistTime();
  bool IsWaitNoitfyClose();

  unsigned int m_dwRegistSerial;
  unsigned short m_wItemSerial;
  unsigned int m_dwETSerialNumber;
  unsigned int m_dwPrice;
  long long m_tStartTime;
  unsigned char m_bySellTurm;
  unsigned int m_dwBuyerSerial;
  unsigned int m_dwTax;
  long long m_tResultTime;
  char m_wszBuyerName[17];
  char m_szBuyerAccount[13];
  unsigned char m_byTableCode;
  unsigned short m_wItemIndex;
  unsigned char m_byStorageIndex;
  unsigned long long m_dwD;
  unsigned int m_dwU;
  CUnmannedTraderItemState m_kState;
};

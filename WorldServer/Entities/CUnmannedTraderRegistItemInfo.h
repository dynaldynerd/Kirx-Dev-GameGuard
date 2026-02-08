#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderItemState.h"

class __cppobj CUnmannedTraderRegistItemInfo
{
public:
  CUnmannedTraderRegistItemInfo();
  void Clear();
  bool IsRegist();
  unsigned __int16 GetItemIndex();
  unsigned __int8 GetTableCode();
  __int64 *GetStartTimePtr();
  __int64 GetStartTime();
  unsigned __int8 GetSellTurm();
  unsigned int GetPrice();
  unsigned int GetETSerial();
  unsigned int GetU();
  unsigned __int64 GetD();
  unsigned __int8 GetStorageIndex();
  unsigned int GetRegistSerial();

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

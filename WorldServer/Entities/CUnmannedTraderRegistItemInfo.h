#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderItemState.h"

class __cppobj CUnmannedTraderRegistItemInfo
{
public:
  CUnmannedTraderRegistItemInfo();
  void Clear();

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

#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _LTD_ITEMINFO
{
  struct _iteminfo
  {
    unsigned int m_dwItemSerial;
    char m_szItemCode[10];
    char m_szUpgradeCode[10];
    char m_szItemName[64];
    unsigned int m_dwDur;
    unsigned __int8 m_byOverlapNum;
  };

  bool m_bExist;
  _SYSTEMTIME m_timeLocal;
  unsigned __int8 m_bySubLogType;
  unsigned __int8 m_byCnt;
  _iteminfo m_ItemInfo[64];
};
#pragma pack(pop)

struct _LTD_EXPEND
{
  bool m_bExist;
  _SYSTEMTIME m_timeLocal;
  unsigned __int8 m_bySubLogType;
  char m_wszEtcInfo[128];
};

#pragma pack(push, 1)
struct _LTD
{
  _SYSTEMTIME m_timeLocal;
  unsigned int m_dwLogSerial;
  unsigned __int8 m_byMainLogType;
  unsigned __int8 m_bySubLogType;
  unsigned int m_dwAccountSerial;
  unsigned int m_dwCharSerial;
  char m_szAccount[13];
  char m_wszAvatorName[17];
  _LTD_ITEMINFO m_ItemInfo;
  _LTD_EXPEND m_Expend;
};
#pragma pack(pop)

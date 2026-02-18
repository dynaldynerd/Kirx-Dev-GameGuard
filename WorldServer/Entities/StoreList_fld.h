#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct sell_info
{
  char m_strItemCode[64];
  int m_nMaxCount;
};

struct  _StoreList_fld : _base_fld
{
  char m_strBinding_DummyName[64];
  char m_strStore_NPCcode[64];
  char m_strStore_NPCname[64];
  char m_strStore_MAPcode[64];
  int m_nStore_trade;
  int m_bSet_NPCangle;
  int m_nStore_NPCangle;
  int m_nNpc_Class[10];
  int m_nStore_LISTcount;
  int m_nLimit_Listcount;
  int m_nLimitItem_InitTime;
  int m_nPriceSet;
  int m_nItemUpCode;
  char m_strItemlist[200][64];
  sell_info m_sellLimitList[16];
};
#pragma pack(pop)

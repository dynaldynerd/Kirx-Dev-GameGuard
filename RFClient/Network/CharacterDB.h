#pragma once

#include <windows.h>

#include "Protocol.h"

#define max_id_len 12
#define patron_id_len 50

#pragma pack(push, 1)

struct _ENTER_WORLD_TEMP_DATA_LEGACY
{
  DWORD m_dwWorldIP;
  WORD m_wWorldPort;
  char m_szID[max_id_len + 1];
  DWORD m_dwAccountSerial;
  DWORD m_dwWorldMasterKey[KEY_NUM];
};

struct _ENTER_WORLD_TEMP_DATA
{
  DWORD m_dwWorldIP;
  WORD m_wWorldPort;
  char m_szID[max_id_len + 1];
  DWORD m_dwAccountSerial;
  DWORD m_dwWorldMasterKey[KEY_NUM];
  WORD m_wZeroFieldValue;
  DWORD m_dwBillingAccountSerial;
  DWORD m_dwBillingType;
  DWORD m_dwIsAdult;
  WORD m_wLanguageIndex;
};

struct _ENTER_WORLD_TEMP_DATA_PATRON
{
  DWORD m_dwWorldIP;
  WORD m_wWorldPort;
  char m_szID[patron_id_len + 1];
  DWORD m_dwAccountSerial;
  DWORD m_dwWorldMasterKey[KEY_NUM];
  WORD m_wZeroFieldValue;
  DWORD m_dwBillingAccountSerial;
  DWORD m_dwBillingType;
  DWORD m_dwIsAdult;
  WORD m_wLanguageIndex;
};

#pragma pack(pop)

static_assert(sizeof(_ENTER_WORLD_TEMP_DATA_LEGACY) == 39, "_ENTER_WORLD_TEMP_DATA_LEGACY packing mismatch");
static_assert(sizeof(_ENTER_WORLD_TEMP_DATA) == 55, "_ENTER_WORLD_TEMP_DATA packing mismatch");
static_assert(sizeof(_ENTER_WORLD_TEMP_DATA_PATRON) == 93, "_ENTER_WORLD_TEMP_DATA_PATRON packing mismatch");

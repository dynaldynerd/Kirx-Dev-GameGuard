#pragma once

#include <windows.h>

#include "CharacterDB.h"
#include "Protocol.h"

#pragma pack(push, 1)

struct _enter_world_request_zone
{
  DWORD dwAccountSerial;
  DWORD dwMasterKey[CHECK_KEY_NUM];
  bool bFullMode;
  DWORD dwProtocolVer;
  char szClientVerCheckKey[33];

  int size() const
  {
    return sizeof(*this);
  }
};

struct _enter_world_result_zone
{
  BYTE byResult;
  BYTE byUserGrade;
  BYTE bySvrType;

  int size() const
  {
    return sizeof(*this);
  }
};

#pragma pack(pop)

#pragma once

#include <cstring>

#include <windows.h>

#include "DqsDbStructs.h"
#include "REGED_AVATOR_DB.h"

#pragma pack(push, 1)
struct _alive_char_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned int dwSerial;
  _REGED_AVATOR_DB AliveChar;
  _alive_char_result_zocl();
};

struct _not_arranged_char_inform_zocl
{
  unsigned __int8 byCharNum;
  _NOT_ARRANGED_AVATOR_DB CharList[50];
  _not_arranged_char_inform_zocl();
  int size();
};

struct _reged_char_result_zone
{
  unsigned __int8 byRetCode;
  unsigned __int8 byCharNum;
  int iLockType[3];
  _REGED_AVATOR_DB RegedList[3];

  _reged_char_result_zone();
  int size();
};

struct _add_char_result_zone
{
  unsigned __int8 byRetCode;
  unsigned __int8 byAddSlotIndex;

  int size() const;
};

struct _del_char_result_zone
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotIndex;

  int size() const;
};

struct _sel_char_result_zone
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotIndex;
  unsigned int dwWorldSerial;
  unsigned int dwDalant;
  unsigned int dwGold;

  int size() const;
};

struct _uilock_inform_request_zocl
{
  char byInformType;
  char byFailCount;
  char byHintIndex;
  char byFindPassFailCount;
};
#pragma pack(pop)

inline _alive_char_result_zocl::_alive_char_result_zocl()
{
}

inline _not_arranged_char_inform_zocl::_not_arranged_char_inform_zocl() : byCharNum(0)
{
}

inline int _not_arranged_char_inform_zocl::size()
{
  if (byCharNum > 50)
  {
    byCharNum = 0;
  }

  return static_cast<int>(3451 - 69LL * (50 - byCharNum));
}

inline _reged_char_result_zone::_reged_char_result_zone()
  : byRetCode(0),
    byCharNum(0)
{
  std::memset(iLockType, 0, sizeof(iLockType));
}

inline int _reged_char_result_zone::size()
{
  if (byCharNum > 3u)
  {
    byCharNum = 0;
  }

  return static_cast<int>(221 - 69LL * (3 - byCharNum));
}

inline int _add_char_result_zone::size() const
{
  return sizeof(*this);
}

inline int _del_char_result_zone::size() const
{
  return sizeof(*this);
}

inline int _sel_char_result_zone::size() const
{
  return 14;
}

static_assert(sizeof(_alive_char_result_zocl) == 74, "_alive_char_result_zocl packing mismatch");
static_assert(sizeof(_not_arranged_char_inform_zocl) == 3451, "_not_arranged_char_inform_zocl packing mismatch");
static_assert(sizeof(_reged_char_result_zone) == 221, "_reged_char_result_zone packing mismatch");
static_assert(sizeof(_add_char_result_zone) == 2, "_add_char_result_zone packing mismatch");
static_assert(sizeof(_del_char_result_zone) == 2, "_del_char_result_zone packing mismatch");
static_assert(sizeof(_sel_char_result_zone) == 14, "_sel_char_result_zone packing mismatch");
static_assert(sizeof(_uilock_inform_request_zocl) == 4, "_uilock_inform_request_zocl packing mismatch");

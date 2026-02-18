#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

#pragma pack(push, 1)
struct  _qry_sheet_lobby
{
  unsigned int dwAvatorSerial;
  _AVATOR_DATA NewData;
  _AVATOR_DATA OldData;
  bool bUpdateRefineCnt;
  unsigned __int8 byRefinedCnt;
  unsigned int dwRefineDate;

  _qry_sheet_lobby();
  ~_qry_sheet_lobby();
  __int64 size() const;
};
#pragma pack(pop)

inline _qry_sheet_lobby::_qry_sheet_lobby() = default;
inline _qry_sheet_lobby::~_qry_sheet_lobby() = default;

inline __int64 _qry_sheet_lobby::size() const
{
  return 74440LL;
}

#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

#pragma pack(push, 1)
struct  _qry_sheet_lobby
{
  unsigned int dwAvatorSerial;
  _AVATOR_DATA NewData;
  _AVATOR_DATA OldData;
  unsigned __int64 dwCanonicalNewUnitCutTime[4];
  unsigned __int64 dwCanonicalOldUnitCutTime[4];
  bool bUpdateRefineCnt;
  unsigned __int8 byRefinedCnt;
  unsigned int dwRefineDate;

  _qry_sheet_lobby();
  ~_qry_sheet_lobby();
  int size() const;
};
#pragma pack(pop)

inline _qry_sheet_lobby::_qry_sheet_lobby() = default;
inline _qry_sheet_lobby::~_qry_sheet_lobby() = default;

inline int _qry_sheet_lobby::size() const
{
  return static_cast<int>(sizeof(*this));
}

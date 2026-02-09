#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

#pragma pack(push, 1)
struct __cppobj _qry_sheet_insert
{
  unsigned int dwAccountSerial;
  char szAccountID[13];
  _REGED_AVATOR_DB InsertData;

  _qry_sheet_insert();
  ~_qry_sheet_insert();
  __int64 size() const;
};
#pragma pack(pop)

inline _qry_sheet_insert::_qry_sheet_insert() = default;
inline _qry_sheet_insert::~_qry_sheet_insert() = default;

inline __int64 _qry_sheet_insert::size() const
{
  return 88LL;
}

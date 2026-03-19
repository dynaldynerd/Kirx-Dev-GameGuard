#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

#pragma pack(push, 1)
struct  _qry_sheet_insert
{
  unsigned int dwAccountSerial;
  char szAccountID[13];
  _REGED_AVATOR_DB InsertData;

  _qry_sheet_insert();
  ~_qry_sheet_insert();
  int size() const;
};
#pragma pack(pop)

inline _qry_sheet_insert::_qry_sheet_insert() = default;
inline _qry_sheet_insert::~_qry_sheet_insert() = default;

inline int _qry_sheet_insert::size() const
{
  // narrowing cast for thunk return parity
  return static_cast<int>(88LL);
}

#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"
#include "DqsDbStructs.h"

#pragma pack(push, 1)
struct __cppobj _qry_sheet_reged
{
  unsigned int dwAccountSerial;
  _REGED RegedData[3];
  _NOT_ARRANGED_AVATOR_DB ArrangedData[50];
  char in_szIP[16];

  _qry_sheet_reged();
  ~_qry_sheet_reged();
  __int64 size() const;
};
#pragma pack(pop)

inline _qry_sheet_reged::_qry_sheet_reged() = default;
inline _qry_sheet_reged::~_qry_sheet_reged() = default;

inline __int64 _qry_sheet_reged::size() const
{
  return 4280LL;
}

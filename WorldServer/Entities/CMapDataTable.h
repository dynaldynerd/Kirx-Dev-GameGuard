#pragma once

#include "IdaCompat.h"

/* 1464 */
struct __cppobj __declspec(align(8)) CMapDataTable
{
  _map_fld *m_pRecord;
  unsigned int m_dwRecordNum;
  virtual ~CMapDataTable() = default;
};

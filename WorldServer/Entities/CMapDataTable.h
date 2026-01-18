#pragma once

#include "IdaCompat.h"

struct _map_fld;

/* 1464 */
class __cppobj __declspec(align(8)) CMapDataTable
{
  _map_fld *m_pRecord;
  unsigned int m_dwRecordNum;
  virtual ~CMapDataTable() = default;
};


#pragma once

#include "IdaCompat.h"

struct _map_fld;

/* 1464 */
class  __declspec(align(8)) CMapDataTable
{
public:
  bool ReadScript(const char *szFileName);
  int GetRecordNum();
  _map_fld *GetRecord(unsigned int dwIndex);

  _map_fld *m_pRecord;
  unsigned int m_dwRecordNum;
  virtual ~CMapDataTable() = default;
};


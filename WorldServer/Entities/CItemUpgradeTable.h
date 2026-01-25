#pragma once

#include "IdaCompat.h"

/* 1294 */
class __cppobj CItemUpgradeTable
{
public:
  bool ReadRecord(const char *fileName, CRecordData *resTable, char *errCode);

  CRecordData m_tblItemUpgrade;
  int m_nResNum;
  unsigned __int16 *m_pwResIndex;
  virtual ~CItemUpgradeTable() = default;
};


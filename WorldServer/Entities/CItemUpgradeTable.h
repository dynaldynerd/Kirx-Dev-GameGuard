#pragma once

#include "IdaCompat.h"

struct _ItemUpgrade_fld;

/* 1294 */
class __cppobj CItemUpgradeTable
{
public:
  bool ReadRecord(const char *fileName, CRecordData *resTable, char *errCode);
  _ItemUpgrade_fld *GetRecord(unsigned int dwIndex);
  _ItemUpgrade_fld *GetRecordFromRes(unsigned int dwResIndex);

  CRecordData m_tblItemUpgrade;
  int m_nResNum;
  unsigned __int16 *m_pwResIndex;
  virtual ~CItemUpgradeTable() = default;

private:
  bool Indexing(CRecordData *resTable, char *errCode);
};


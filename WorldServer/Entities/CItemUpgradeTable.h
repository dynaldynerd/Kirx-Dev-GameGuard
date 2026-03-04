#pragma once

#include "IdaCompat.h"
#include "CRecordData.h"

struct _ItemUpgrade_fld;

/* 1294 */
class  CItemUpgradeTable
{
public:
  CItemUpgradeTable();

  bool ReadRecord(const char *fileName, CRecordData *resTable, char *errCode);
  _ItemUpgrade_fld *GetRecord(unsigned int dwIndex);
  _ItemUpgrade_fld *GetRecordFromRes(unsigned int dwResIndex);
  unsigned int GetSize();

  CRecordData m_tblItemUpgrade;
  int m_nResNum;
  unsigned __int16 *m_pwResIndex;
  virtual ~CItemUpgradeTable();

private:
  bool Indexing(CRecordData *resTable, char *errCode);
};


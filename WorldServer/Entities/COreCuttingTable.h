#pragma once

#include "IdaCompat.h"

/* 1290 */
class __cppobj COreCuttingTable
{
public:
  struct _ore_cut_list
  {
    struct _res_list
    {
      unsigned __int16 wResIndex;
      unsigned int dwRate;
      unsigned int dwCumRate;
    };

    int nResNum;
    unsigned int dwTotalRate;
    _res_list ResList[100];
  };

  COreCuttingTable();

  bool ReadRecord(const char *fileName, CRecordData *oreTable, CRecordData *resTable, char *errCode);
  unsigned int GetOreIndexFromRate(unsigned int dwOreIndex, unsigned int dwRate);

  virtual ~COreCuttingTable() = default;

private:
  bool Indexing(CRecordData *oreTable, CRecordData *resTable, char *errCode);
  CRecordData m_tblOreCutting;
  int m_nOreNum;
  _ore_cut_list *pOreList;
};


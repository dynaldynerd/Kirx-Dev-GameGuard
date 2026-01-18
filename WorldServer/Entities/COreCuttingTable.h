#pragma once

#include "IdaCompat.h"

/* 1290 */
class __cppobj COreCuttingTable
{
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

  CRecordData m_tblOreCutting;
  int m_nOreNum;
  _ore_cut_list *pOreList;
  virtual ~COreCuttingTable() = default;
};


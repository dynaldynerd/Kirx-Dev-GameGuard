#pragma once

#include "IdaCompat.h"

/* 1281 */
struct __cppobj CRecordData
{
  bool m_bLoad;
  char m_szFileName[129];
  unsigned int m_dwTotalSize;
  _record_bin_header m_Header;
  int m_nLowNum;
  char **m_ppsRecord;
  unsigned int *m_pdwHashList;
  virtual ~CRecordData() = default;
};

#pragma once

#include "IdaCompat.h"
#include "RecordDataTypes.h"

struct _base_fld;

/* 1281 */
class  CRecordData
{
public:
  CRecordData();
  bool m_bLoad;
  char m_szFileName[129];
  unsigned int m_dwTotalSize;
  _record_bin_header m_Header;
  int m_nLowNum;
  char **m_ppsRecord;
  unsigned int *m_pdwHashList;

  bool ReadRecord(const char *fileName, int structSize, char *errCode);
  bool ReadRecord_Ex(const char *fileName1, const char *fileName2, unsigned int structSize, char *errCode);
  bool LoadRecordHeader(void *hFile, char *errCode);
  bool LoadRecordData(void *hFile, char *errCode);
  unsigned int FileSize(const char *fileName);
  unsigned int GetRecordNum() const;
  _base_fld *GetRecord(int index);
  _base_fld *GetRecord(const char *code);
  _base_fld *GetRecord(const char *szRecordCode, int nCompareLen);
  _base_fld *GetRecordByHash(const char *szRecordCode, int offset, int len);
  bool MakeHashTable(int keyIndex, int keyLength, char *errCode);
  static unsigned int MakeHash(const char *p, int len);
  bool IsTableOpen() const;

  virtual ~CRecordData();
};


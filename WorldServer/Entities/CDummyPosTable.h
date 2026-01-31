#pragma once

#include "IdaCompat.h"

/* 1524 */
class __cppobj __declspec(align(8)) CDummyPosTable
{
  _dummy_position *m_pDumPos;
  int m_nDumPosDataNum;

public:
  CDummyPosTable();
  bool LoadDummyPosition(const char *szFileName, const char *szLabel);
  int GetRecordNum();
  void *GetRecord(int index);
  void *GetRecord(const char *szLabel);
  virtual ~CDummyPosTable();
};

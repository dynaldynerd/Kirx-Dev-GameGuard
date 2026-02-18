#pragma once

#include "IdaCompat.h"

struct _dummy_position;
class CGameObject;

extern const char *C_HEROES_DUMMY_NAME[3];

/* 1524 */
class  __declspec(align(8)) CDummyPosTable
{
  _dummy_position *m_pDumPos;
  int m_nDumPosDataNum;

public:
  static _dummy_position *ms_pHeroes_Dummy[3];
  static bool CheckHeroesDummy(CGameObject *pObj, unsigned __int8 byRaceCode);
  static bool FindDummy(char *pszTextFileName, char *pszDummyCode, _dummy_position *pDummyPos);

  CDummyPosTable();
  bool LoadDummyPosition(const char *szFileName, const char *szLabel);
  int GetRecordNum();
  void *GetRecord(int index);
  void *GetRecord(const char *szLabel);
  virtual ~CDummyPosTable();
};

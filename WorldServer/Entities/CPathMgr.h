#pragma once

#include "IdaCompat.h"

class CMonster;

/* 1671 */
class  CPathMgr
{
public:
  float m_PosPool[16][3];
  unsigned __int8 m_Size;
  unsigned __int8 m_StartPos;
  virtual ~CPathMgr();

  CPathMgr();
  void Init();
  unsigned __int8 GetPathSize();
  int PopNextPath(float *pPos);
  int SearchPathA(CMonster *pMon, float *vTarPos, int bBackupRestore);
  void Copy(CPathMgr *pDst);

  static CPathMgr ms_BackupPath;
};



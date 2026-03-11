#include "pch.h"

#include "CPathMgr.h"

#include "CBsp.h"
#include "CLevel.h"
#include "CMonster.h"

CPathMgr CPathMgr::ms_BackupPath;

CPathMgr::CPathMgr()
{
  Init();
  std::memset(m_PosPool, 0, sizeof(m_PosPool));
}

CPathMgr::~CPathMgr()
{
  Init();
}

void CPathMgr::Init()
{
  m_Size = 0;
  m_StartPos = 0;
}

unsigned __int8 CPathMgr::GetPathSize()
{
  return m_Size;
}

int CPathMgr::PopNextPath(float *pPos)
{
  if (!m_Size)
  {
    return 0;
  }
  std::memcpy(pPos, m_PosPool[m_StartPos], 12);
  ++m_StartPos;
  --m_Size;
  if (m_StartPos >= 16)
  {
    m_StartPos = 0;
  }
  return 1;
}

int CPathMgr::SearchPathA(CMonster *pMon, float *vTarPos, int bBackupRestore)
{
  unsigned int tempCount = 0;
  float tempPos[8] = {};

  if (!pMon)
  {
    return -2;
  }

  if (bBackupRestore)
  {
    Copy(&CPathMgr::ms_BackupPath);
  }

  Init();
  if (pMon->m_pCurMap->m_Level.mBsp->CanYouGoThere(pMon->m_fCurPos, vTarPos, (float (*)[3])tempPos))
  {
    m_Size = 1;
    m_StartPos = 0;
    std::memcpy(m_PosPool, vTarPos, 12);
    return m_Size;
  }

  CLevel *level = &pMon->m_pCurMap->m_Level;
  unsigned int pathCount = level->GetPathFromDepth(pMon->m_fCurPos, vTarPos, 16, m_PosPool, &tempCount);
  if (pathCount)
  {
    m_Size = tempCount;
    m_StartPos = 0;
    return m_Size;
  }

  if (bBackupRestore)
  {
    CPathMgr::ms_BackupPath.Copy(this);
  }
  return -1;
}

void CPathMgr::Copy(CPathMgr *pDst)
{
  pDst->m_Size = m_Size;
  pDst->m_StartPos = m_StartPos;
  for (int j = m_StartPos; j < m_Size + m_StartPos; ++j)
  {
    std::memcpy(pDst->m_PosPool[j], m_PosPool[j], sizeof(pDst->m_PosPool[j]));
  }
}

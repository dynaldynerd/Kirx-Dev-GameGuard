#include "CResourceDataMgr.h"

#include <new>

#include "DataFile.h"

CCharResDataMgr *g_pCharResDataMgr = NULL;

namespace
{
BOOL ReadCountedBlock(CDataString *pi_pDataString,
                      void **po_ppData,
                      DWORD *po_pCount,
                      DWORD pi_dwStride)
{
  if (!pi_pDataString || !po_ppData || !po_pCount || !pi_dwStride)
  {
    return FALSE;
  }

  *po_ppData = NULL;
  *po_pCount = 0;

  DWORD l_dwCount = 0;
  if (pi_pDataString->Read(&l_dwCount, sizeof(DWORD), 1) != sizeof(DWORD))
  {
    return FALSE;
  }

  *po_pCount = l_dwCount;
  if (l_dwCount == 0)
  {
    return TRUE;
  }

  BYTE *l_pData = new (std::nothrow) BYTE[l_dwCount * pi_dwStride];
  if (!l_pData)
  {
    return FALSE;
  }

  const DWORD l_dwReadSize = pi_dwStride * l_dwCount;
  if (pi_pDataString->Read(l_pData, pi_dwStride, l_dwCount) != l_dwReadSize)
  {
    delete[] l_pData;
    *po_pCount = 0;
    return FALSE;
  }

  *po_ppData = l_pData;
  return TRUE;
}
} // namespace

CCharResData::CCharResData()
{
  Init();
}

CCharResData::~CCharResData()
{
  Destroy();
}

void CCharResData::Init(void)
{
  m_dwBoneNum = 0;
  m_dwMeshNum = 0;
  m_dwAniNum = 0;
  m_listBone = NULL;
  m_listMesh = NULL;
  m_listAni = NULL;
}

void CCharResData::Destroy(void)
{
  delete[] m_listBone;
  delete[] m_listMesh;
  delete[] m_listAni;
  Init();
}

BOOL CCharResData::LoadBoneDataList(CDataString *pi_pDataString)
{
  delete[] m_listBone;
  m_listBone = NULL;
  m_dwBoneNum = 0;

  return ReadCountedBlock(pi_pDataString,
                          reinterpret_cast<void **>(&m_listBone),
                          &m_dwBoneNum,
                          sizeof(BONE_DATA));
}

BOOL CCharResData::LoadMeshDataList(CDataString *pi_pDataString)
{
  delete[] m_listMesh;
  m_listMesh = NULL;
  m_dwMeshNum = 0;

  return ReadCountedBlock(pi_pDataString,
                          reinterpret_cast<void **>(&m_listMesh),
                          &m_dwMeshNum,
                          sizeof(MESH_DATA));
}

BOOL CCharResData::LoadAniDataList(CDataString *pi_pDataString)
{
  delete[] m_listAni;
  m_listAni = NULL;
  m_dwAniNum = 0;

  return ReadCountedBlock(pi_pDataString,
                          reinterpret_cast<void **>(&m_listAni),
                          &m_dwAniNum,
                          sizeof(ANI_DATA));
}

BONE_DATA *CCharResData::GetBoneData(DWORD pi_dwIndex)
{
  if (pi_dwIndex == static_cast<DWORD>(-1))
  {
    return NULL;
  }

  for (DWORD i = 0; i < m_dwBoneNum; ++i)
  {
    if (m_listBone[i].dwID == pi_dwIndex)
    {
      return &m_listBone[i];
    }
  }

  return NULL;
}

MESH_DATA *CCharResData::GetMeshData(DWORD pi_dwIndex)
{
  if (pi_dwIndex == static_cast<DWORD>(-1))
  {
    return NULL;
  }

  for (DWORD i = 0; i < m_dwMeshNum; ++i)
  {
    if (m_listMesh[i].dwID == pi_dwIndex)
    {
      return &m_listMesh[i];
    }
  }

  return NULL;
}

ANI_DATA *CCharResData::GetAniData(DWORD pi_dwIndex)
{
  if (pi_dwIndex == static_cast<DWORD>(-1))
  {
    return NULL;
  }

  for (DWORD i = 0; i < m_dwAniNum; ++i)
  {
    if (m_listAni[i].dwID == pi_dwIndex)
    {
      return &m_listAni[i];
    }
  }

  return NULL;
}

BONE_DATA *CCharResData::GetBoneDataByOrder(DWORD pi_dwIndex)
{
  return pi_dwIndex < m_dwBoneNum ? &m_listBone[pi_dwIndex] : NULL;
}

MESH_DATA *CCharResData::GetMeshDataByOrder(DWORD pi_dwIndex)
{
  return pi_dwIndex < m_dwMeshNum ? &m_listMesh[pi_dwIndex] : NULL;
}

ANI_DATA *CCharResData::GetAniDataByOrder(DWORD pi_dwIndex)
{
  return pi_dwIndex < m_dwAniNum ? &m_listAni[pi_dwIndex] : NULL;
}

DWORD CCharResData::GetTotalBoneNum(void) const
{
  return m_dwBoneNum;
}

DWORD CCharResData::GetTotalMeshNum(void) const
{
  return m_dwMeshNum;
}

DWORD CCharResData::GetTotalAniNum(void) const
{
  return m_dwAniNum;
}

CCharResDataMgr::CCharResDataMgr()
  : m_bLoaded(FALSE)
{
}

CCharResDataMgr::~CCharResDataMgr()
{
  UnloadData();
}

BOOL CCharResDataMgr::LoadData(void)
{
  UnloadData();

  CDataFile l_fileResData(const_cast<char *>(".\\DataTable\\Resource.edf"));
  CDataString *l_pSourceData = l_fileResData.GetSourceData();
  if (!l_pSourceData || !l_pSourceData->GetString())
  {
    return FALSE;
  }

  for (int i = 0; i < MAX_RES_DATA_LIST; ++i)
  {
    if (!m_listResData[i].LoadBoneDataList(l_pSourceData) ||
        !m_listResData[i].LoadMeshDataList(l_pSourceData) ||
        !m_listResData[i].LoadAniDataList(l_pSourceData))
    {
      UnloadData();
      return FALSE;
    }
  }

  m_bLoaded = TRUE;
  return TRUE;
}

void CCharResDataMgr::UnloadData(void)
{
  for (int i = 0; i < MAX_RES_DATA_LIST; ++i)
  {
    m_listResData[i].Destroy();
  }

  m_bLoaded = FALSE;
}

BOOL CCharResDataMgr::IsLoaded(void) const
{
  return m_bLoaded;
}

CCharResData *CCharResDataMgr::GetResourceList(BYTE pi_byType)
{
  return pi_byType < MAX_RES_DATA_LIST ? &m_listResData[pi_byType] : NULL;
}

const CCharResData *CCharResDataMgr::GetResourceList(BYTE pi_byType) const
{
  return pi_byType < MAX_RES_DATA_LIST ? &m_listResData[pi_byType] : NULL;
}

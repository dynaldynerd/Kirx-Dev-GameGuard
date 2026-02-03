#include "pch.h"

#include "CGameObject.h"

#include "CMapData.h"
#include "CNetworkEX.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "pnt_rect.h"

#include <cstdlib>
#include <ctime>

bool _100_per_random_table::s_bRecordSet = false;
unsigned __int16 _100_per_random_table::s_wRecord[10][100] = {};

CGameObject *CGameObject::s_pSelectObject = nullptr;
CGameObject *CGameObject::s_pTotalObject[42642] = {};
int CGameObject::s_nTotalObjectNum = 0;

_object_id::_object_id()
{
  m_byKind = 0;
  m_byID = 0;
  m_wIndex = 0;
}

_object_id::_object_id(unsigned __int8 byKind, unsigned __int8 byID, unsigned __int16 wIndex)
{
  m_byKind = byKind;
  m_byID = byID;
  m_wIndex = wIndex;
}

_100_per_random_table::_100_per_random_table()
{
  if (!s_bRecordSet)
  {
    s_bRecordSet = true;
    for (int j = 0; j < 10; ++j)
    {
      for (int k = 0; k < 100; ++k)
      {
        s_wRecord[j][k] = static_cast<unsigned __int16>(k);
      }
    }
  }
  m_wCurTable = 0;
  m_wCurPoint = 0;
}

unsigned __int16 _100_per_random_table::GetRand()
{
  if (m_wCurPoint >= 100)
  {
    m_wCurPoint = 0;
  }
  unsigned __int16 value = s_wRecord[m_wCurTable][m_wCurPoint++];
  return static_cast<unsigned __int16>(value % 100);
}

void _100_per_random_table::reset()
{
  m_wCurTable = 0;
  m_wCurPoint = 0;
}

void CGameObject::Init(_object_id *pID)
{
  m_ObjID = *pID;
  m_SectorPoint.SetPoint(this);
  m_SectorNetPoint.SetPoint(this);
  m_bMapLoading = false;
  m_bMaxVision = false;
  m_nCirclePlayerNum = 0;
  s_pTotalObject[s_nTotalObjectNum] = this;
  m_nTotalObjIndex = s_nTotalObjectNum;
  m_pRecordSet = nullptr;
  m_dwObjSerial = 0;
  m_bLive = false;
  m_bCorpse = false;
  m_bMove = false;
  m_bStun = false;
  m_dwLastSendTime = 0;
  m_fCurPos[0] = 0.0f;
  m_fCurPos[1] = 0.0f;
  m_fCurPos[2] = 0.0f;
  m_fAbsPos[0] = 0.0f;
  m_fAbsPos[1] = 0.0f;
  m_fAbsPos[2] = 0.0f;
  m_nScreenPos[0] = 0;
  m_nScreenPos[1] = 0;
  m_fOldPos[0] = 0.0f;
  m_fOldPos[1] = 0.0f;
  m_fOldPos[2] = 0.0f;
  m_pCurMap = nullptr;
  m_wMapLayerIndex = 0;
  m_dwNextFreeStunTime = static_cast<unsigned int>(-1);
  m_dwOldTickBreakTranspar = static_cast<unsigned int>(-1);
  m_bBreakTranspar = false;
  m_bPlayerCircleList = nullptr;
  m_bObserver = false;
  m_dwCurSec = 0;
  if (++s_nTotalObjectNum > 42642)
  {
    MyMessageBox("error", "CGameObject::Init : Lack Object Num");
    ServerProgramExit("CGameObject::Init()", 0);
  }
}

/*
CGameObject *CGameObject::s_pSelectObject = nullptr;
CGameObject *CGameObject::s_pTotalObject[42642] = {};
int CGameObject::s_nTotalObjectNum = 0;

bool _100_per_random_table::s_bRecordSet = false;
unsigned __int16 _100_per_random_table::s_wRecord[10][100] = {};

_object_id::_object_id()
{
  ;
}

_object_id::_object_id(unsigned __int8 byKind, unsigned __int8 byID, unsigned __int16 wIndex)
{
  m_byKind = byKind;
  m_byID = byID;
  m_wIndex = wIndex;
}

_100_per_random_table::_100_per_random_table()
{
  if ( !s_bRecordSet )
  {
    s_bRecordSet = true;
    srand(static_cast<unsigned int>(time(nullptr)));
    for ( int j = 0; j < 10; ++j )
    {
      for ( int k = 0; k < 100; ++k )
        s_wRecord[j][k] = static_cast<unsigned __int16>(k);
    }
    reset();
  }
  m_wCurTable = rand() % 10;
  m_wCurPoint = 0;
}

unsigned __int16 _100_per_random_table::GetRand()
{
  if ( m_wCurPoint >= 0x64u )
  {
    m_wCurTable = rand() % 10;
    m_wCurPoint = 0;
  }
  unsigned __int16 value = s_wRecord[m_wCurTable][m_wCurPoint++];
  return static_cast<unsigned __int16>(value % 100);
}

void _100_per_random_table::reset()
{
  for ( int j = 0; j < 10; ++j )
  {
    for ( int k = 0; k < 100; ++k )
    {
      unsigned __int16 v6 = rand() % 100;
      unsigned __int16 v7 = rand() % 100;
      unsigned __int16 v8 = s_wRecord[j][v7];
      s_wRecord[j][v7] = s_wRecord[j][v6];
      s_wRecord[j][v6] = v8;
    }
  }
}

void CGameObject::AlterSec()
{
  ;
}

__int64 CGameObject::AttackableHeight()
{
  return 50LL;
}

void CGameObject::BeTargeted(CCharacter *pSeacher)
{
  ;
}

CGameObject::CGameObject()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v3; // [rsp+0h] [rbp-28h] BYREF

  v1 = &v3;
  for ( i = 8LL; i; --i )
  {
    *(_DWORD *)v1 = -858993460;
    v1 = (__int64 *)((char *)v1 + 4);
  }
  this->m_ObjID = _object_id();
  this->m_rtPer100 = _100_per_random_table();
  this->m_SectorPoint = _object_list_point();
  this->m_SectorNetPoint = _object_list_point();
  this->m_pRecordSet = 0LL;
  this->m_pCurMap = 0LL;
  this->m_wMapLayerIndex = 0;
  this->m_bLive = 0;
  this->m_bMove = 0;
  this->m_bCorpse = 0;
  this->m_bMaxVision = 0;
  this->m_nTotalObjIndex = -1;
  this->m_bPlayerCircleList = 0LL;
}

void CGameObject::CalcAbsPos()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v3; // [rsp+0h] [rbp-38h] BYREF
  _bsp_info *BspInfo; // [rsp+20h] [rbp-18h]

  v1 = &v3;
  for ( i = 12LL; i; --i )
  {
    *(_DWORD *)v1 = -858993460;
    v1 = (__int64 *)((char *)v1 + 4);
  }
  BspInfo = CMapData::GetBspInfo(this->m_pCurMap);
  this->m_fAbsPos[0] = (float)-BspInfo->m_nMapMinSize[0] + this->m_fCurPos[0];
  this->m_fAbsPos[1] = (float)BspInfo->m_nMapMaxSize[1] - this->m_fCurPos[1];
  this->m_fAbsPos[2] = (float)BspInfo->m_nMapMaxSize[2] - this->m_fCurPos[2];
}

__int64 CGameObject::CalcCirclePlayerNum(int nRange)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-78h] BYREF
  _pnt_rect pRect; // [rsp+28h] [rbp-50h] BYREF
  _sec_info *SecInfo; // [rsp+48h] [rbp-30h]
  unsigned int v8; // [rsp+50h] [rbp-28h]
  int j; // [rsp+54h] [rbp-24h]
  int k; // [rsp+58h] [rbp-20h]
  unsigned int dwSecIndex; // [rsp+5Ch] [rbp-1Ch]
  CObjectList *SectorListPlayer; // [rsp+60h] [rbp-18h]

  v2 = &v5;
  for ( i = 28LL; i; --i )
  {
    *(_DWORD *)v2 = -858993460;
    v2 = (__int64 *)((char *)v2 + 4);
  }
  if ( this->m_bPlayerCircleList )
    return 1LL;
  SecInfo = CMapData::GetSecInfo(this->m_pCurMap);
  CMapData::GetRectInRadius(this->m_pCurMap, &pRect, nRange, this->m_dwCurSec);
  v8 = 0;
  for ( j = pRect.nStarty; j <= pRect.nEndy; ++j )
  {
    for ( k = pRect.nStartx; k <= pRect.nEndx; ++k )
    {
      dwSecIndex = SecInfo->m_nSecNumW * j + k;
      SectorListPlayer = CMapData::GetSectorListPlayer(this->m_pCurMap, this->m_wMapLayerIndex, dwSecIndex);
      if ( SectorListPlayer )
        v8 += SectorListPlayer->m_nSize;
    }
  }
  return v8;
}

__int64 CGameObject::CalcCirclePlayerNum(int nRange, bool (__fastcall *fnComp)(CGameObject *))
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-88h] BYREF
  _pnt_rect pRect; // [rsp+28h] [rbp-60h] BYREF
  _sec_info *SecInfo; // [rsp+48h] [rbp-40h]
  unsigned int v9; // [rsp+50h] [rbp-38h]
  int j; // [rsp+54h] [rbp-34h]
  int k; // [rsp+58h] [rbp-30h]
  unsigned int dwSecIndex; // [rsp+5Ch] [rbp-2Ch]
  CObjectList *SectorListPlayer; // [rsp+60h] [rbp-28h]
  _object_list_point *m_pNext; // [rsp+68h] [rbp-20h]
  CGameObject *v15; // [rsp+70h] [rbp-18h]

  v3 = &v6;
  for ( i = 32LL; i; --i )
  {
    *(_DWORD *)v3 = -858993460;
    v3 = (__int64 *)((char *)v3 + 4);
  }
  if ( this->m_bPlayerCircleList )
    return 1LL;
  SecInfo = CMapData::GetSecInfo(this->m_pCurMap);
  CMapData::GetRectInRadius(this->m_pCurMap, &pRect, nRange, this->m_dwCurSec);
  v9 = 0;
  for ( j = pRect.nStarty; j <= pRect.nEndy; ++j )
  {
    for ( k = pRect.nStartx; k <= pRect.nEndx; ++k )
    {
      dwSecIndex = SecInfo->m_nSecNumW * j + k;
      SectorListPlayer = CMapData::GetSectorListPlayer(this->m_pCurMap, this->m_wMapLayerIndex, dwSecIndex);
      if ( SectorListPlayer )
      {
        m_pNext = SectorListPlayer->m_Head.m_pNext;
        while ( m_pNext != &SectorListPlayer->m_Tail )
        {
          v15 = m_pNext->m_pItem;
          m_pNext = m_pNext->m_pNext;
          if ( fnComp(v15) )
            ++v9;
        }
      }
    }
  }
  return v9;
}

__int64 CGameObject::CalcCurHPRate()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v3; // eax
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]

  v1 = &v5;
  for ( i = 12LL; i; --i )
  {
    *(_DWORD *)v1 = -858993460;
    v1 = (__int64 *)((char *)v1 + 4);
  }
  v6 = (float)this->GetHP();
  v3 = this->GetMaxHP();
  return (unsigned int)(int)(float)((float)(v6 / (float)v3) * 10000.0);
}

void CGameObject::CalcScrExtendPoint(CRect *prcWnd, CRect *prcExtend)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-48h] BYREF
  float v6; // [rsp+28h] [rbp-20h]
  float v7; // [rsp+2Ch] [rbp-1Ch]

  v3 = &v5;
  for ( i = 16LL; i; --i )
  {
    *(_DWORD *)v3 = -858993460;
    v3 = (__int64 *)((char *)v3 + 4);
  }
  v6 = this->m_fAbsPos[0] - (float)prcExtend->left;
  v7 = this->m_fAbsPos[2] - (float)prcExtend->top;
  this->m_nScreenPos[0] = (int)(float)((float)(v6 * (float)prcWnd->right) / (float)(prcExtend->right - prcExtend->left));
  this->m_nScreenPos[1] = (int)(float)((float)(v7 * (float)prcWnd->bottom) / (float)(prcExtend->bottom - prcExtend->top));
}

void CGameObject::CalcScrNormalPoint(CRect *prcWnd)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
  int right; // [rsp+20h] [rbp-18h]
  int bottom; // [rsp+24h] [rbp-14h]
  _bsp_info *BspInfo; // [rsp+28h] [rbp-10h]

  v2 = &v4;
  for ( i = 12LL; i; --i )
  {
    *(_DWORD *)v2 = -858993460;
    v2 = (__int64 *)((char *)v2 + 4);
  }
  right = prcWnd->right;
  bottom = prcWnd->bottom;
  BspInfo = CMapData::GetBspInfo(this->m_pCurMap);
  this->m_nScreenPos[0] = right * (int)this->m_fAbsPos[0] / BspInfo->m_nMapSize[0];
  this->m_nScreenPos[1] = bottom * (int)this->m_fAbsPos[2] / BspInfo->m_nMapSize[2];
}

__int64 CGameObject::CalcSecIndex()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
  unsigned int v5; // [rsp+20h] [rbp-18h]
  unsigned int v6; // [rsp+24h] [rbp-14h]
  _sec_info *SecInfo; // [rsp+28h] [rbp-10h]

  v1 = &v4;
  for ( i = 12LL; i; --i )
  {
    *(_DWORD *)v1 = -858993460;
    v1 = (__int64 *)((char *)v1 + 4);
  }
  CGameObject::CalcAbsPos();
  v5 = (int)(float)(this->m_fAbsPos[0] / 100.0);
  v6 = (int)(float)(this->m_fAbsPos[2] / 100.0);
  SecInfo = CMapData::GetSecInfo(this->m_pCurMap);
  if ( v5 < SecInfo->m_nSecNumW && v6 < SecInfo->m_nSecNumH )
    return SecInfo->m_nSecNumW * v6 + v5;
  g_Main.m_logSystemError.Write(
    "kind(%d), id(%d).. Out of Sector",
    this->m_ObjID.m_byKind,
    this->m_ObjID.m_byID);
  return this->m_dwCurSec;
}

void CGameObject::CircleReport(unsigned __int8 *pbyType, char *szMsg, unsigned __int16 nMsgSize, bool bToOne)
{
  __int64 *v5; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-A8h] BYREF
  unsigned int dwClientIndex; // [rsp+30h] [rbp-78h]
  _pnt_rect pRect; // [rsp+48h] [rbp-60h] BYREF
  _sec_info *SecInfo; // [rsp+68h] [rbp-40h]
  int nRadius; // [rsp+70h] [rbp-38h]
  int j; // [rsp+74h] [rbp-34h]
  int k; // [rsp+78h] [rbp-30h]
  unsigned int dwSecIndex; // [rsp+7Ch] [rbp-2Ch]
  CObjectList *SectorListPlayer; // [rsp+80h] [rbp-28h]
  _object_list_point *m_pNext; // [rsp+88h] [rbp-20h]
  CGameObject *m_pItem; // [rsp+90h] [rbp-18h]
  _object_id *p_m_ObjID; // [rsp+98h] [rbp-10h]

  v5 = &v7;
  for ( i = 40LL; i; --i )
  {
    *(_DWORD *)v5 = -858993460;
    v5 = (__int64 *)((char *)v5 + 4);
  }
  if ( this->m_bPlayerCircleList )
  {
    for ( dwClientIndex = 0; (int)dwClientIndex < 2532; ++dwClientIndex )
    {
      if ( this->m_bPlayerCircleList[dwClientIndex] )
        g_Network.m_pProcess[0]->LoadSendMsg(dwClientIndex, pbyType, szMsg, nMsgSize);
    }
  }
  else
  {
    if ( bToOne && !this->m_ObjID.m_byKind && !this->m_ObjID.m_byID )
      g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, nMsgSize);
    if ( this->m_pCurMap && this->m_dwCurSec != -1 )
    {
      SecInfo = CMapData::GetSecInfo(this->m_pCurMap);
      nRadius = CGameObject::GetUseSectorRange();
      CMapData::GetRectInRadius(this->m_pCurMap, &pRect, nRadius, this->m_dwCurSec);
      for ( j = pRect.nStarty; j <= pRect.nEndy; ++j )
      {
        for ( k = pRect.nStartx; k <= pRect.nEndx; ++k )
        {
          dwSecIndex = SecInfo->m_nSecNumW * j + k;
          SectorListPlayer = CMapData::GetSectorListPlayer(this->m_pCurMap, this->m_wMapLayerIndex, dwSecIndex);
          if ( SectorListPlayer )
          {
            m_pNext = SectorListPlayer->m_Head.m_pNext;
            while ( m_pNext != &SectorListPlayer->m_Tail )
            {
              m_pItem = m_pNext->m_pItem;
              m_pNext = m_pNext->m_pNext;
              p_m_ObjID = &m_pItem->m_ObjID;
              if ( m_pItem != this && (!this->m_bObserver || BYTE4(m_pItem[9].m_SectorNetPoint.m_pNext)) )
                g_Network.m_pProcess[0]->LoadSendMsg(p_m_ObjID->m_wIndex, pbyType, szMsg, nMsgSize);
            }
          }
        }
      }
    }
  }
}

void CGameObject::CircleReport(unsigned __int8 *pbyType, char *szMsg, unsigned __int16 nMsgSize, unsigned int dwPassObjSerial, bool bToOne)
{
  __int64 *v6; // rdi
  __int64 i; // rcx
  __int64 v8; // [rsp+0h] [rbp-A8h] BYREF
  unsigned int dwClientIndex; // [rsp+30h] [rbp-78h]
  _pnt_rect pRect; // [rsp+48h] [rbp-60h] BYREF
  _sec_info *SecInfo; // [rsp+68h] [rbp-40h]
  int nRadius; // [rsp+70h] [rbp-38h]
  int j; // [rsp+74h] [rbp-34h]
  int k; // [rsp+78h] [rbp-30h]
  unsigned int dwSecIndex; // [rsp+7Ch] [rbp-2Ch]
  CObjectList *SectorListPlayer; // [rsp+80h] [rbp-28h]
  _object_list_point *m_pNext; // [rsp+88h] [rbp-20h]
  CGameObject *m_pItem; // [rsp+90h] [rbp-18h]
  _object_id *p_m_ObjID; // [rsp+98h] [rbp-10h]

  v6 = &v8;
  for ( i = 40LL; i; --i )
  {
    *(_DWORD *)v6 = -858993460;
    v6 = (__int64 *)((char *)v6 + 4);
  }
  if ( this->m_bPlayerCircleList )
  {
    for ( dwClientIndex = 0; (int)dwClientIndex < 2532; ++dwClientIndex )
    {
      if ( this->m_bPlayerCircleList[dwClientIndex] )
        g_Network.m_pProcess[0]->LoadSendMsg(dwClientIndex, pbyType, szMsg, nMsgSize);
    }
  }
  else
  {
    if ( bToOne && !this->m_ObjID.m_byKind && !this->m_ObjID.m_byID )
      g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, nMsgSize);
    if ( this->m_pCurMap && this->m_dwCurSec != -1 )
    {
      SecInfo = CMapData::GetSecInfo(this->m_pCurMap);
      nRadius = CGameObject::GetUseSectorRange();
      CMapData::GetRectInRadius(this->m_pCurMap, &pRect, nRadius, this->m_dwCurSec);
      for ( j = pRect.nStarty; j <= pRect.nEndy; ++j )
      {
        for ( k = pRect.nStartx; k <= pRect.nEndx; ++k )
        {
          dwSecIndex = SecInfo->m_nSecNumW * j + k;
          SectorListPlayer = CMapData::GetSectorListPlayer(this->m_pCurMap, this->m_wMapLayerIndex, dwSecIndex);
          if ( SectorListPlayer )
          {
            m_pNext = SectorListPlayer->m_Head.m_pNext;
            while ( m_pNext != &SectorListPlayer->m_Tail )
            {
              m_pItem = m_pNext->m_pItem;
              m_pNext = m_pNext->m_pNext;
              p_m_ObjID = &m_pItem->m_ObjID;
              if ( m_pItem != this
                && m_pItem->m_dwObjSerial != dwPassObjSerial
                && (!this->m_bObserver || BYTE4(m_pItem[9].m_SectorNetPoint.m_pNext)) )
              {
                g_Network.m_pProcess[0]->LoadSendMsg(p_m_ObjID->m_wIndex, pbyType, szMsg, nMsgSize);
              }
            }
          }
        }
      }
    }
  }
}

char CGameObject::Create(_object_create_setdata *pData)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  _sec_info *SecInfo; // rax
  __int64 v6; // [rsp+0h] [rbp-48h] BYREF
  unsigned int dwSecIndex; // [rsp+30h] [rbp-18h]

  v2 = &v6;
  for ( i = 16LL; i; --i )
  {
    *(_DWORD *)v2 = -858993460;
    v2 = (__int64 *)((char *)v2 + 4);
  }
  if ( this->m_bLive )
    return 0;
  this->m_pCurMap = pData->m_pMap;
  this->m_wMapLayerIndex = pData->m_nLayerIndex;
  memcpy_0(this->m_fCurPos, pData->m_fStartPos, sizeof(this->m_fCurPos));
  memcpy_0(this->m_fOldPos, pData->m_fStartPos, sizeof(this->m_fOldPos));
  this->m_bMaxVision = 0;
  this->m_bObserver = 0;
  this->m_nCirclePlayerNum = 0;
  dwSecIndex = CGameObject::CalcSecIndex();
  SecInfo = CMapData::GetSecInfo(pData->m_pMap);
  if ( dwSecIndex < SecInfo->m_nSecNum )
  {
    CMapData::EnterMap(pData->m_pMap, this, dwSecIndex);
    this->m_bLive = 1;
    this->m_pRecordSet = pData->m_pRecordSet;
    this->m_dwCurSec = dwSecIndex;
    this->m_bMove = 0;
    this->m_bCorpse = 0;
    this->m_bMapLoading = 0;
    this->m_bBreakTranspar = 0;
    this->m_dwLastSendTime = GetLoopTime();
    this->m_dwOldTickBreakTranspar = GetLoopTime();
    return 1;
  }
  else
  {
    this->m_pCurMap = 0LL;
    g_Main.m_logSystemError.Write(
      "CGameObject::Create() : dwSec >= MAX(this[%d-%d-%d])",
      this->m_ObjID.m_byKind,
      this->m_ObjID.m_byID,
      this->m_ObjID.m_wIndex);
    return 0;
  }
}

char CGameObject::Destroy()
{
  _DWORD *v1; // rdi
  __int64 i; // rcx
  _QWORD v4[5]; // [rsp+0h] [rbp-38h] BYREF
  void *m_bPlayerCircleList; // [rsp+28h] [rbp-10h]

  v1 = v4;
  for ( i = 12LL; i; --i )
    *v1++ = -858993460;
  if ( !this->m_bLive )
    return 0;
  if ( this->m_pCurMap )
  {
    v4[4] = CMapData::GetSecInfo(this->m_pCurMap);
    CMapData::ExitMap(this->m_pCurMap, this, this->m_dwCurSec);
  }
  if ( CGameObject::s_pSelectObject == this )
    CGameObject::s_pSelectObject = 0LL;
  this->m_bLive = 0;
  this->m_bCorpse = 0;
  this->m_bMove = 0;
  this->m_bStun = 0;
  this->m_bMapLoading = 0;
  this->m_SectorPoint.InitLink();
  this->m_SectorNetPoint.InitLink();
  if ( this->m_bPlayerCircleList )
  {
    m_bPlayerCircleList = this->m_bPlayerCircleList;
    operator delete[](m_bPlayerCircleList);
  }
  this->m_bPlayerCircleList = 0LL;
  this->m_bMaxVision = 0;
  return 1;
}

bool CGameObject::FixTargetWhile(CCharacter *pkTarget, unsigned int dwMiliSecond)
{
  return 0;
}

__int64 CGameObject::GetAttackDP()
{
  return 0LL;
}

__int64 CGameObject::GetAttackLevel()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  v1 = &v4;
  for ( i = 8LL; i; --i )
  {
    *(_DWORD *)v1 = -858993460;
    v1 = (__int64 *)((char *)v1 + 4);
  }
  return this->GetLevel();
}

float CGameObject::GetAttackRange()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-18h] BYREF

  v1 = &v4;
  for ( i = 4LL; i; --i )
  {
    *(_DWORD *)v1 = -858993460;
    v1 = (__int64 *)((char *)v1 + 4);
  }
  return 0.0;
}

__int64 CGameObject::GetAvoidRate()
{
  return 0LL;
}

__int64 CGameObject::GetCurSecNum()
{
  return this->m_dwCurSec;
}

__int64 CGameObject::GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart)
{
  return 1LL;
}

float CGameObject::GetDefFacing(int nPart)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-18h] BYREF

  v2 = &v5;
  for ( i = 4LL; i; --i )
  {
    *(_DWORD *)v2 = -858993460;
    v2 = (__int64 *)((char *)v2 + 4);
  }
  return FLOAT_0_5;
}

float CGameObject::GetDefGap(int nPart)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-18h] BYREF

  v2 = &v5;
  for ( i = 4LL; i; --i )
  {
    *(_DWORD *)v2 = -858993460;
    v2 = (__int64 *)((char *)v2 + 4);
  }
  return FLOAT_0_5;
}

__int64 CGameObject::GetDefSkill(bool bBackAttackDamage)
{
  return 1LL;
}

__int64 CGameObject::GetFireTol()
{
  return 0LL;
}

__int64 CGameObject::GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack)
{
  return 0LL;
}

__int64 CGameObject::GetHP()
{
  return 1LL;
}

__int64 CGameObject::GetLevel()
{
  return 1LL;
}

__int64 CGameObject::GetMaxHP()
{
  return 1LL;
}

char * CGameObject::GetObjName()
{
  return 0LL;
}

__int64 CGameObject::GetObjRace()
{
  return 0xFFFFFFFFLL;
}

__int64 CGameObject::GetSoilTol()
{
  return 0LL;
}

bool CGameObject::GetStun()
{
  return this->m_bStun;
}

__int64 CGameObject::GetUseSectorRange()
{
  return (unsigned int)this->m_pCurMap->m_pMapSet->m_nRadius;
}

__int64 CGameObject::GetWaterTol()
{
  return 0LL;
}

float CGameObject::GetWeaponAdjust()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-18h] BYREF

  v1 = &v4;
  for ( i = 4LL; i; --i )
  {
    *(_DWORD *)v1 = -858993460;
    v1 = (__int64 *)((char *)v1 + 4);
  }
  return FLOAT_0_5;
}

__int64 CGameObject::GetWeaponClass()
{
  return 0LL;
}

float CGameObject::GetWidth()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-18h] BYREF

  v1 = &v4;
  for ( i = 4LL; i; --i )
  {
    *(_DWORD *)v1 = -858993460;
    v1 = (__int64 *)((char *)v1 + 4);
  }
  return 0.0;
}

__int64 CGameObject::GetWindTol()
{
  return 0LL;
}

void CGameObject::Init(_object_id *pID)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  v2 = &v4;
  for ( i = 8LL; i; --i )
  {
    *(_DWORD *)v2 = -858993460;
    v2 = (__int64 *)((char *)v2 + 4);
  }
  this->m_ObjID = *pID;
  this->m_SectorPoint.SetPoint(this);
  this->m_SectorNetPoint.SetPoint(this);
  this->m_bMapLoading = 0;
  this->m_bMaxVision = 0;
  this->m_nCirclePlayerNum = 0;
  CGameObject::s_pTotalObject[CGameObject::s_nTotalObjectNum] = this;
  this->m_nTotalObjIndex = CGameObject::s_nTotalObjectNum;
  this->m_pRecordSet = 0LL;
  this->m_dwObjSerial = 0;
  this->m_bLive = 0;
  this->m_bCorpse = 0;
  this->m_bMove = 0;
  this->m_bStun = 0;
  this->m_dwLastSendTime = 0;
  this->m_fCurPos[0] = 0.0;
  this->m_fCurPos[1] = 0.0;
  this->m_fCurPos[2] = 0.0;
  this->m_fAbsPos[0] = 0.0;
  this->m_fAbsPos[1] = 0.0;
  this->m_fAbsPos[2] = 0.0;
  this->m_nScreenPos[0] = 0;
  this->m_nScreenPos[1] = 0;
  this->m_fOldPos[0] = 0.0;
  this->m_fOldPos[1] = 0.0;
  this->m_fOldPos[2] = 0.0;
  this->m_pCurMap = 0LL;
  this->m_wMapLayerIndex = 0;
  this->m_dwNextFreeStunTime = -1;
  this->m_dwOldTickBreakTranspar = -1;
  this->m_bBreakTranspar = 0;
  this->m_bPlayerCircleList = 0LL;
  this->m_bObserver = 0;
  this->m_dwCurSec = 0;
  if ( ++CGameObject::s_nTotalObjectNum > 42642 )
  {
    MyMessageBox("error", "CGameObject::Init : Lack Object Num");
    ServerProgramExit("CGameObject::Init()", 0);
  }
}

bool CGameObject::IsAttackableInTown()
{
  return 0;
}

bool CGameObject::IsBeAttackedAble(bool bFirst)
{
  return 0;
}

char CGameObject::IsBeCirclePlayer(int nRange)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-78h] BYREF
  _pnt_rect pRect; // [rsp+28h] [rbp-50h] BYREF
  _sec_info *SecInfo; // [rsp+48h] [rbp-30h]
  int v8; // [rsp+50h] [rbp-28h]
  int j; // [rsp+54h] [rbp-24h]
  int k; // [rsp+58h] [rbp-20h]
  unsigned int dwSecIndex; // [rsp+5Ch] [rbp-1Ch]
  CObjectList *SectorListPlayer; // [rsp+60h] [rbp-18h]

  v2 = &v5;
  for ( i = 28LL; i; --i )
  {
    *(_DWORD *)v2 = -858993460;
    v2 = (__int64 *)((char *)v2 + 4);
  }
  if ( this->m_bPlayerCircleList )
    return 1;
  SecInfo = CMapData::GetSecInfo(this->m_pCurMap);
  CMapData::GetRectInRadius(this->m_pCurMap, &pRect, nRange, this->m_dwCurSec);
  v8 = 0;
  for ( j = pRect.nStarty; j <= pRect.nEndy; ++j )
  {
    for ( k = pRect.nStartx; k <= pRect.nEndx; ++k )
    {
      dwSecIndex = SecInfo->m_nSecNumW * j + k;
      SectorListPlayer = CMapData::GetSectorListPlayer(this->m_pCurMap, this->m_wMapLayerIndex, dwSecIndex);
      if ( SectorListPlayer && SectorListPlayer->m_nSize > 0 )
        return 1;
    }
  }
  return 0;
}

char CGameObject::IsBeDamagedAble(CCharacter *pAtter)
{
  return 1;
}

char CGameObject::IsCircleObject(int nRange, CGameObject *pObject)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-78h] BYREF
  _pnt_rect pRect; // [rsp+28h] [rbp-50h] BYREF
  _sec_info *SecInfo; // [rsp+48h] [rbp-30h]
  int j; // [rsp+50h] [rbp-28h]
  int k; // [rsp+54h] [rbp-24h]
  unsigned int dwSecIndex; // [rsp+58h] [rbp-20h]
  CObjectList *SectorListObj; // [rsp+60h] [rbp-18h]
  _object_list_point *node; // [rsp+68h] [rbp-10h]

  v3 = &v6;
  for ( i = 28LL; i; --i )
  {
    *(_DWORD *)v3 = -858993460;
    v3 = (__int64 *)((char *)v3 + 4);
  }
  SecInfo = CMapData::GetSecInfo(this->m_pCurMap);
  CMapData::GetRectInRadius(this->m_pCurMap, &pRect, nRange, this->m_dwCurSec);
  for ( j = pRect.nStarty; j <= pRect.nEndy; ++j )
  {
    for ( k = pRect.nStartx; k <= pRect.nEndx; ++k )
    {
      dwSecIndex = SecInfo->m_nSecNumW * j + k;
      SectorListObj = CMapData::GetSectorListObj(this->m_pCurMap, this->m_wMapLayerIndex, dwSecIndex);
      if ( SectorListObj )
      {
        for ( node = SectorListObj->m_Head.m_pNext; node != &SectorListObj->m_Tail; node = node->m_pNext )
        {
          if ( pObject == node->m_pItem )
            return 1;
        }
      }
    }
  }
  return 0;
}

bool CGameObject::IsInTown()
{
  return 0;
}

char CGameObject::IsRecvableContEffect()
{
  return 1;
}

bool CGameObject::IsRewardExp()
{
  return 0;
}

bool CGameObject::Is_Battle_Mode()
{
  return 0;
}

void CGameObject::Loop()
{
  ;
}

void CGameObject::OnLoop()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  _BOOL8 v3; // rdx
  int UseSectorRange; // eax
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  unsigned int LoopTime; // [rsp+20h] [rbp-18h]

  v1 = &v5;
  for ( i = 12LL; i; --i )
  {
    *(_DWORD *)v1 = -858993460;
    v1 = (__int64 *)((char *)v1 + 4);
  }
  LoopTime = GetLoopTime();
  if ( this->m_pCurMap && CMapData::IsMapIn(this->m_pCurMap, this->m_fCurPos) )
  {
    if ( this->m_bStun && LoopTime > this->m_dwNextFreeStunTime )
      this->SetStun(0);
    if ( this->m_bBreakTranspar && LoopTime - this->m_dwOldTickBreakTranspar > 0x7530 )
      CGameObject::SetBreakTranspar(0);
    if ( !this->m_bMapLoading && this->m_dwCurSec != -1 )
      CGameObject::UpdateSecList();
    CGameObject::_ResetCirclePlayer();
    this->Loop();
    if ( LoopTime - this->m_dwLastSendTime > 0xFA0 )
    {
      this->m_dwLastSendTime = GetLoopTime();
      LOBYTE(v3) = 1;
      this->SendMsg_RealFixPosition(v3);
    }
    if ( !this->m_ObjID.m_byKind && !this->m_ObjID.m_byID )
    {
      this->SendMsg_RealFixPosition(0);
      UseSectorRange = CGameObject::GetUseSectorRange();
      this->m_nCirclePlayerNum = CGameObject::CalcCirclePlayerNum(UseSectorRange);
    }
  }
  else
  {
    this->OutOfSec();
  }
}

void CGameObject::OutOfSec()
{
  ;
}

void CGameObject::RecvKillMessage(CCharacter *pDier)
{
  ;
}

__int64 CGameObject::RerangeSecIndex(unsigned int dwOld, unsigned int dwNew)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  _sec_info *v6; // rax
  __int64 v7; // [rsp+0h] [rbp-68h] BYREF
  unsigned int v8; // [rsp+20h] [rbp-48h]
  unsigned int v9; // [rsp+24h] [rbp-44h]
  unsigned int v10; // [rsp+28h] [rbp-40h]
  unsigned int v11; // [rsp+2Ch] [rbp-3Ch]
  int v12; // [rsp+30h] [rbp-38h]
  int v13; // [rsp+34h] [rbp-34h]
  _sec_info *SecInfo; // [rsp+38h] [rbp-30h]
  _sec_info *v15; // [rsp+40h] [rbp-28h]
  _sec_info *v16; // [rsp+48h] [rbp-20h]
  _sec_info *v17; // [rsp+50h] [rbp-18h]

  v3 = &v7;
  for ( i = 24LL; i; --i )
  {
    *(_DWORD *)v3 = -858993460;
    v3 = (__int64 *)((char *)v3 + 4);
  }
  if ( dwOld == -1 )
    return 0xFFFFFFFFLL;
  SecInfo = CMapData::GetSecInfo(this->m_pCurMap);
  v8 = dwOld % SecInfo->m_nSecNumW;
  v15 = CMapData::GetSecInfo(this->m_pCurMap);
  v9 = dwOld / v15->m_nSecNumW;
  v16 = CMapData::GetSecInfo(this->m_pCurMap);
  v10 = dwNew % v16->m_nSecNumW;
  v17 = CMapData::GetSecInfo(this->m_pCurMap);
  v11 = dwNew / v17->m_nSecNumW;
  v12 = v10 - v8;
  v13 = v11 - v9;
  if ( (int)(v10 - v8) <= 1 )
  {
    if ( v12 < -1 )
      v10 = v8 - 1;
  }
  else
  {
    v10 = v8 + 1;
  }
  if ( v13 <= 1 )
  {
    if ( v13 < -1 )
      v11 = v9 - 1;
  }
  else
  {
    v11 = v9 + 1;
  }
  v6 = CMapData::GetSecInfo(this->m_pCurMap);
  return v6->m_nSecNumW * v11 + v10;
}

void CGameObject::ResetSector(unsigned int dwOldSec, unsigned int dwNewSec)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  int UseSectorRange; // eax
  unsigned int CurSecNum; // eax
  unsigned int v7; // eax
  int m_wIndex; // eax
  int v9; // eax
  int v10; // eax
  __int64 v11; // [rsp+0h] [rbp-C8h] BYREF
  _sec_info *SecInfo; // [rsp+20h] [rbp-A8h]
  int v13; // [rsp+28h] [rbp-A0h]
  int v14; // [rsp+2Ch] [rbp-9Ch]
  _pnt_rect pRect; // [rsp+38h] [rbp-90h] BYREF
  int v16; // [rsp+54h] [rbp-74h]
  int v17; // [rsp+58h] [rbp-70h]
  int j; // [rsp+5Ch] [rbp-6Ch]
  int k; // [rsp+60h] [rbp-68h]
  bool v20; // [rsp+64h] [rbp-64h]
  unsigned int dwSecIndex; // [rsp+68h] [rbp-60h]
  CObjectList *SectorListObj; // [rsp+70h] [rbp-58h]
  _object_list_point *m_pNext; // [rsp+78h] [rbp-50h]
  CGameObject *v24; // [rsp+80h] [rbp-48h]
  unsigned int v25; // [rsp+88h] [rbp-40h]
  unsigned int v26; // [rsp+8Ch] [rbp-3Ch]
  int nSecNum; // [rsp+90h] [rbp-38h]
  CGameObject_vtbl *v28; // [rsp+98h] [rbp-30h]
  CGameObject_vtbl *v29; // [rsp+A0h] [rbp-28h]
  CGameObject_vtbl *v30; // [rsp+A8h] [rbp-20h]
  CGameObject_vtbl *v31; // [rsp+B0h] [rbp-18h]

  v3 = &v11;
  for ( i = 48LL; i; --i )
  {
    *(_DWORD *)v3 = -858993460;
    v3 = (__int64 *)((char *)v3 + 4);
  }
  if ( !this->m_bMaxVision )
  {
    SecInfo = CMapData::GetSecInfo(this->m_pCurMap);
    v25 = dwNewSec % SecInfo->m_nSecNumW;
    v13 = v25 - dwOldSec % SecInfo->m_nSecNumW;
    v26 = dwNewSec / SecInfo->m_nSecNumW;
    v14 = v26 - dwOldSec / SecInfo->m_nSecNumW;
    nSecNum = CGameObject::GetCurSecNum();
    UseSectorRange = CGameObject::GetUseSectorRange();
    CMapData::GetRectInRadius(this->m_pCurMap, &pRect, UseSectorRange, nSecNum);
    CurSecNum = CGameObject::GetCurSecNum();
    v16 = CurSecNum % SecInfo->m_nSecNumW;
    v7 = CGameObject::GetCurSecNum();
    v17 = v7 / SecInfo->m_nSecNumW;
    for ( j = pRect.nStarty; j <= pRect.nEndy; ++j )
    {
      for ( k = pRect.nStartx; k <= pRect.nEndx; ++k )
      {
        v20 = 0;
        if ( v13 <= 0 )
        {
          if ( v13 < 0 )
            v20 = k == pRect.nStartx;
        }
        else
        {
          v20 = k == pRect.nEndx;
        }
        if ( !v20 )
        {
          if ( v14 <= 0 )
          {
            if ( v14 < 0 )
              v20 = j == pRect.nStarty;
          }
          else
          {
            v20 = j == pRect.nEndy;
          }
        }
        if ( v20 )
        {
          dwSecIndex = SecInfo->m_nSecNumW * j + k;
          SectorListObj = CMapData::GetSectorListObj(this->m_pCurMap, this->m_wMapLayerIndex, dwSecIndex);
          if ( SectorListObj )
          {
            m_pNext = SectorListObj->m_Head.m_pNext;
            while ( m_pNext != &SectorListObj->m_Tail )
            {
              v24 = m_pNext->m_pItem;
              m_pNext = m_pNext->m_pNext;
              if ( v24 != this && !v24->m_bMaxVision )
              {
                if ( !v24->m_ObjID.m_byKind && !v24->m_ObjID.m_byID )
                {
                  m_wIndex = v24->m_ObjID.m_wIndex;
                  if ( this->m_bMove )
                    this->SendMsg_RealMovePoint(m_wIndex);
                  else
                    this->SendMsg_FixPosition(m_wIndex);
                }
                if ( !this->m_ObjID.m_byKind && !this->m_ObjID.m_byID )
                {
                  if ( v24->m_bMove )
                  {
                    v9 = this->m_ObjID.m_wIndex;
                    v24->SendMsg_RealMovePoint(v9);
                  }
                  else
                  {
                    v10 = this->m_ObjID.m_wIndex;
                    v24->SendMsg_FixPosition(v10);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

bool CGameObject::RobbedHP(CCharacter *pDst, int nDecHP)
{
  return 0;
}

void CGameObject::SFContDelMessage(unsigned __int8 byContCode, unsigned __int8 byListIndex, bool bSend)
{
  ;
}

void CGameObject::SFContInsertMessage(unsigned __int8 byContCode, unsigned __int8 byListIndex, bool bAura)
{
  ;
}

void CGameObject::SFContUpdateTimeMessage(unsigned __int8 byContCode, unsigned __int8 byListIndex, int nLeftTime)
{
  ;
}

bool CGameObject::SF_AllContDamageForceRemove_Once(CCharacter *pDstObj)
{
  return 0;
}

bool CGameObject::SF_AllContHelpForceRemove_Once(CCharacter *pDstObj)
{
  return 0;
}

bool CGameObject::SF_AllContHelpSkillRemove_Once(CCharacter *pDstObj)
{
  return 0;
}

bool CGameObject::SF_AttHPtoDstFP_Once(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_ContDamageTimeInc_Once(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_ContHelpTimeInc_Once(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_ConvertMonsterTarget(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_ConvertTargetDest(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_DamageAndStun(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_FPDec(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_HPInc_Once(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_IncHPCircleParty(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_IncreaseDP(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_LateContDamageRemove_Once(CCharacter *pDstObj)
{
  return 0;
}

bool CGameObject::SF_LateContHelpForceRemove_Once(CCharacter *pDstObj)
{
  return 0;
}

bool CGameObject::SF_LateContHelpSkillRemove_Once(CCharacter *pDstObj)
{
  return 0;
}

bool CGameObject::SF_MakePortalReturnBindPositionPartyMember(CCharacter *pDstObj, float fEffectValue, unsigned __int8 *byRet)
{
  return 0;
}

bool CGameObject::SF_MakeZeroAnimusRecallTimeOnce(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_OthersContHelpSFRemove_Once(float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_OverHealing_Once(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_RecoverAllReturnStateAnimusHPFull(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_ReleaseMonsterTarget(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_RemoveAllContHelp_Once(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_Resurrect_Once(CCharacter *pDstObj)
{
  return 0;
}

bool CGameObject::SF_ReturnBindPosition(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_SPDec(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_STInc_Once(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_SelfDestruction(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_SkillContHelpTimeInc_Once(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_Stun(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

bool CGameObject::SF_TeleportToDestination(CCharacter *pDstObj, bool bStone)
{
  return 0;
}

bool CGameObject::SF_TransDestHP(CCharacter *pDstObj, float fEffectValue, unsigned __int8 *byRet)
{
  return 0;
}

bool CGameObject::SF_TransMonsterHP(CCharacter *pDstObj, float fEffectValue)
{
  return 0;
}

void CGameObject::SendMsg_BreakStop()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v3; // [rsp+0h] [rbp-98h] BYREF
  char szMsg; // [rsp+38h] [rbp-60h] BYREF
  unsigned int m_dwObjSerial; // [rsp+39h] [rbp-5Fh]
  __int16 pShort[19]; // [rsp+3Dh] [rbp-5Bh] BYREF
  unsigned __int8 pbyType[28]; // [rsp+64h] [rbp-34h] BYREF

  v1 = &v3;
  for ( i = 36LL; i; --i )
  {
    *(_DWORD *)v1 = -858993460;
    v1 = (__int64 *)((char *)v1 + 4);
  }
  szMsg = this->m_ObjID.m_byID;
  m_dwObjSerial = this->m_dwObjSerial;
  FloatToShort(this->m_fCurPos, pShort, 3);
  pbyType[0] = 4;
  pbyType[1] = 30;
  CGameObject::CircleReport(pbyType, &szMsg, 11, 0);
}

void CGameObject::SendMsg_FixPosition(int n)
{
  ;
}

void CGameObject::SendMsg_RealFixPosition(bool bCircle)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-78h] BYREF
  char szMsg[2]; // [rsp+38h] [rbp-40h] BYREF
  unsigned __int16 m_wIndex; // [rsp+3Ah] [rbp-3Eh]
  unsigned int m_dwObjSerial; // [rsp+3Ch] [rbp-3Ch]
  unsigned __int8 pbyType[36]; // [rsp+54h] [rbp-24h] BYREF

  v2 = &v4;
  for ( i = 28LL; i; --i )
  {
    *(_DWORD *)v2 = -858993460;
    v2 = (__int64 *)((char *)v2 + 4);
  }
  szMsg[0] = this->m_ObjID.m_byKind;
  szMsg[1] = this->m_ObjID.m_byID;
  m_wIndex = this->m_ObjID.m_wIndex;
  m_dwObjSerial = this->m_dwObjSerial;
  pbyType[0] = 4;
  pbyType[1] = 10;
  if ( bCircle )
    CGameObject::CircleReport(pbyType, szMsg, 8, 0);
  else
    g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, 8u);
}

void CGameObject::SendMsg_RealMovePoint(int n)
{
  ;
}

void CGameObject::SendMsg_SetHPInform()
{
  ;
}

void CGameObject::SendMsg_StunInform()
{
  ;
}

void CGameObject::SetAttackPart(int nAttactPart)
{
  ;
}

void CGameObject::SetBreakTranspar(bool bBreak)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  v2 = &v4;
  for ( i = 8LL; i; --i )
  {
    *(_DWORD *)v2 = -858993460;
    v2 = (__int64 *)((char *)v2 + 4);
  }
  this->m_bBreakTranspar = bBreak;
  if ( this->m_bBreakTranspar )
    this->m_dwOldTickBreakTranspar = GetLoopTime();
}

char CGameObject::SetCurBspMap(CMapData *pMap)
{
  if ( this->m_pCurMap == pMap )
    return 0;
  this->m_pCurMap = pMap;
  return 1;
}

char CGameObject::SetCurPos(float *pPos)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  v2 = &v5;
  for ( i = 8LL; i; --i )
  {
    *(_DWORD *)v2 = -858993460;
    v2 = (__int64 *)((char *)v2 + 4);
  }
  if ( !CMapData::IsMapIn(this->m_pCurMap, pPos) )
    return 0;
  memcpy_0(this->m_fCurPos, pPos, sizeof(this->m_fCurPos));
  return 1;
}

void CGameObject::SetCurSecNum(unsigned int dwNewSecNum)
{
  this->m_dwCurSec = dwNewSecNum;
}

__int64 CGameObject::SetDamage(int nDam, CCharacter *pDst, int nDstLv)
{
  return 0LL;
}

char CGameObject::SetHP(int nHP, bool bOver)
{
  return 1;
}

void CGameObject::SetMaxVersion()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v3; // [rsp+0h] [rbp-38h] BYREF
  bool *v4; // [rsp+20h] [rbp-18h]

  v1 = &v3;
  for ( i = 12LL; i; --i )
  {
    *(_DWORD *)v1 = -858993460;
    v1 = (__int64 *)((char *)v1 + 4);
  }
  this->m_bMaxVision = 1;
  if ( !this->m_bPlayerCircleList )
  {
    v4 = (bool *)operator new[](0x9E4uLL);
    this->m_bPlayerCircleList = v4;
  }
  CGameObject::_ResetCirclePlayer();
}

void CGameObject::SetStun(bool bStun)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  v2 = &v4;
  for ( i = 8LL; i; --i )
  {
    *(_DWORD *)v2 = -858993460;
    v2 = (__int64 *)((char *)v2 + 4);
  }
  this->m_bStun = bStun;
  if ( this->m_bStun )
    this->m_dwNextFreeStunTime = GetLoopTime() + 1000;
}

char CGameObject::UpdateSecList()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
  unsigned int dwNew; // [rsp+20h] [rbp-18h]
  unsigned int dwOldSec; // [rsp+24h] [rbp-14h]

  v1 = &v4;
  for ( i = 12LL; i; --i )
  {
    *(_DWORD *)v1 = -858993460;
    v1 = (__int64 *)((char *)v1 + 4);
  }
  dwNew = CGameObject::CalcSecIndex();
  if ( this->m_dwCurSec == dwNew )
    return 1;
  dwOldSec = this->m_dwCurSec;
  if ( !this->m_ObjID.m_byKind && !this->m_ObjID.m_byID )
    dwNew = CGameObject::RerangeSecIndex(this->m_dwCurSec, dwNew);
  if ( !CMapData::UpdateSecterList(this->m_pCurMap, this, this->m_dwCurSec, dwNew) )
    return 0;
  CGameObject::SetCurSecNum(dwNew);
  CGameObject::ResetSector(dwOldSec, dwNew);
  this->AlterSec();
  return 1;
}

CGameObject::~CGameObject()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v3; // [rsp+0h] [rbp-38h] BYREF
  void *m_bPlayerCircleList; // [rsp+20h] [rbp-18h]

  v1 = &v3;
  for ( i = 12LL; i; --i )
  {
    *(_DWORD *)v1 = -858993460;
    v1 = (__int64 *)((char *)v1 + 4);
  }
  if ( this->m_bPlayerCircleList )
  {
    m_bPlayerCircleList = this->m_bPlayerCircleList;
    operator delete[](m_bPlayerCircleList);
    this->m_bPlayerCircleList = 0LL;
  }
}

void CGameObject::_ResetCirclePlayer()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int CurSecNum; // eax
  _sec_info *SecInfo; // rax
  int m_wIndex; // eax
  __int64 v6; // [rsp+0h] [rbp-AA8h] BYREF
  _BYTE v7[2568]; // [rsp+30h] [rbp-A78h] BYREF
  _pnt_rect pRect; // [rsp+A38h] [rbp-70h] BYREF
  int j; // [rsp+A54h] [rbp-54h]
  int k; // [rsp+A58h] [rbp-50h]
  unsigned int dwSecIndex; // [rsp+A5Ch] [rbp-4Ch]
  CObjectList *SectorListPlayer; // [rsp+A60h] [rbp-48h]
  _object_list_point *m_pNext; // [rsp+A68h] [rbp-40h]
  CGameObject *m_pItem; // [rsp+A70h] [rbp-38h]
  CGameObject_vtbl *v15; // [rsp+A80h] [rbp-28h]
  CGameObject_vtbl *v16; // [rsp+A88h] [rbp-20h]

  v1 = &v6;
  for ( i = 680LL; i; --i )
  {
    *(_DWORD *)v1 = -858993460;
    v1 = (__int64 *)((char *)v1 + 4);
  }
  if ( this->m_bPlayerCircleList )
  {
    memcpy_0(v7, this->m_bPlayerCircleList, 0x9E4uLL);
    memset_0(this->m_bPlayerCircleList, 0, 0x9E4uLL);
    CurSecNum = CGameObject::GetCurSecNum();
    CMapData::GetRectInRadius(this->m_pCurMap, &pRect, 11, CurSecNum);
    for ( j = pRect.nStarty; j <= pRect.nEndy; ++j )
    {
      for ( k = pRect.nStartx; k <= pRect.nEndx; ++k )
      {
        SecInfo = CMapData::GetSecInfo(this->m_pCurMap);
        dwSecIndex = SecInfo->m_nSecNumW * j + k;
        SectorListPlayer = CMapData::GetSectorListPlayer(this->m_pCurMap, this->m_wMapLayerIndex, dwSecIndex);
        if ( SectorListPlayer )
        {
          m_pNext = SectorListPlayer->m_Head.m_pNext;
          while ( m_pNext != &SectorListPlayer->m_Tail )
          {
            m_pItem = m_pNext->m_pItem;
            m_pNext = m_pNext->m_pNext;
            if ( !v7[m_pItem->m_ObjID.m_wIndex] )
            {
              m_wIndex = m_pItem->m_ObjID.m_wIndex;
              if ( this->m_bMove )
                this->SendMsg_RealMovePoint(m_wIndex);
              else
                this->SendMsg_FixPosition(m_wIndex);
            }
            this->m_bPlayerCircleList[m_pItem->m_ObjID.m_wIndex] = 1;
          }
        }
      }
    }
  }
}
*/

void CGameObject::CalcAbsPos()
{
  _bsp_info *BspInfo = this->m_pCurMap->GetBspInfo();
  this->m_fAbsPos[0] = static_cast<float>(-BspInfo->m_nMapMinSize[0]) + this->m_fCurPos[0];
  this->m_fAbsPos[1] = static_cast<float>(BspInfo->m_nMapMaxSize[1]) - this->m_fCurPos[1];
  this->m_fAbsPos[2] = static_cast<float>(BspInfo->m_nMapMaxSize[2]) - this->m_fCurPos[2];
}

__int64 CGameObject::CalcSecIndex()
{
  this->CalcAbsPos();
  unsigned int secX = static_cast<unsigned int>(static_cast<int>(this->m_fAbsPos[0] / 100.0f));
  unsigned int secZ = static_cast<unsigned int>(static_cast<int>(this->m_fAbsPos[2] / 100.0f));
  _sec_info *SecInfo = this->m_pCurMap->GetSecInfo();
  if (secX < SecInfo->m_nSecNumW && secZ < SecInfo->m_nSecNumH)
    return SecInfo->m_nSecNumW * secZ + secX;
  g_Main.m_logSystemError.Write(
    "kind(%d), id(%d).. Out of Sector",
    this->m_ObjID.m_byKind,
    this->m_ObjID.m_byID);
  return this->m_dwCurSec;
}

__int64 CGameObject::GetUseSectorRange()
{
  return static_cast<unsigned int>(this->m_pCurMap->m_pMapSet->m_nRadius);
}

bool CGameObject::IsInTown()
{
  return false;
}

void CGameObject::CircleReport(unsigned __int8 *pbyType, char *szMsg, unsigned __int16 nMsgSize, bool bToOne)
{
  if (this->m_bPlayerCircleList)
  {
    for (unsigned int dwClientIndex = 0; static_cast<int>(dwClientIndex) < 2532; ++dwClientIndex)
    {
      if (this->m_bPlayerCircleList[dwClientIndex])
        g_Network.m_pProcess[0]->LoadSendMsg(dwClientIndex, pbyType, szMsg, nMsgSize);
    }
    return;
  }

  if (bToOne && !this->m_ObjID.m_byKind && !this->m_ObjID.m_byID)
    g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, nMsgSize);

  if (this->m_pCurMap && this->m_dwCurSec != static_cast<unsigned int>(-1))
  {
    _sec_info *SecInfo = this->m_pCurMap->GetSecInfo();
    int nRadius = static_cast<int>(this->GetUseSectorRange());
    _pnt_rect pRect;
    this->m_pCurMap->GetRectInRadius(&pRect, nRadius, this->m_dwCurSec);
    for (int j = pRect.nStarty; j <= pRect.nEndy; ++j)
    {
      for (int k = pRect.nStartx; k <= pRect.nEndx; ++k)
      {
        unsigned int dwSecIndex = SecInfo->m_nSecNumW * j + k;
        CObjectList *SectorListPlayer =
          this->m_pCurMap->GetSectorListPlayer(this->m_wMapLayerIndex, dwSecIndex);
        if (SectorListPlayer)
        {
          _object_list_point *m_pNext = SectorListPlayer->m_Head.m_pNext;
          while (m_pNext != &SectorListPlayer->m_Tail)
          {
            CGameObject *m_pItem = m_pNext->m_pItem;
            m_pNext = m_pNext->m_pNext;
            _object_id *p_m_ObjID = &m_pItem->m_ObjID;
            if (m_pItem != this && (!this->m_bObserver || BYTE4(m_pItem[9].m_SectorNetPoint.m_pNext)))
              g_Network.m_pProcess[0]->LoadSendMsg(p_m_ObjID->m_wIndex, pbyType, szMsg, nMsgSize);
          }
        }
      }
    }
  }
}

void CGameObject::CircleReport(
  unsigned __int8 *pbyType,
  char *szMsg,
  unsigned __int16 nMsgSize,
  unsigned int dwPassObjSerial,
  bool bToOne)
{
  if (this->m_bPlayerCircleList)
  {
    for (unsigned int dwClientIndex = 0; static_cast<int>(dwClientIndex) < 2532; ++dwClientIndex)
    {
      if (this->m_bPlayerCircleList[dwClientIndex])
        g_Network.m_pProcess[0]->LoadSendMsg(dwClientIndex, pbyType, szMsg, nMsgSize);
    }
    return;
  }

  if (bToOne && !this->m_ObjID.m_byKind && !this->m_ObjID.m_byID)
    g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, nMsgSize);

  if (this->m_pCurMap && this->m_dwCurSec != static_cast<unsigned int>(-1))
  {
    _sec_info *SecInfo = this->m_pCurMap->GetSecInfo();
    int nRadius = static_cast<int>(this->GetUseSectorRange());
    _pnt_rect pRect;
    this->m_pCurMap->GetRectInRadius(&pRect, nRadius, this->m_dwCurSec);
    for (int j = pRect.nStarty; j <= pRect.nEndy; ++j)
    {
      for (int k = pRect.nStartx; k <= pRect.nEndx; ++k)
      {
        unsigned int dwSecIndex = SecInfo->m_nSecNumW * j + k;
        CObjectList *SectorListPlayer =
          this->m_pCurMap->GetSectorListPlayer(this->m_wMapLayerIndex, dwSecIndex);
        if (SectorListPlayer)
        {
          _object_list_point *m_pNext = SectorListPlayer->m_Head.m_pNext;
          while (m_pNext != &SectorListPlayer->m_Tail)
          {
            CGameObject *m_pItem = m_pNext->m_pItem;
            m_pNext = m_pNext->m_pNext;
            _object_id *p_m_ObjID = &m_pItem->m_ObjID;
            if (m_pItem != this
              && m_pItem->m_dwObjSerial != dwPassObjSerial
              && (!this->m_bObserver || BYTE4(m_pItem[9].m_SectorNetPoint.m_pNext)))
            {
              g_Network.m_pProcess[0]->LoadSendMsg(p_m_ObjID->m_wIndex, pbyType, szMsg, nMsgSize);
            }
          }
        }
      }
    }
  }
}

char CGameObject::Create(_object_create_setdata *pData)
{
  if (this->m_bLive)
    return 0;

  this->m_pCurMap = pData->m_pMap;
  this->m_wMapLayerIndex = pData->m_nLayerIndex;
  memcpy_0(this->m_fCurPos, pData->m_fStartPos, sizeof(this->m_fCurPos));
  memcpy_0(this->m_fOldPos, pData->m_fStartPos, sizeof(this->m_fOldPos));
  this->m_bMaxVision = 0;
  this->m_bObserver = 0;
  this->m_nCirclePlayerNum = 0;

  unsigned int dwSecIndex = static_cast<unsigned int>(this->CalcSecIndex());
  _sec_info *SecInfo = pData->m_pMap->GetSecInfo();
  if (dwSecIndex < SecInfo->m_nSecNum)
  {
    pData->m_pMap->EnterMap(this, dwSecIndex);
    this->m_bLive = 1;
    this->m_pRecordSet = pData->m_pRecordSet;
    this->m_dwCurSec = dwSecIndex;
    this->m_bMove = 0;
    this->m_bCorpse = 0;
    this->m_bMapLoading = 0;
    this->m_bBreakTranspar = 0;
    this->m_dwLastSendTime = GetLoopTime();
    this->m_dwOldTickBreakTranspar = GetLoopTime();
    return 1;
  }

  this->m_pCurMap = nullptr;
  g_Main.m_logSystemError.Write(
    "CGameObject::Create() : dwSec >= MAX(this[%d-%d-%d])",
    this->m_ObjID.m_byKind,
    this->m_ObjID.m_byID,
    this->m_ObjID.m_wIndex);
  return 0;
}

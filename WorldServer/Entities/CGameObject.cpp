#include "pch.h"

#include "CGameObject.h"

#include "CMapData.h"
#include "CNetworkEX.h"
#include "CPlayer.h"
#include "CTrap.h"
#include "GlobalObjects.h"
#include "Packet/ZoneClientPacket.h"
#include "WorldServerUtil.h"
#include "pnt_rect.h"

#include <cstdlib>
#include <ctime>

namespace
{
bool CanSendCircleReportToPlayer(
  CGameObject *sourceObject,
  CPlayer *targetPlayer,
  unsigned __int16 nMsgSize,
  bool bToOne)
{
  if (sourceObject->m_bObserver && !targetPlayer->m_byUserDgr)
  {
    return false;
  }

  if (!sourceObject->m_ObjID.m_byKind)
  {
    if (!sourceObject->m_ObjID.m_byID)
    {
      CPlayer *sourcePlayer = static_cast<CPlayer *>(sourceObject);
      // Yorozuya fix implementation (non-IDA): hide stealth/invisible players unless the
      // viewer has the detect effect.
      if (sourcePlayer->GetStealth(true) && targetPlayer->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_22) <= 0.0f)
      {
        return false;
      }
    }
    else if (sourceObject->m_ObjID.m_byID == 7)
    {
      const CTrap *trap = static_cast<const CTrap *>(sourceObject);
      if (trap->m_dwMasterSerial != targetPlayer->m_Param.GetCharSerial())
      {
        if (!(nMsgSize == 5 && !bToOne))
        {
          // Yorozuya fix implementation (non-IDA): hide traps unless detection is active.
          if (!targetPlayer->m_EP.GetEff_State(EFF_STATE_TRAP_DETECTION))
          {
            return false;
          }
        }
      }
    }
  }

  return true;
}
}

bool _100_per_random_table::s_bRecordSet;
unsigned __int16 _100_per_random_table::s_wRecord[10][100];

CGameObject *CGameObject::s_pSelectObject;
CGameObject *CGameObject::s_pTotalObject[MAX_TOTAL_CGAMEOBJECT];
int CGameObject::s_nTotalObjectNum;

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
    const unsigned int seed = static_cast<unsigned int>(time(nullptr));
    srand(seed);
    for (int j = 0; j < 10; ++j)
    {
      for (int k = 0; k < 100; ++k)
      {
        s_wRecord[j][k] = static_cast<unsigned __int16>(k);
      }
    }
    reset();
  }
  m_wCurTable = static_cast<unsigned __int16>(rand() % 10);
  m_wCurPoint = 0;
}

unsigned __int16 _100_per_random_table::GetRand()
{
  if (m_wCurPoint >= 100)
  {
    m_wCurTable = static_cast<unsigned __int16>(rand() % 10);
    m_wCurPoint = 0;
  }
  unsigned __int16 value = s_wRecord[m_wCurTable][m_wCurPoint++];
  return static_cast<unsigned __int16>(value % 100);
}

void _100_per_random_table::reset()
{
  for (int j = 0; j < 10; ++j)
  {
    for (int k = 0; k < 100; ++k)
    {
      const unsigned __int16 a = static_cast<unsigned __int16>(rand() % 100);
      const unsigned __int16 b = static_cast<unsigned __int16>(rand() % 100);
      const unsigned __int16 temp = s_wRecord[j][b];
      s_wRecord[j][b] = s_wRecord[j][a];
      s_wRecord[j][a] = temp;
    }
  }
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
  if (++s_nTotalObjectNum > MAX_TOTAL_CGAMEOBJECT)
  {
    MyMessageBox("error", "CGameObject::Init : Lack Object Num");
    ServerProgramExit("CGameObject::Init()", 0);
  }
}

void CGameObject::SFContDelMessage(
  unsigned __int8 /*byContCode*/,
  unsigned __int8 /*byListIndex*/,
  bool /*bSend*/,
  bool /*bAura*/)
{
  // this is not a stub
}

void CGameObject::SFContInsertMessage(unsigned __int8 /*byContCode*/, unsigned __int8 /*byListIndex*/, bool /*bAura*/)
{
  // this is not a stub
}

void CGameObject::SFContUpdateTimeMessage(unsigned __int8 /*byContCode*/, unsigned __int8 /*byListIndex*/, int /*nLeftTime*/)
{
  // this is not a stub
}

bool CGameObject::SF_AllContDamageForceRemove_Once(CCharacter * /*pDstObj*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_AllContHelpForceRemove_Once(CCharacter * /*pDstObj*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_AllContHelpSkillRemove_Once(CCharacter * /*pDstObj*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_AttHPtoDstFP_Once(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_ContDamageTimeInc_Once(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_ContHelpTimeInc_Once(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_ConvertMonsterTarget(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_ConvertTargetDest(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_DamageAndStun(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_FPDec(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_HPInc_Once(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_IncHPCircleParty(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_IncreaseDP(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_LateContDamageRemove_Once(CCharacter * /*pDstObj*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_LateContHelpForceRemove_Once(CCharacter * /*pDstObj*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_LateContHelpSkillRemove_Once(CCharacter * /*pDstObj*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_MakePortalReturnBindPositionPartyMember(
  CCharacter * /*pDstObj*/,
  float /*fEffectValue*/,
  unsigned __int8 * /*byRet*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_MakeZeroAnimusRecallTimeOnce(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_OthersContHelpSFRemove_Once(float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_OverHealing_Once(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_RecoverAllReturnStateAnimusHPFull(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_ReleaseMonsterTarget(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_RemoveAllContHelp_Once(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_Resurrect_Once(CCharacter * /*pDstObj*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_ReturnBindPosition(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_SPDec(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_STInc_Once(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_SelfDestruction(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_SkillContHelpTimeInc_Once(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_Stun(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_TeleportToDestination(CCharacter * /*pDstObj*/, bool /*bStone*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_TransDestHP(CCharacter * /*pDstObj*/, float /*fEffectValue*/, unsigned __int8 * /*byRet*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::SF_TransMonsterHP(CCharacter * /*pDstObj*/, float /*fEffectValue*/)
{
  // this is not a stub
  return false;
}

bool CGameObject::IsAttackableInTown()
{
  // this is not a stub
  return false;
}

char *CGameObject::GetObjName()
{
  // this is not a stub
  return nullptr;
}

bool CGameObject::RobbedHP(CCharacter * /*pDst*/, int /*nDecHP*/)
{
  // this is not a stub
  return false;
}

void CGameObject::CalcScrExtendPoint(CRect *prcWnd, CRect *prcExtend)
{
  const float localX = m_fAbsPos[0] - static_cast<float>(prcExtend->left);
  const float localY = m_fAbsPos[2] - static_cast<float>(prcExtend->top);

  m_nScreenPos[0] = static_cast<int>(
    (localX * static_cast<float>(prcWnd->right))
    / static_cast<float>(prcExtend->right - prcExtend->left));
  m_nScreenPos[1] = static_cast<int>(
    (localY * static_cast<float>(prcWnd->bottom))
    / static_cast<float>(prcExtend->bottom - prcExtend->top));
}

void CGameObject::CalcScrNormalPoint(CRect *prcWnd)
{
  _bsp_info *bspInfo = m_pCurMap->GetBspInfo();
  m_nScreenPos[0] = prcWnd->right * static_cast<int>(m_fAbsPos[0]) / bspInfo->m_nMapSize[0];
  m_nScreenPos[1] = prcWnd->bottom * static_cast<int>(m_fAbsPos[2]) / bspInfo->m_nMapSize[2];
}

unsigned int CGameObject::GetCurSecNum()
{
  return m_dwCurSec;
}

bool CGameObject::IsBeAttackedAble(bool bFirst)
{
  // this is not a stub
  return false;
}

bool CGameObject::IsBeDamagedAble(CCharacter *pAtter)
{
  // this is not a stub
  return true;
}

bool CGameObject::IsRecvableContEffect()
{
  // this is not a stub
  return true;
}

bool CGameObject::IsRewardExp()
{
  // this is not a stub
  return false;
}

bool CGameObject::Is_Battle_Mode()
{
  // this is not a stub
  return false;
}

void CGameObject::Loop()
{
  // this is not a stub
}

void CGameObject::OnLoop()
{
  const unsigned int loopTime = GetLoopTime();
  if (m_pCurMap && m_pCurMap->IsMapIn(m_fCurPos))
  {
    if (m_bStun && loopTime > m_dwNextFreeStunTime)
    {
      SetStun(false);
    }

    if (m_bBreakTranspar && loopTime - m_dwOldTickBreakTranspar > 30000)
    {
      SetBreakTranspar(false);
    }

    if (!m_bMapLoading && m_dwCurSec != static_cast<unsigned int>(-1))
    {
      UpdateSecList();
    }

    _ResetCirclePlayer();
    Loop();

    if (loopTime - m_dwLastSendTime > 4000)
    {
      m_dwLastSendTime = GetLoopTime();
      SendMsg_RealFixPosition(true);
    }

    if (!m_ObjID.m_byKind && !m_ObjID.m_byID)
    {
      SendMsg_RealFixPosition(false);
      m_nCirclePlayerNum = CalcCirclePlayerNum(static_cast<int>(GetUseSectorRange()));
    }
  }
  else
  {
    OutOfSec();
  }
}

void CGameObject::AlterSec()
{
  // this is not a stub
}

int CGameObject::AttackableHeight()
{
  return 50;
}

void CGameObject::BeTargeted(CCharacter * /*pSeacher*/)
{
  // this is not a stub
}

void CGameObject::OutOfSec()
{
  // this is not a stub
}

void CGameObject::RecvKillMessage(CCharacter * /*pDier*/)
{
  // this is not a stub
}

int CGameObject::CalcCirclePlayerNum(int nRange)
{
  if (m_bPlayerCircleList)
  {
    return 1;
  }

  _sec_info *secInfo = m_pCurMap->GetSecInfo();
  _pnt_rect rect{};
  m_pCurMap->GetRectInRadius(&rect, nRange, m_dwCurSec);

  int count = 0;
  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = m_pCurMap->GetSectorListPlayer(m_wMapLayerIndex, secIndex);
      if (sectorList)
      {
        count += sectorList->m_nSize;
      }
    }
  }

  return count;
}

unsigned int CGameObject::RerangeSecIndex(unsigned int dwOld, unsigned int dwNew)
{
  if (dwOld == static_cast<unsigned int>(-1))
  {
    return static_cast<unsigned int>(-1);
  }

  _sec_info *secInfo = m_pCurMap->GetSecInfo();
  const unsigned int oldX = dwOld % secInfo->m_nSecNumW;
  const unsigned int oldY = dwOld / secInfo->m_nSecNumW;
  unsigned int newX = dwNew % secInfo->m_nSecNumW;
  unsigned int newY = dwNew / secInfo->m_nSecNumW;

  const int deltaX = static_cast<int>(newX) - static_cast<int>(oldX);
  const int deltaY = static_cast<int>(newY) - static_cast<int>(oldY);

  if (deltaX > 1)
  {
    newX = oldX + 1;
  }
  else if (deltaX < -1)
  {
    newX = oldX - 1;
  }

  if (deltaY > 1)
  {
    newY = oldY + 1;
  }
  else if (deltaY < -1)
  {
    newY = oldY - 1;
  }

  return secInfo->m_nSecNumW * newY + newX;
}

void CGameObject::ResetSector(unsigned int dwOldSec, unsigned int dwNewSec)
{
  if (m_bMaxVision)
  {
    return;
  }

  _sec_info *secInfo = m_pCurMap->GetSecInfo();
  const int deltaX = static_cast<int>(dwNewSec % secInfo->m_nSecNumW)
    - static_cast<int>(dwOldSec % secInfo->m_nSecNumW);
  const int deltaY = static_cast<int>(dwNewSec / secInfo->m_nSecNumW)
    - static_cast<int>(dwOldSec / secInfo->m_nSecNumW);

  const int currentSec = static_cast<int>(GetCurSecNum());
  const int useSectorRange = static_cast<int>(GetUseSectorRange());

  _pnt_rect rect{};
  m_pCurMap->GetRectInRadius(&rect, useSectorRange, currentSec);

  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      bool isBoundary = false;

      if (deltaX > 0)
      {
        isBoundary = (x == rect.nEndx);
      }
      else if (deltaX < 0)
      {
        isBoundary = (x == rect.nStartx);
      }

      if (!isBoundary)
      {
        if (deltaY > 0)
        {
          isBoundary = (y == rect.nEndy);
        }
        else if (deltaY < 0)
        {
          isBoundary = (y == rect.nStarty);
        }
      }

      if (!isBoundary)
      {
        continue;
      }

      const unsigned int sectorIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = m_pCurMap->GetSectorListObj(m_wMapLayerIndex, sectorIndex);
      if (!sectorList)
      {
        continue;
      }

      for (_object_list_point *node = sectorList->m_Head.m_pNext; node != &sectorList->m_Tail; node = node->m_pNext)
      {
        CGameObject *other = node->m_pItem;
        if (!other || other == this || other->m_bMaxVision)
        {
          continue;
        }

        if (!other->m_ObjID.m_byKind && !other->m_ObjID.m_byID)
        {
          if (m_bMove)
          {
            SendMsg_RealMovePoint(other->m_ObjID.m_wIndex);
          }
          else
          {
            SendMsg_FixPosition(other->m_ObjID.m_wIndex);
          }
        }

        if (!m_ObjID.m_byKind && !m_ObjID.m_byID)
        {
          if (other->m_bMove)
          {
            other->SendMsg_RealMovePoint(m_ObjID.m_wIndex);
          }
          else
          {
            other->SendMsg_FixPosition(m_ObjID.m_wIndex);
          }
        }
      }
    }
  }
}

bool CGameObject::UpdateSecList()
{
  unsigned int newSec = static_cast<unsigned int>(CalcSecIndex());
  if (m_dwCurSec == newSec)
  {
    return true;
  }

  const unsigned int oldSec = m_dwCurSec;
  if (!m_ObjID.m_byKind && !m_ObjID.m_byID)
  {
    newSec = static_cast<unsigned int>(RerangeSecIndex(m_dwCurSec, newSec));
  }

  if (!m_pCurMap->UpdateSecterList(this, m_dwCurSec, newSec))
  {
    return false;
  }

  SetCurSecNum(newSec);
  ResetSector(oldSec, newSec);
  AlterSec();
  return true;
}

void CGameObject::SendMsg_RealFixPosition(bool bCircle)
{
  _object_real_fixpositon_zocl payload{};
  payload.byObjKind = static_cast<char>(m_ObjID.m_byKind);
  payload.byObjID = static_cast<char>(m_ObjID.m_byID);
  payload.wIndex = m_ObjID.m_wIndex;
  payload.dwSerial = m_dwObjSerial;

  unsigned __int8 type[2]{4, 10};
  if (bCircle)
  {
    CircleReport(type, reinterpret_cast<char *>(&payload), sizeof(payload), false);
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(
      m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(&payload),
      static_cast<unsigned __int16>(sizeof(payload)));
  }
}

bool CGameObject::FixTargetWhile(CCharacter *pkTarget, unsigned int dwMiliSecond)
{
  (void)pkTarget;
  (void)dwMiliSecond;
  return false;
}

int CGameObject::GetAttackDP()
{
  return 0;
}

int CGameObject::GetAttackLevel()
{
  __int64 *stackPtr = nullptr;
  __int64 i = 0;
  for (i = 8LL; i; --i)
  {
    stackPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(stackPtr) + 4);
  }
  return GetLevel();
}

float CGameObject::GetAttackRange()
{
  __int64 *stackPtr = nullptr;
  __int64 i = 0;
  for (i = 4LL; i; --i)
  {
    stackPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(stackPtr) + 4);
  }
  return 0.0f;
}

int CGameObject::SetDamage(
  int /*nDam*/,
  CCharacter * /*pDst*/,
  int /*nDstLv*/,
  bool /*bCrt*/,
  int /*nAttackType*/,
  unsigned int /*dwAttackSerial*/,
  bool /*bJadeReturn*/)
{
  return 0;
}

int CGameObject::GetHP()
{
  return 1;
}

int CGameObject::GetLevel()
{
  return 1;
}

int CGameObject::GetMaxHP()
{
  return 1;
}

bool CGameObject::SetCurPos(float *pPos)
{
  if (!m_pCurMap->IsMapIn(pPos))
    return false;
  std::memcpy(m_fCurPos, pPos, sizeof(m_fCurPos));
  return true;
}

void CGameObject::SetCurSecNum(unsigned int dwNewSecNum)
{
  m_dwCurSec = dwNewSecNum;
}

void CGameObject::SetStun(bool bStun)
{
  m_bStun = bStun;
  if (m_bStun)
  {
    m_dwNextFreeStunTime = GetLoopTime() + 1000;
  }
}

bool CGameObject::Destroy()
{
  if (!m_bLive)
  {
    return false;
  }

  if (m_pCurMap)
  {
    (void)m_pCurMap->GetSecInfo();
    m_pCurMap->ExitMap(this, m_dwCurSec);
  }

  if (CGameObject::s_pSelectObject == this)
  {
    CGameObject::s_pSelectObject = nullptr;
  }

  m_bLive = false;
  m_bCorpse = false;
  m_bMove = false;
  m_bStun = false;
  m_bMapLoading = false;
  m_SectorPoint.InitLink();
  m_SectorNetPoint.InitLink();

  if (m_bPlayerCircleList)
  {
    operator delete[](m_bPlayerCircleList);
  }
  m_bPlayerCircleList = nullptr;
  m_bMaxVision = false;
  return 1;
}

void CGameObject::SetMaxVersion()
{
  m_bMaxVision = true;
  if (!m_bPlayerCircleList)
  {
    m_bPlayerCircleList = static_cast<bool *>(operator new[](2532));
  }
  _ResetCirclePlayer();
}

void CGameObject::_ResetCirclePlayer()
{
  if (!m_bPlayerCircleList)
  {
    return;
  }

  bool previousList[2532]{};
  std::memcpy(previousList, m_bPlayerCircleList, sizeof(previousList));
  std::memset(m_bPlayerCircleList, 0, sizeof(previousList));

  _pnt_rect rect{};
  const unsigned int curSec = GetCurSecNum();
  m_pCurMap->GetRectInRadius(&rect, 11, curSec);

  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      _sec_info *secInfo = m_pCurMap->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorPlayers = m_pCurMap->GetSectorListPlayer(m_wMapLayerIndex, secIndex);
      if (!sectorPlayers)
      {
        continue;
      }

      _object_list_point *node = sectorPlayers->m_Head.m_pNext;
      while (node != &sectorPlayers->m_Tail)
      {
        CGameObject *obj = node->m_pItem;
        node = node->m_pNext;
        if (!previousList[obj->m_ObjID.m_wIndex])
        {
          const int targetIndex = obj->m_ObjID.m_wIndex;
          if (m_bMove)
          {
            SendMsg_RealMovePoint(targetIndex);
          }
          else
          {
            SendMsg_FixPosition(targetIndex);
          }
        }
        m_bPlayerCircleList[obj->m_ObjID.m_wIndex] = true;
      }
    }
  }
}

void CGameObject::SendMsg_BreakStop()
{

  _break_stop_result_zocl msg{};
  msg.byObjID = m_ObjID.m_byID;
  msg.dwObjSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zCur, 3);

  unsigned __int8 type[2] = {4, 30};
  CircleReport(type, reinterpret_cast<char *>(&msg), 11, false);
}

void CGameObject::SendMsg_FixPosition(int n)
{
  // this is not a stub
}

void CGameObject::SendMsg_RealMovePoint(int n)
{
  // this is not a stub
}

void CGameObject::SendMsg_SetHPInform()
{
  // this is not a stub
}

void CGameObject::SendMsg_StunInform()
{
  // this is not a stub
}

bool CGameObject::SetHP(int /*nHP*/, bool /*bOver*/)
{
  // this is not a stub
  return true;
}

void CGameObject::CalcAbsPos()
{
  _bsp_info *BspInfo = this->m_pCurMap->GetBspInfo();
  this->m_fAbsPos[0] = static_cast<float>(-BspInfo->m_nMapMinSize[0]) + this->m_fCurPos[0];
  this->m_fAbsPos[1] = static_cast<float>(BspInfo->m_nMapMaxSize[1]) - this->m_fCurPos[1];
  this->m_fAbsPos[2] = static_cast<float>(BspInfo->m_nMapMaxSize[2]) - this->m_fCurPos[2];
}

unsigned int CGameObject::CalcSecIndex()
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

int CGameObject::GetUseSectorRange()
{
  return this->m_pCurMap->m_pMapSet->m_nRadius;
}

unsigned __int16 CGameObject::CalcCurHPRate()
{
  const float currentHp = static_cast<float>(GetHP());
  const int maxHp = static_cast<int>(GetMaxHP());
  return static_cast<unsigned int>(static_cast<int>((currentHp / static_cast<float>(maxHp)) * 10000.0f));
}

bool CGameObject::IsInTown()
{
  // this is not a stub
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
            if (m_pItem != this
                && CanSendCircleReportToPlayer(this, static_cast<CPlayer *>(m_pItem), nMsgSize, bToOne))
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
              && CanSendCircleReportToPlayer(this, static_cast<CPlayer *>(m_pItem), nMsgSize, bToOne))
            {
              g_Network.m_pProcess[0]->LoadSendMsg(p_m_ObjID->m_wIndex, pbyType, szMsg, nMsgSize);
            }
          }
        }
      }
    }
  }
}

bool CGameObject::Create(_object_create_setdata *pData)
{
  if (this->m_bLive)
    return false;

  this->m_pCurMap = pData->m_pMap;
  this->m_wMapLayerIndex = pData->m_nLayerIndex;
  std::memcpy(this->m_fCurPos, pData->m_fStartPos, sizeof(this->m_fCurPos));
  std::memcpy(this->m_fOldPos, pData->m_fStartPos, sizeof(this->m_fOldPos));
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
    return true;
  }

  this->m_pCurMap = nullptr;
  g_Main.m_logSystemError.Write(
    "CGameObject::Create() : dwSec >= MAX(this[%d-%d-%d])",
    this->m_ObjID.m_byKind,
    this->m_ObjID.m_byID,
    this->m_ObjID.m_wIndex);
  return false;
}

int CGameObject::GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart)
{
// this is not a stub
  return true;
}

float CGameObject::GetDefFacing(int nPart)
{
return FLOAT_0_5;
}

float CGameObject::GetDefGap(int nPart)
{
return FLOAT_0_5;
}

int CGameObject::GetDefSkill(bool bBackAttackDamage)
{
// this is not a stub
  return 1;
}

int CGameObject::GetFireTol()
{
  return 0;
}

int CGameObject::GetSoilTol()
{
  return 0;
}

int CGameObject::GetWaterTol()
{
  return 0;
}

int CGameObject::GetWindTol()
{
  return 0;
}

float CGameObject::GetWeaponAdjust()
{
  return FLOAT_0_5;
}

int CGameObject::GetObjRace()
{
  return -1;
}

int CGameObject::GetAvoidRate()
{
  return 0;
}

int CGameObject::GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack)
{
// this is not a stub
  return 0;
}

int CGameObject::GetWeaponClass()
{
  return 0;
}

float CGameObject::GetWidth()
{
  return 0.0f;
}

void CGameObject::SetAttackPart(int nAttactPart)
{
  // this is not a stub
}

void CGameObject::SetBreakTranspar(bool bBreak)
{
  m_bBreakTranspar = bBreak;
  if (m_bBreakTranspar)
  {
    m_dwOldTickBreakTranspar = GetLoopTime();
  }
}

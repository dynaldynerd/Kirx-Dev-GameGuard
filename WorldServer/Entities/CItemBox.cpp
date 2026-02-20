#include "pch.h"

#include "CItemBox.h"

#include <cstring>
#include <new>
#include <mmsystem.h>

#include "CNetworkEX.h"
#include "CPartyPlayer.h"
#include "CMonster.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

int CItemBox::s_nLiveNum;
unsigned int CItemBox::s_dwSerialCounter;

CItemBox::CItemBox()
  : m_dwOwnerSerial(static_cast<unsigned int>(-1)),
    m_wOwnerIndex(static_cast<unsigned __int16>(-1)),
    m_dwThrowerSerial(static_cast<unsigned int>(-1)),
    m_byThrowerID(static_cast<unsigned __int8>(-1)),
    m_wThrowerIndex(static_cast<unsigned __int16>(-1)),
    m_wMonRecIndex(static_cast<unsigned __int16>(-1)),
    m_bBossMob(false),
    m_dwThrowerCharSerial(static_cast<unsigned int>(-1)),
    m_byThrowerRaceCode(static_cast<unsigned __int8>(-1)),
    m_byThrowerDegree(static_cast<unsigned __int8>(-1)),
    m_szThrowerItemHistoryFileName(nullptr),
    m_dwLootStartTime(0),
    m_nStateCode(0),
    m_dwLastDestroyTime(0),
    m_byCreateCode(static_cast<unsigned __int8>(-1)),
    m_dwPartyBossSerial(static_cast<unsigned int>(-1)),
    m_bPartyShare(false),
    m_bCompDgr(false),
    m_dwEventPartyBoss(static_cast<unsigned int>(-1)),
    m_dwEventGuildSerial(static_cast<unsigned int>(-1)),
    m_byEventRaceCode(static_cast<unsigned __int8>(-1)),
    m_byEventLootAuth(3),
    m_bHolyScanner(0),
    m_Item(),
    m_bHide(false)
{
  m_szThrowerItemHistoryFileName = new (std::nothrow) char[0x40];
}

CItemBox::~CItemBox()
{
  if (m_szThrowerItemHistoryFileName)
  {
    delete[] m_szThrowerItemHistoryFileName;
    m_szThrowerItemHistoryFileName = nullptr;
  }
}

void CItemBox::Init(_object_id *pID)
{
  CGameObject::Init(pID);
  m_dwLastDestroyTime = 0;
  m_wMonRecIndex = static_cast<unsigned __int16>(-1);
  m_bBossMob = false;
  std::memset(m_wszThrowerName, 0, sizeof(m_wszThrowerName));
  std::memset(m_aszThrowerName, 0, sizeof(m_aszThrowerName));
  std::memset(m_szThrowerID, 0, sizeof(m_szThrowerID));
  if (m_szThrowerItemHistoryFileName != nullptr)
  {
    std::memset(m_szThrowerItemHistoryFileName, 0, 0x40);
  }
  m_byCreateCode = static_cast<unsigned __int8>(-1);
  m_bCompDgr = false;
  m_bHide = false;
}

bool CItemBox::IsTakeRight(CPlayer *pOne)
{
  if (m_byCreateCode == 7)
  {
    if (!m_bHolyScanner
        || pOne->m_MinigTicket.AuthLastMentalTicket(
             g_HolySys.GetStartYear(),
             g_HolySys.GetStartMonth(),
             g_HolySys.GetStartDay(),
             g_HolySys.GetStartHour(),
             g_HolySys.GetNumOfTime()))
    {
      switch (m_byEventLootAuth)
      {
        case 0:
          return m_dwEventPartyBoss == pOne->m_Param.GetCharSerial()
                 || (pOne->m_pPartyMgr && pOne->m_pPartyMgr->IsPartyMode()
                     && m_dwEventPartyBoss == pOne->m_pPartyMgr->m_pPartyBoss->m_id.dwSerial);
        case 1:
          return m_byEventRaceCode == pOne->m_Param.GetRaceCode();
        case 2:
          return pOne->m_Param.m_pGuild && m_dwEventGuildSerial == pOne->m_Param.m_pGuild->m_dwSerial;
        case 3:
          return true;
        case 4:
        {
          auto *system = CPvpUserAndGuildRankingSystem::Instance();
          const unsigned int bossSerial = system->GetCurrentRaceBossSerial(m_byEventRaceCode, 0);
          return bossSerial == pOne->m_Param.GetCharSerial();
        }
        case 5:
          if (m_byEventRaceCode == pOne->m_Param.GetRaceCode() && pOne->m_Param.m_pGuild)
          {
            const unsigned int guildMasterSerial = pOne->m_Param.m_pGuild->GetGuildMasterSerial();
            return guildMasterSerial == pOne->m_Param.GetCharSerial();
          }
          return false;
        default:
          return false;
      }
    }
    return false;
  }
  if (m_byCreateCode == 4 || m_byCreateCode == 6)
  {
    return g_HolySys.IsItemLootAuthority(pOne, m_byCreateCode) != 0;
  }
  if (m_dwOwnerSerial == pOne->m_dwObjSerial)
  {
    return true;
  }
  if (m_dwThrowerCharSerial == pOne->m_dwObjSerial)
  {
    return true;
  }
  if (m_byCreateCode == 3)
  {
    return false;
  }
  if (m_bCompDgr && !pOne->m_byUserDgr)
  {
    return false;
  }
  if (m_byThrowerDegree != 255 && m_byThrowerDegree != pOne->m_byUserDgr)
  {
    return false;
  }
  if (m_byThrowerRaceCode != 255 && pOne->m_Param.GetRaceCode() != m_byThrowerRaceCode)
  {
    return false;
  }
  if (m_dwOwnerSerial == static_cast<unsigned int>(-1))
  {
    return true;
  }
  if (m_nStateCode <= 0)
  {
    if (m_bPartyShare && pOne->m_pPartyMgr)
    {
      CPartyPlayer **partyMembers = pOne->m_pPartyMgr->GetPtrPartyMember();
      if (partyMembers)
      {
        for (int j = 0; j < 8 && partyMembers[j]; ++j)
        {
          if (partyMembers[j]->m_id.dwSerial == m_dwOwnerSerial)
          {
            return true;
          }
        }
      }
    }
    return false;
  }
  return true;
}

bool CItemBox::Create(_itembox_create_setdata *pParam, bool bHide)
{
  if (!CGameObject::Create(pParam))
  {
    return false;
  }

  m_dwObjSerial = s_dwSerialCounter++;
  m_bHide = bHide;
  m_bCompDgr = false;
  m_dwPartyBossSerial = pParam->dwPartyBossSerial;
  m_byEventLootAuth = pParam->byEventItemLootAuth;
  m_bHolyScanner = pParam->bHolyScanner;

  if (pParam->pAttacker)
  {
    m_byEventRaceCode = pParam->pAttacker->m_Param.GetRaceCode();
    if (pParam->pAttacker->m_pPartyMgr && pParam->pAttacker->m_pPartyMgr->IsPartyMode())
    {
      m_dwEventPartyBoss = pParam->pAttacker->m_pPartyMgr->m_pPartyBoss->m_id.dwSerial;
    }
    else
    {
      m_dwEventPartyBoss = pParam->pAttacker->m_Param.GetCharSerial();
    }
    if (pParam->pAttacker->m_Param.m_pGuild)
    {
      m_dwEventGuildSerial = pParam->pAttacker->m_Param.m_pGuild->m_dwSerial;
    }
  }

  if (pParam->pOwner)
  {
    m_dwOwnerSerial = pParam->pOwner->m_dwObjSerial;
    m_wOwnerIndex = pParam->pOwner->m_ObjID.m_wIndex;
    m_bPartyShare = pParam->bParty;
  }

  if (pParam->pThrower)
  {
    m_dwThrowerSerial = pParam->pThrower->m_dwObjSerial;
    m_wThrowerIndex = pParam->pThrower->m_ObjID.m_wIndex;
    m_byThrowerID = pParam->pThrower->m_ObjID.m_byID;
    m_byThrowerRaceCode = static_cast<unsigned __int8>(-1);
    m_bCompDgr = false;

    if (pParam->pThrower->m_ObjID.m_byID == 0)
    {
      CPlayer *throwerPlayer = static_cast<CPlayer *>(pParam->pThrower);
      const char *charNameW = throwerPlayer->m_Param.GetCharNameW();
      strcpy_0(m_wszThrowerName, charNameW);
      W2M(m_wszThrowerName, m_aszThrowerName, 0x11u);
      m_dwThrowerCharSerial = throwerPlayer->m_Param.GetCharSerial();
      if (throwerPlayer->m_pUserDB)
      {
        strcpy_0(m_szThrowerID, throwerPlayer->m_pUserDB->m_szAccountID);
      }
      m_byThrowerRaceCode = throwerPlayer->m_Param.GetRaceCode();
      m_byThrowerDegree = throwerPlayer->m_byUserDgr;
      if (m_szThrowerItemHistoryFileName)
      {
        strcpy_0(m_szThrowerItemHistoryFileName, throwerPlayer->m_szItemHistoryFileName);
      }
    }

    if (pParam->pThrower->m_ObjID.m_byID == 1)
    {
      if (pParam->pOwner && pParam->pOwner->m_byUserDgr)
      {
        m_bCompDgr = true;
      }
      CMonster *monster = reinterpret_cast<CMonster *>(pParam->pThrower);
      if (monster->m_pMonRec)
      {
        m_wMonRecIndex = static_cast<unsigned __int16>(monster->m_pMonRec->m_dwIndex);
        m_bBossMob = (*reinterpret_cast<const int *>(reinterpret_cast<const char *>(monster->m_pMonRec) + 272) == 1);
      }
    }
  }

  m_byCreateCode = pParam->byCreateCode;
  memcpy_0(&m_Item, &pParam->Item, sizeof(m_Item));
  m_dwLootStartTime = GetLoopTime();
  switch (m_byCreateCode)
  {
    case 1:
      m_nStateCode = 1;
      m_dwLootStartTime += 20000;
      break;
    case 2:
    case 3:
      m_nStateCode = 0;
      m_dwLootStartTime += 40000;
      break;
    case 4:
    case 6:
      m_nStateCode = 0;
      m_dwLootStartTime += 60000;
      break;
    default:
      m_nStateCode = 0;
      break;
  }
  ++s_nLiveNum;
  SendMsg_Create();
  return true;
}

bool CItemBox::Destroy()
{
  m_dwOwnerSerial = static_cast<unsigned int>(-1);
  m_wOwnerIndex = static_cast<unsigned __int16>(-1);
  m_dwThrowerSerial = static_cast<unsigned int>(-1);
  m_wThrowerIndex = static_cast<unsigned __int16>(-1);
  m_dwThrowerCharSerial = static_cast<unsigned int>(-1);
  m_byThrowerID = static_cast<unsigned __int8>(-1);
  m_wszThrowerName[0] = '\0';
  m_aszThrowerName[0] = '\0';
  m_szThrowerID[0] = '\0';
  m_byThrowerDegree = static_cast<unsigned __int8>(-1);
  m_bPartyShare = false;
  m_byThrowerRaceCode = static_cast<unsigned __int8>(-1);
  m_wMonRecIndex = static_cast<unsigned __int16>(-1);
  m_bBossMob = false;
  m_dwEventPartyBoss = static_cast<unsigned int>(-1);
  m_dwEventGuildSerial = static_cast<unsigned int>(-1);
  m_byEventRaceCode = static_cast<unsigned __int8>(-1);
  m_byEventLootAuth = 3;
  m_bHolyScanner = 0;
  m_dwLastDestroyTime = timeGetTime();
  SendMsg_Destroy();
  --s_nLiveNum;
  return CGameObject::Destroy();
}

void CItemBox::SendMsg_Create()
{
  _itembox_create_zocl msg{};

  msg.byItemTableCode = m_Item.m_byTableCode;
  msg.wItemRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.byAmount = static_cast<unsigned __int8>(m_Item.m_dwDur);
  msg.wBoxIndex = m_ObjID.m_wIndex;
  msg.byState = static_cast<char>(m_nStateCode);
  msg.dwOwerSerial = m_dwOwnerSerial;
  msg.idDumber.byID = m_byThrowerID;
  msg.idDumber.wIndex = m_wThrowerIndex;
  msg.idDumber.dwSerial = m_dwThrowerSerial;
  msg.byThrowerRace = m_byThrowerRaceCode;
  FloatToShort(m_fCurPos, msg.zPos, 3);

  unsigned __int8 pbyType[2] = {3, 20};
  if (m_bHide)
  {
    if (m_wOwnerIndex != 0xFFFF)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(m_wOwnerIndex, pbyType, reinterpret_cast<char *>(&msg), 0x19u);
    }
  }
  else
  {
    CircleReport(pbyType, reinterpret_cast<char *>(&msg), 25, 0);
  }
}

void CItemBox::SendMsg_Destroy()
{
  unsigned __int16 index = m_ObjID.m_wIndex;
  unsigned __int8 pbyType[2] = {3, 28};
  CircleReport(pbyType, reinterpret_cast<char *>(&index), 2, 0);
}

void CItemBox::SendMsg_StateChange()
{
  if (m_bHide)
  {
    return;
  }

  _itembox_state_change_zocl msg{};

  msg.byState = static_cast<char>(m_nStateCode);
  msg.wItemBoxIndex = m_ObjID.m_wIndex;
  msg.dwOwerSerial = m_dwOwnerSerial;

  unsigned __int8 pbyType[2] = {7, 1};
  CircleReport(pbyType, reinterpret_cast<char *>(&msg), 7, 0);
}

void CItemBox::SendMsg_FixPosition(unsigned int n)
{
  if (m_bHide)
  {
    return;
  }

  _itembox_fixpositon_zocl msg{};

  msg.byItemTableCode = m_Item.m_byTableCode;
  msg.wItemRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.byAmount = static_cast<unsigned __int8>(m_Item.m_dwDur);
  msg.byState = static_cast<char>(m_nStateCode);
  msg.wItemBoxIndex = m_ObjID.m_wIndex;
  msg.dwOwerSerial = m_dwOwnerSerial;
  msg.byThrowerRace = m_byThrowerRaceCode;
  FloatToShort(m_fCurPos, msg.zPos, 3);

  unsigned __int8 pbyType[2] = {4, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(n, pbyType, reinterpret_cast<char *>(&msg), 0x12u);
}

void CItemBox::Loop()
{
  const unsigned int loopTime = GetLoopTime();
  if (loopTime <= m_dwLootStartTime)
  {
    return;
  }

  if (m_nStateCode == 0)
  {
    const unsigned int elapsed = loopTime - m_dwLootStartTime;
    if (elapsed > 20000)
    {
      if (m_byCreateCode == 2 || m_byCreateCode == 3 || m_byCreateCode == 6 || m_byCreateCode == 4)
      {
        Destroy();
      }
      else
      {
        m_nStateCode = 1;
        SendMsg_StateChange();
      }
    }
    return;
  }

  if (m_nStateCode == 1)
  {
    if (loopTime - m_dwLootStartTime > 50000)
    {
      m_nStateCode = 2;
      SendMsg_StateChange();
    }
    return;
  }

  if (m_nStateCode == 2 && loopTime - m_dwLootStartTime > 60000)
  {
    Destroy();
  }
}

CItemBox *CreateItemBox(
  _STORAGE_LIST::_db_con *pItem,
  CPlayer *pOwner,
  unsigned int dwPartyBossSerial,
  bool bPartyShare,
  CCharacter *pThrower,
  unsigned __int8 byCreateCode,
  CMapData *pMap,
  unsigned __int16 wLayerIndex,
  float *pStdPos,
  bool bHide)
{
  CItemBox *itemBox = nullptr;
  for (int j = 0; j < MAX_ITEMBOX; ++j)
  {
    if (!g_ItemBox[j].m_bLive)
    {
      itemBox = &g_ItemBox[j];
      break;
    }
  }
  if (!itemBox)
  {
    return nullptr;
  }

  _itembox_create_setdata param;
  memcpy_0(&param.Item, pItem, sizeof(param.Item));
  param.m_pRecordSet = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  if (!param.m_pRecordSet)
  {
    return nullptr;
  }
  param.byCreateCode = byCreateCode;
  param.pOwner = pOwner;
  param.bParty = bPartyShare;
  param.pThrower = pThrower;
  param.m_pMap = pMap;
  param.m_nLayerIndex = wLayerIndex;
  param.dwPartyBossSerial = dwPartyBossSerial;
  pMap->GetRandPosInRange(pStdPos, 10, param.m_fStartPos);
  if (itemBox->Create(&param, bHide))
  {
    return itemBox;
  }
  return nullptr;
}

CItemBox *CreateItemBox(
  _STORAGE_LIST::_db_con *pItem,
  unsigned __int8 byCreateCode,
  CMapData *pMap,
  unsigned __int16 wLayerIndex,
  float *pStdPos,
  bool bHide,
  CPlayer *pAttacker,
  int bHolyScanner,
  unsigned __int8 byEventItemLootAuth)
{
  CItemBox *itemBox = nullptr;
  for (int j = 0; j < MAX_ITEMBOX; ++j)
  {
    if (!g_ItemBox[j].m_bLive)
    {
      itemBox = &g_ItemBox[j];
      break;
    }
  }
  if (!itemBox)
  {
    return nullptr;
  }

  _itembox_create_setdata param;
  memcpy_0(&param.Item, pItem, sizeof(param.Item));
  param.m_pRecordSet = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  if (!param.m_pRecordSet)
  {
    return nullptr;
  }
  param.byCreateCode = byCreateCode;
  param.pOwner = nullptr;
  param.bParty = false;
  param.pThrower = nullptr;
  param.m_pMap = pMap;
  param.m_nLayerIndex = wLayerIndex;
  param.dwPartyBossSerial = static_cast<unsigned int>(-1);
  memcpy_0(param.m_fStartPos, pStdPos, sizeof(param.m_fStartPos));
  param.byEventItemLootAuth = byEventItemLootAuth;
  param.bHolyScanner = bHolyScanner;
  param.pAttacker = pAttacker;
  if (itemBox->Create(&param, bHide))
  {
    return itemBox;
  }
  return nullptr;
}

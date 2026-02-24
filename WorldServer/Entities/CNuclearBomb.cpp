#include "pch.h"

#include "CNuclearBomb.h"

#include "CNetProcess.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CMapData.h"
#include "CObjectList.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "nuclear_find_rader_result_zocl.h"
#include "nuclear_result_code_zocl.h"
#include "pnt_rect.h"

#include <mmsystem.h>
#include <cstring>

unsigned int CNuclearBomb::s_dwSerialCnt;

_be_damaged_player::_be_damaged_player()
{
  memset_0(this, 0, sizeof(_be_damaged_player));
}

_be_damaged_char::_be_damaged_char()
{
  memset_0(this, 0, sizeof(_be_damaged_char));
}

CNuclearBomb::CNuclearBomb()
{
  m_pMaster = nullptr;
  s_dwSerialCnt = 0;
}

CNuclearBomb::~CNuclearBomb() = default;

__int64 CNuclearBomb::GetGenAttackProb(CCharacter * /*pDst*/, int /*nPart*/, bool /*bBackAttack*/)
{
  return 100;
}

void CNuclearBomb::Loop()
{
  const DWORD currentTime = timeGetTime();
  if (m_pMaster->m_bCorpse || !m_pMaster->m_bLive)
  {
    if (m_bIsLive)
    {
      SendMsg_MasterDie();
      m_bIsLive = false;
    }

    if (m_bUse && m_byBombState <= 4u)
    {
      m_byBombState = 6;
      Destroy();
    }
  }
  if (m_byBombState != 0)
  {
    switch (m_byBombState)
    {
      case 1:
        if (currentTime >= m_dwDurTime + m_dwStartTime - m_dwAttInformTime)
        {
          SendMsg_InformDropPos();
          ++m_byBombState;
        }
        break;
      case 2:
        if (currentTime >= m_dwDurTime + m_dwStartTime - m_dwAttStartTime)
        {
          if (strcmp_0(m_pCurMap->m_pMapSet->m_strCode, "resources") == 0)
          {
            SendMsg_DropMissile();
          }
          ++m_byBombState;
        }
        break;
      case 3:
        if (currentTime >= m_dwDurTime + m_dwStartTime)
        {
          GetShowEffectList();
          SendMsg_AddEffect();
          NuclearDamege();
          ++m_byBombState;
        }
        break;
      default:
        if (m_byBombState == 5 && currentTime >= m_dwDelayTime + m_dwStartTime)
        {
          m_byBombState = 6;
          if (m_bLive)
          {
            Destroy();
          }
        }
        break;
    }
  }
  else if (currentTime >= m_dwDurTime + m_dwStartTime - m_dwWarnTime)
  {
    const unsigned __int8 raceCode = static_cast<unsigned __int8>(m_pMaster->m_Param.GetRaceCode());
    WarningToAll(raceCode);
    ++m_byBombState;
  }
}

bool CNuclearBomb::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_pMaster = nullptr;
  m_bUse = false;
  m_dwDelayTime = 0;
  m_dwDurTime = 0;
  m_dwStartTime = 0;
  m_nStartDmLoop = 0;
  return true;
}

bool CNuclearBomb::Create(_nuclear_create_setdata *pData)
{
  if (CCharacter::Create(pData))
  {
    m_pMaster = pData->pMaster;
    m_dwStartTime = timeGetTime();
    m_dwDurTime = 1000 * *reinterpret_cast<unsigned int *>(&m_pRecordSet[5].m_strCode[4]);
    m_dwDelayTime = 1000 * *reinterpret_cast<unsigned int *>(&m_pRecordSet[5].m_strCode[12]);
    m_dwObjSerial = static_cast<unsigned int>(GetNewSerial());
    m_bUse = true;
    m_bIsLive = true;
    memcpy_0(m_fDropPos, pData->m_fStartPos, sizeof(m_fDropPos));
    m_byBombState = 0;
    m_nDamagedObjNum = 0;
    m_nEffObjNum = 0;
    m_dwWarnTime = 1000 * pData->m_dwWarnTime;
    m_dwAttInformTime = 1000 * pData->m_dwAttInformTime;
    m_dwAttStartTime = 1000 * pData->m_dwAttStartTime;
  }

  return true;
}

bool CNuclearBomb::GetUse()
{
  return m_bUse;
}

unsigned __int8 CNuclearBomb::GetBombStatus()
{
  return m_byBombState;
}

__int64 CNuclearBomb::GetDamagedObjNum()
{
  return static_cast<unsigned int>(m_nDamagedObjNum);
}

void CNuclearBomb::SetBombStatus()
{
  ++m_byBombState;
}

void CNuclearBomb::Attack(int startNum, int objCount)
{
  const int beginIndex = 30 * startNum;
  for (int index = beginIndex; index < beginIndex + objCount; ++index)
  {
    _be_damaged_player &damageInfo = m_DamList[index];
    if (damageInfo.m_pChar->m_bLive && damageInfo.m_pChar->m_dwObjSerial == damageInfo.m_dwDamCharSerial)
    {
      const int level = static_cast<int>(GetLevel());
      damageInfo.m_pChar->SetDamage(damageInfo.m_nDamage, this, level, false, -1, 0, true);
    }
  }
}

float *CNuclearBomb::GetMissilePos()
{
  return m_fDropPos;
}

unsigned __int16 CNuclearBomb::GetItemIndex()
{
  return m_wItemIndex;
}

void CNuclearBomb::SetNuclearIndex(unsigned __int16 wItemIndex)
{
  m_wItemIndex = wItemIndex;
}

unsigned __int16 CNuclearBomb::GetControlSerial()
{
  return m_wControlSerial;
}

void CNuclearBomb::SetControlSerial(unsigned __int16 wControlSerial)
{
  m_wControlSerial = wControlSerial;
}

__int64 CNuclearBomb::GetMasterRace()
{
  return static_cast<unsigned int>(m_pMaster->m_Param.GetRaceCode());
}

unsigned __int8 CNuclearBomb::GetMasterClass()
{
  const unsigned __int8 raceCode = m_pMaster->m_Param.GetRaceCode();
  const unsigned int charSerial = m_pMaster->m_Param.GetCharSerial();
  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  const unsigned __int8 bossType = ranking->GetBossType(raceCode, charSerial);
  if (bossType == 0)
  {
    return 0;
  }
  if (bossType == 1)
  {
    return 1;
  }
  if (bossType == 5)
  {
    return 2;
  }
  return 3;
}

void CNuclearBomb::GetShowEffectList()
{
  const int rangeWithBias = static_cast<int>(*reinterpret_cast<float *>(&m_pRecordSet[5].m_strCode[20]) + 100.0f);
  const int radius = rangeWithBias / 100;

  _pnt_rect rect{};
  m_pCurMap->GetRectInRadius(&rect, radius, static_cast<unsigned int>(GetCurSecNum()));

  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      const unsigned int secIndex = static_cast<unsigned int>(m_pCurMap->GetSecInfo()->m_nSecNumW * y + x);
      CObjectList *sectorPlayers = m_pCurMap->GetSectorListPlayer(m_wMapLayerIndex, secIndex);
      if (!sectorPlayers)
      {
        continue;
      }

      for (_object_list_point *node = sectorPlayers->m_Head.m_pNext; node != &sectorPlayers->m_Tail; node = node->m_pNext)
      {
        CCharacter *candidate = static_cast<CCharacter *>(node->m_pItem);
        if (candidate->IsAttackableInTown() || !candidate->IsInTown())
        {
          if (candidate->IsBeAttackedAble(true))
          {
            const float maxDistance = static_cast<float>(rangeWithBias) + candidate->GetWidth() / 2.0f;
            if (GetSqrt(candidate->m_fCurPos, m_fCurPos) <= maxDistance
              && candidate->m_ObjID.m_byID == 0
              && m_nEffObjNum < 400)
            {
              m_EffList[m_nEffObjNum++].m_pChar = candidate;
            }
          }
        }
      }
    }
  }
}

void CNuclearBomb::NuclearDamege()
{
  const int damageRange = static_cast<int>(*reinterpret_cast<float *>(&m_pRecordSet[5].m_strCode[20]));
  const int radius = damageRange / 100;

  _pnt_rect rect{};
  m_pCurMap->GetRectInRadius(&rect, radius, static_cast<unsigned int>(GetCurSecNum()));

  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      const unsigned int secIndex = static_cast<unsigned int>(m_pCurMap->GetSecInfo()->m_nSecNumW * y + x);
      CObjectList *sectorPlayers = m_pCurMap->GetSectorListPlayer(m_wMapLayerIndex, secIndex);
      if (!sectorPlayers)
      {
        continue;
      }

      for (_object_list_point *node = sectorPlayers->m_Head.m_pNext; node != &sectorPlayers->m_Tail; node = node->m_pNext)
      {
        CCharacter *candidate = static_cast<CCharacter *>(node->m_pItem);
        if (!candidate->m_bCorpse && candidate->m_bLive)
        {
          const int candidateRace = static_cast<int>(candidate->GetObjRace());
          if (candidateRace != static_cast<int>(GetMasterRace())
            && (candidate->IsAttackableInTown() || !candidate->IsInTown()))
          {
            if (candidate->IsBeAttackedAble(true))
            {
              const float maxDistance = static_cast<float>(damageRange) + candidate->GetWidth() / 2.0f;
              if (GetSqrt(candidate->m_fCurPos, m_fCurPos) <= maxDistance
                && candidate->m_ObjID.m_byID == 0
                && m_nDamagedObjNum < 300)
              {
                m_DamList[m_nDamagedObjNum].m_pChar = candidate;
                m_DamList[m_nDamagedObjNum].m_nDamage =
                  *reinterpret_cast<int *>(&m_pRecordSet[5].m_strCode[48]);
                m_DamList[m_nDamagedObjNum++].m_dwDamCharSerial = candidate->m_dwObjSerial;
              }
            }
          }
        }
      }
    }
  }
}

void CNuclearBomb::WarningToAll(unsigned __int8 byRaceCode)
{
  for (unsigned int clientIndex = 0; clientIndex < MAX_PLAYER; ++clientIndex)
  {
    CPlayer *player = &g_Player[clientIndex];
    if (player->m_bOper && strcmp_0(player->m_pCurMap->m_pMapSet->m_strCode, "resources") == 0)
    {
      if (static_cast<int>(byRaceCode) != player->m_Param.GetRaceCode())
      {
        SendMsg_NuclearFind(clientIndex, byRaceCode);
      }
    }
  }
}

void CNuclearBomb::SendMsg_NuclearFind(unsigned int n, unsigned __int8 race)
{
  _nuclear_find_rader_result_zocl result{};
  result.byRaceCode = race;
  result.byUseClass = GetMasterClass();

  unsigned __int8 type[2]{60, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&result), 2u);
}

void CNuclearBomb::SendMsg_InformDropPos()
{
  char payload[0x0E]{};
  for (unsigned int clientIndex = 0; clientIndex < MAX_PLAYER; ++clientIndex)
  {
    CPlayer *targetPlayer = &g_Player[clientIndex];
    if (targetPlayer->m_bOper && !targetPlayer->m_bCorpse &&
        strcmp_0(targetPlayer->m_pCurMap->m_pMapSet->m_strCode, "resources") == 0)
    {
      payload[0] = static_cast<char>(m_pMaster->m_Param.GetRaceCode());
      payload[1] = static_cast<char>(GetMasterClass());
      *reinterpret_cast<float *>(payload + 2) = m_fCurPos[0];
      *reinterpret_cast<float *>(payload + 6) = m_fCurPos[1];
      *reinterpret_cast<float *>(payload + 10) = m_fCurPos[2];

      unsigned __int8 type[2]{60, 7};
      g_Network.m_pProcess[0]->LoadSendMsg(clientIndex, type, payload, 0x0Eu);
    }
  }
}

void CNuclearBomb::SendMsg_AddEffect()
{
  for (int index = 0; index < m_nEffObjNum; ++index)
  {
    if (m_EffList[index].m_pChar &&
        strcmp_0(m_EffList[index].m_pChar->m_pCurMap->m_pMapSet->m_strCode, "resources") == 0)
    {
      char payload[2]{};
      payload[0] = static_cast<char>(m_pMaster->m_Param.GetRaceCode());
      payload[1] = static_cast<char>(GetMasterClass());

      unsigned __int8 type[2]{60, 6};
      g_Network.m_pProcess[0]->LoadSendMsg(m_EffList[index].m_pChar->m_ObjID.m_wIndex, type, payload, 2u);
    }
  }

  SendMsg_InformAttack();
}

void CNuclearBomb::SendMsg_InformAttack()
{
  const unsigned __int8 raceCode = m_pMaster->m_Param.GetRaceCode();
  for (unsigned int clientIndex = 0; clientIndex < MAX_PLAYER; ++clientIndex)
  {
    CPlayer *targetPlayer = &g_Player[clientIndex];
    if (targetPlayer->m_bOper && !targetPlayer->m_bCorpse && targetPlayer->m_bLive &&
        strcmp_0(targetPlayer->m_pCurMap->m_pMapSet->m_strCode, "resources") == 0)
    {
      char payload[2]{};
      payload[0] = static_cast<char>(raceCode);
      payload[1] = static_cast<char>(GetMasterClass());

      unsigned __int8 type[2]{60, 8};
      g_Network.m_pProcess[0]->LoadSendMsg(clientIndex, type, payload, 2u);
    }
  }
}

void CNuclearBomb::SendMsg_MasterDie()
{
  const unsigned __int8 raceCode = m_pMaster->m_Param.GetRaceCode();
  for (unsigned int clientIndex = 0; clientIndex < MAX_PLAYER; ++clientIndex)
  {
    CPlayer *targetPlayer = &g_Player[clientIndex];
    if (targetPlayer->m_bOper && !targetPlayer->m_bCorpse && targetPlayer->m_bLive &&
        strcmp_0(targetPlayer->m_pCurMap->m_pMapSet->m_strCode, "resources") == 0 &&
        m_byBombState != 0 && m_byBombState < 5u)
    {
      char payload[2]{};
      payload[0] = static_cast<char>(raceCode);
      payload[1] = static_cast<char>(GetMasterClass());

      unsigned __int8 type[2]{60, 9};
      g_Network.m_pProcess[0]->LoadSendMsg(clientIndex, type, payload, 2u);
    }
  }
}

void CNuclearBomb::SendMsg_Result(unsigned int n, unsigned __int8 byCode)
{
  _nuclear_result_code_zocl result{};
  result.byRetCode = byCode;

  unsigned __int8 type[2]{60, 0};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&result), result.size());
}

void CNuclearBomb::SendMsg_DropMissile()
{
  char payload[5]{};
  payload[0] = static_cast<char>(m_pMaster->m_Param.GetRaceCode());
  payload[1] = 26;
  *reinterpret_cast<unsigned __int16 *>(payload + 2) = m_wItemIndex;
  payload[4] = static_cast<char>(GetMasterClass());

  unsigned __int8 type[2]{60, 5};
  CircleReport(type, payload, 5, false);
}

__int64 CNuclearBomb::GetNewSerial()
{
  return ++s_dwSerialCnt;
}

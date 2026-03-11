#include "pch.h"

#include "CRadarItemMgr.h"

#include <cstring>
#include <mmsystem.h>

#include "CPartyPlayer.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CRecordData.h"
#include "GlobalObjects.h"
#include "RadarItem_fld.h"

namespace
{
unsigned int AddRadarCharInfo(_detected_char_list &list, unsigned __int8 charType, float *pos)
{
  if (list.nSize >= 50)
  {
    return static_cast<unsigned int>(list.nSize);
  }

  _detected_char_list::_char_info *entry = &list.CharInfoList[list.nSize];
  entry->m_byCharType = charType;
  entry->m_fPos[0] = pos[0];
  entry->m_fPos[1] = pos[2];
  ++list.nSize;
  return static_cast<unsigned int>(list.nSize);
}
}

CRadarItemMgr::CRadarItemMgr()
{
  Init();
}

CRadarItemMgr::~CRadarItemMgr()
{
}

void CRadarItemMgr::Init()
{
  m_bUse = false;
  m_bPlayerEnd = false;
  m_bMonEnd = false;
  std::memset(m_strRadarCode, 0, sizeof(m_strRadarCode));
  m_pDestMap = nullptr;
  m_dwDelayTime = 0;
  m_dwDurTime = 0;
  m_dwStartTime = 0;
  m_pMaster = nullptr;
  m_nPlayerNum = 0;
  m_nMonNum = 0;
  ResetFlags();
  ResetUpdate();
  m_RadarResult.nSize = 0;
}

void CRadarItemMgr::Init(unsigned int dwDelayTime)
{
  if (dwDelayTime)
  {
    m_bUse = true;
    m_bPlayerEnd = true;
    m_bMonEnd = true;
    m_dwStartTime = timeGetTime();
    m_dwDelayTime = dwDelayTime;
  }
  else
  {
    m_bUse = false;
    m_bPlayerEnd = false;
    m_bMonEnd = false;
    m_dwStartTime = 0;
    m_dwDelayTime = 0;
  }

  m_pMaster = nullptr;
  std::memset(m_strRadarCode, 0, sizeof(m_strRadarCode));
  m_pDestMap = nullptr;
  m_dwDurTime = 0;
  m_nPlayerNum = 0;
  m_nMonNum = 0;
  ResetFlags();
  ResetUpdate();
  m_RadarResult.nSize = 0;
}

bool CRadarItemMgr::IsUse()
{
  return m_bUse;
}

bool CRadarItemMgr::IsRadarUse()
{
  const unsigned int now = timeGetTime();
  const unsigned int endTick = m_dwDelayTime + m_dwStartTime;
  if (!endTick && !m_bUse)
  {
    return m_bUse;
  }
  if (now >= endTick)
  {
    Init();
    m_bUpdate = true;
  }
  return m_bUse;
}

bool CRadarItemMgr::IsUpdate()
{
  return m_bUpdate;
}

unsigned int CRadarItemMgr::GetStartTime()
{
  return m_dwStartTime;
}

void CRadarItemMgr::SetUseRadar(
  char (*strRadarCode)[64],
  CPlayer *pMaster,
  unsigned int dwDurTime,
  unsigned int dwDelayTime)
{
  m_bUse = true;
  m_bPlayerEnd = false;
  m_bMonEnd = false;
  std::memcpy(m_strRadarCode, strRadarCode, sizeof(m_strRadarCode));
  m_dwStartTime = timeGetTime();
  m_dwDurTime = 1000 * dwDurTime;
  m_dwDelayTime = 1000 * dwDelayTime;
  m_pMaster = pMaster;
  m_pDestMap = pMaster->m_pCurMap;
  m_nPlayerNum = 0;
  m_nMonNum = 0;
}

char CRadarItemMgr::RadarProc(_RadarItem_fld *pRadarFld)
{
  ResetFlags();
  m_RadarResult.nSize = 0;

  if (!m_bUse || !m_pMaster || !m_pDestMap || !pRadarFld)
  {
    m_bPlayerEnd = true;
    m_bMonEnd = true;
    return 0;
  }

  if (m_bPlayerEnd && m_bMonEnd)
  {
    return 0;
  }

  CPartyPlayer *partyMgr = m_pMaster->m_pPartyMgr;
  const unsigned __int8 raceCode = m_pMaster->m_Param.GetRaceCode();

  if (pRadarFld->m_strEffSort[0] == '1')
  {
    m_bSameRace = true;
  }
  if (pRadarFld->m_strEffSort[1] == '1')
  {
    m_bNorDiffRace = true;
  }
  if (pRadarFld->m_strEffSort[2] == '1')
  {
    m_bChiefDiffRace = true;
  }
  if (pRadarFld->m_strEffSort[3] == '1')
  {
    m_bEliteMonster = true;
  }
  if (!m_bEliteMonster)
  {
    m_bMonEnd = true;
  }

  unsigned int detectedCount = 0;
  bool chunkLimited = false;

  if (!m_bPlayerEnd)
  {
    bool detectedRaceChief = false;
    int j = m_nPlayerNum;
    for (; j < MAX_PLAYER; ++j)
    {
      if (detectedCount >= 50)
      {
        m_nPlayerNum = j;
        chunkLimited = true;
        break;
      }

      CPlayer *targetPlayer = &g_Player[j];
      const unsigned int targetSerial = targetPlayer->m_Param.GetCharSerial();
      const unsigned __int8 targetRace = targetPlayer->m_Param.GetRaceCode();
      CMapData *targetMap = targetPlayer->m_pCurMap;
      if (!targetPlayer->m_bLive || targetMap != m_pDestMap)
      {
        continue;
      }

      if (m_bChiefDiffRace && raceCode != targetRace)
      {
        for (int k = 0; k < 3; ++k)
        {
          if (raceCode == static_cast<unsigned __int8>(k))
          {
            continue;
          }

          CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
          const unsigned int bossSerial = ranking->GetCurrentRaceBossSerial(k, 0);
          if (targetSerial == bossSerial)
          {
            detectedCount = AddRadarCharInfo(m_RadarResult, 3u, targetPlayer->m_fCurPos);
            detectedRaceChief = true;
            break;
          }
        }

        if (detectedRaceChief)
        {
          detectedRaceChief = false;
          continue;
        }
      }

      if (m_bNorDiffRace && raceCode != targetRace)
      {
        detectedCount = AddRadarCharInfo(m_RadarResult, 2u, targetPlayer->m_fCurPos);
        continue;
      }

      if (m_bSameRace && raceCode == targetRace && m_pMaster != targetPlayer)
      {
        CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
        const unsigned int raceBossSerial = ranking->GetCurrentRaceBossSerial(raceCode, 0);
        if (targetSerial == raceBossSerial)
        {
          detectedCount = AddRadarCharInfo(m_RadarResult, 1u, targetPlayer->m_fCurPos);
        }
        else if (partyMgr->IsPartyMode())
        {
          if (!partyMgr->IsPartyMember(targetPlayer))
          {
            detectedCount = AddRadarCharInfo(m_RadarResult, 0u, targetPlayer->m_fCurPos);
          }
        }
        else
        {
          detectedCount = AddRadarCharInfo(m_RadarResult, 0u, targetPlayer->m_fCurPos);
        }
      }
    }

    if (!chunkLimited)
    {
      m_nPlayerNum = j;
      m_bPlayerEnd = true;
    }
  }

  if (!m_bMonEnd)
  {
    chunkLimited = false;
    int j = m_nMonNum;
    for (; j < MAX_MONSTER; ++j)
    {
      if (detectedCount >= 50)
      {
        m_nMonNum = j;
        chunkLimited = true;
        break;
      }

      CMonster *monster = &g_Monster[j];
      CMapData *monsterMap = monster->m_pCurMap;
      if (monster->m_bLive && monsterMap == m_pDestMap && monster->m_pMonRec->m_nMobGrade == 2)
      {
        detectedCount = AddRadarCharInfo(m_RadarResult, 4u, monster->m_fCurPos);
      }
    }

    if (!chunkLimited)
    {
      m_nMonNum = j;
      m_bMonEnd = true;
    }
  }

  return 1;
}

void CRadarItemMgr::ResetFlags()
{
  m_bSameRace = false;
  m_bNorDiffRace = false;
  m_bChiefDiffRace = false;
  m_bEliteMonster = false;
}

void CRadarItemMgr::ResetUpdate()
{
  m_bUpdate = false;
}

unsigned int CRadarItemMgr::CalcDelay()
{
  if (!m_bUse)
  {
    return 0;
  }

  const unsigned int now = timeGetTime();
  const unsigned int elapsed = now - m_dwStartTime;
  if (elapsed >= m_dwDelayTime)
  {
    return 0;
  }

  return m_dwDelayTime - elapsed;
}

unsigned int CRadarItemMgr::GetDelayTime()
{
  return m_dwDelayTime;
}


#include "pch.h"

#include "CNuclearBomb.h"

#include "CNetProcess.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "GlobalObjects.h"
#include "nuclear_find_rader_result_zocl.h"

unsigned int CNuclearBomb::s_dwSerialCnt = 0;

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
    m_dwStartTime = GetTickCount();
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

void CNuclearBomb::SendMsg_NuclearFind(unsigned int n, unsigned __int8 race)
{
  _nuclear_find_rader_result_zocl result{};
  result.byRaceCode = race;
  result.byUseClass = GetMasterClass();

  unsigned __int8 type[2]{60, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&result), 2u);
}

__int64 CNuclearBomb::GetNewSerial()
{
  return ++s_dwSerialCnt;
}

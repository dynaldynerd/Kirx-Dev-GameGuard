#include "pch.h"

#include "CPlayerDB.h"
#include "AutominePersonalMgr.h"
#include "CPlayer.h"
#include "CRecordData.h"
#include "CGuild.h"
#include "CLogFile.h"
#include "DqsDbStructs.h"
#include "GlobalObjects.h"
#include "LendItemMng.h"
#include "TimeItem.h"
#include "UIDGenerator.h"
#include "WorldServerUtil.h"

#include <cstdlib>
#include <ctime>

int CPlayerDB::GetRaceCode()
{
  return static_cast<int>(this->m_dbChar.m_byRaceSexCode) >> 1;
}

int CPlayerDB::GetRaceSexCode()
{
  return this->m_dbChar.m_byRaceSexCode;
}

int CPlayerDB::GetMapCode()
{
  return static_cast<unsigned int>(this->m_dbChar.m_sStartMapCode);
}

void CPlayerDB::SetMapCode(unsigned __int8 byCode)
{
  this->m_dbChar.m_sStartMapCode = byCode;
}

char *CPlayerDB::GetCharNameA()
{
  return this->m_aszName;
}

char *CPlayerDB::GetCharNameW()
{
  return this->m_dbChar.m_wszCharID;
}

unsigned int CPlayerDB::GetCharSerial()
{
  return this->m_dbChar.m_dwSerial;
}

long double CPlayerDB::GetExp()
{
  return this->m_dbChar.m_dExp;
}

long double CPlayerDB::GetLossExp()
{
  return this->m_dbChar.m_dLossExp;
}

void CPlayerDB::SetExp(long double dExp)
{
  this->m_dbChar.m_dExp = dExp;
}

void CPlayerDB::SetLossExp(long double dLossExp)
{
  this->m_dbChar.m_dLossExp = dLossExp;
}

float *CPlayerDB::GetCurPos()
{
  return this->m_dbChar.m_fStartPos;
}

unsigned int CPlayerDB::GetPvpRank()
{
  return this->m_dbChar.m_dwRank;
}

long double CPlayerDB::GetPvPPoint()
{
  return this->m_dbChar.m_dPvPPoint;
}

void CPlayerDB::SetPvPPoint(long double dPoint)
{
  this->m_dbChar.m_dPvPPoint = dPoint;
}

unsigned int CPlayerDB::GetGuildSerial()
{
  if (this->m_pGuild)
  {
    return this->m_pGuild->m_dwSerial;
  }
  return static_cast<unsigned int>(-1);
}

unsigned __int8 CPlayerDB::GetClassInGuild()
{
  return m_byClassInGuild;
}

void CPlayerDB::SetClassInGuild(unsigned __int8 byClassInGuild)
{
  m_byClassInGuild = byClassInGuild;
}

unsigned int CPlayerDB::GetLevel()
{
  return this->m_dbChar.m_byLevel;
}

void CPlayerDB::SetLevel(unsigned __int8 nLv)
{
  this->m_dbChar.m_byLevel = nLv;
}

unsigned int CPlayerDB::GetMaxLevel()
{
  return this->m_dbChar.m_byMaxLevel;
}

bool CPlayerDB::IsClassChangeableLv()
{
  if (!this->m_pClassData)
  {
    return false;
  }
  const int level = static_cast<int>(this->GetLevel());
  return this->m_pClassData->m_nUpGradeLv <= level;
}

unsigned __int8 CPlayerDB::GetTrunkSlotNum()
{
  return this->m_byTrunkSlotNum;
}

unsigned __int8 CPlayerDB::GetExtTrunkSlotNum()
{
  return this->m_byExtTrunkSlotNum;
}

void CPlayerDB::SetMaxLevel(unsigned __int8 nLv)
{
  this->m_dbChar.m_byMaxLevel = nLv;
}

_class_fld *CPlayerDB::GetPtrCurClass()
{
  return this->m_pClassData;
}

_class_fld *CPlayerDB::GetPtrBaseClass()
{
  if (this->m_pClassHistory[0])
  {
    return this->m_pClassHistory[0];
  }
  return this->m_pClassData;
}

unsigned int CPlayerDB::GetHP()
{
  return this->m_dbChar.m_dwHP;
}

void CPlayerDB::SetHP(unsigned int dwHP)
{
  this->m_dbChar.m_dwHP = dwHP;
}

unsigned int CPlayerDB::GetFP()
{
  return this->m_dbChar.m_dwFP;
}

void CPlayerDB::SetFP(unsigned int dwFP)
{
  this->m_dbChar.m_dwFP = dwFP;
}

unsigned int CPlayerDB::GetSP()
{
  return this->m_dbChar.m_dwSP;
}

void CPlayerDB::SetSP(unsigned int dwSP)
{
  this->m_dbChar.m_dwSP = dwSP;
}

unsigned int CPlayerDB::GetDP()
{
  return this->m_dbChar.m_dwDP;
}

void CPlayerDB::SetDP(unsigned int dwDP)
{
  this->m_dbChar.m_dwDP = dwDP;
}

unsigned int CPlayerDB::GetDalant()
{
  return this->m_dbChar.m_dwDalant;
}

void CPlayerDB::SetDalant(unsigned int dwDt)
{
  this->m_dbChar.m_dwDalant = dwDt;
}

unsigned int CPlayerDB::GetGold()
{
  return this->m_dbChar.m_dwGold;
}

void CPlayerDB::SetGold(unsigned int dwGold)
{
  this->m_dbChar.m_dwGold = dwGold;
}

void CPlayerDB::SetCurPos(float *fPos)
{
  memcpy_0(this->m_dbChar.m_fStartPos, fPos, sizeof(this->m_dbChar.m_fStartPos));
}

unsigned __int8 CPlayerDB::GetBagNum()
{
  return this->m_dbChar.m_byUseBagNum;
}

void CPlayerDB::SetBagNum(unsigned __int8 byNum)
{
  this->m_dbChar.m_byUseBagNum = byNum;
}

int CPlayerDB::GetUseSlot()
{
  return 20 * static_cast<unsigned int>(this->m_dbChar.m_byUseBagNum);
}

unsigned __int16 CPlayerDB::GetNewItemSerial()
{
  return this->m_wSerialCount++;
}

bool CPlayerDB::BeHaveBoxOfAMP()
{
  return m_bPersonalAmineInven;
}

void CPlayerDB::SetHaveBoxOfAMP(bool bFlag)
{
  if (!m_bPersonalAmineInven)
  {
    m_bPersonalAmineInven = bFlag;
  }
}

char CPlayerDB::CalcCharGrade(unsigned __int8 byLv, unsigned __int16 wRankRate)
{
  unsigned __int16 rankRate = wRankRate;
  if (wRankRate >= 0x2710u)
  {
    rankRate = 0;
  }

  const float rate = static_cast<float>(static_cast<float>(rankRate) / 100.0f) / 100.0f;

  char buffer[48]{};
  _itoa(byLv, buffer, 10);

  _base_fld *record = g_Main.m_tblGrade.GetRecord(buffer);
  while (record)
  {
    if (*reinterpret_cast<float *>(&record[1].m_dwIndex) >= rate)
    {
      return record[1].m_strCode[0];
    }

    record = g_Main.m_tblGrade.GetRecord(record->m_dwIndex + 1);
    if (!record)
    {
      return 0;
    }
    if (strcmp_0(record->m_strCode, buffer))
    {
      return 0;
    }
  }

  return 0;
}

void CPlayerDB::InitResBuffer()
{
  if (m_wCuttingResBuffer)
  {
    for (int j = 0; j < GetMaxResKind(); ++j)
    {
      m_wCuttingResBuffer[j] = 0;
    }
  }
}

void CPlayerDB::InitAlterMastery()
{
  memset_0(m_dwAlterMastery, 0, sizeof(m_dwAlterMastery));
}

void CPlayerDB::InitClass()
{
  m_pClassData = m_pClassHistory[0];
  for (int j = 0; j < 3; ++j)
  {
    m_pClassHistory[j] = nullptr;
  }
}

void CPlayerDB::SelectClass(unsigned __int8 byHistoryRecordNum, _class_fld *pSelectClass)
{
  m_pClassHistory[byHistoryRecordNum] = m_pClassData;
  m_pClassData = pSelectClass;
  const int maxTrap =
    (m_nMakeTrapMaxNum <= pSelectClass->m_nMakeTrapMaxNum) ? pSelectClass->m_nMakeTrapMaxNum : m_nMakeTrapMaxNum;
  m_nMakeTrapMaxNum = maxTrap;
}

bool CPlayerDB::IsActableClassSkill(char *pszSkillCode, int *pnClassGrade)
{
  if (pnClassGrade)
  {
    *pnClassGrade = -1;
  }

  _class_fld *currentClass = *m_ppHistoryEffect[1];
  if (!currentClass)
  {
    currentClass = m_pClassData;
  }

  const int skillLen = static_cast<int>(strlen_0(pszSkillCode));
  _class_fld *found = nullptr;

  for (int j = 0; j < 4; ++j)
  {
    _class_fld *classInfo = *m_ppHistoryEffect[j];
    if (!classInfo)
    {
      break;
    }

    bool matched = false;
    for (int k = 0; k < 10
         && strncmp_0(reinterpret_cast<const char *>(classInfo) + (static_cast<__int64>(k) << 6) + 796, "-1", 2u);
         ++k)
    {
      if (!strncmp_0(
            reinterpret_cast<const char *>(classInfo) + (static_cast<__int64>(k) << 6) + 796,
            pszSkillCode,
            skillLen))
      {
        matched = true;
        break;
      }
    }

    if (matched)
    {
      found = classInfo;
      break;
    }
  }

  if (!found)
  {
    return false;
  }

  if (pnClassGrade)
  {
    *pnClassGrade = *reinterpret_cast<int *>(reinterpret_cast<char *>(found) + 80);
  }
  return currentClass->m_nClass == *reinterpret_cast<int *>(reinterpret_cast<char *>(found) + 72);
}

void CPlayerDB::InitPlayerDB(CPlayer *pThis)
{
  if (!m_wCuttingResBuffer)
  {
    const unsigned __int64 maxResKind = static_cast<unsigned __int64>(GetMaxResKind());
    m_wCuttingResBuffer = static_cast<unsigned __int16 *>(operator new[](saturated_mul(maxResKind, 2uLL)));
  }

  const int maxResKind = GetMaxResKind();
  memset_0(m_wCuttingResBuffer, 0, 2LL * maxResKind);
  m_pThis = pThis;
  m_wSerialCount = 0;
  InitResBuffer();
  for (int j = 0; j < 8; ++j)
  {
    m_pStoragePtr[j]->SetAllEmpty();
  }
  m_QuestDB.Init();
  m_UnitDB.Init();
  m_ItemCombineDB.Init();
  InitAlterMastery();
  m_byPvPGrade = 0;
  m_pClassData = nullptr;
  for (int j = 0; j < 3; ++j)
  {
    m_pClassHistory[j] = nullptr;
  }
  for (int j = 0; j < 50; ++j)
  {
    m_QLink[j].init();
  }
  m_ppHistoryEffect[0] = &m_pClassData;
  m_ppHistoryEffect[1] = m_pClassHistory;
  m_ppHistoryEffect[2] = &m_pClassHistory[1];
  m_ppHistoryEffect[3] = &m_pClassHistory[2];
  m_pGuild = nullptr;
  m_pGuildMemPtr = nullptr;
  m_pApplyGuild = nullptr;
  m_byClassInGuild = static_cast<unsigned __int8>(-1);
  m_bGuildLock = false;
  m_nMakeTrapMaxNum = 0;
  const int nIdx = pThis->m_id.wIndex;
  AutominePersonalMgr *mgr = AutominePersonalMgr::instance();
  m_pAPM = mgr->get_machine(nIdx);
  if (!m_pAPM)
  {
    m_bPersonalAmineInven = false;
  }
  m_PostStorage.Init();
  m_ReturnPostStorage.Init();
  m_dPvpPointLeak = 0.0;
}

_SFCONT_DB_BASE::_SFCONT_DB_BASE()
{
  Init();
}

void _SFCONT_DB_BASE::Init()
{
  for (int j = 0; j < 2; ++j)
  {
    for (int k = 0; k < 8; ++k)
    {
      m_List[j][k].Init();
    }
  }
}

void _SFCONT_DB_BASE::_LIST::Init()
{
  dwKey = static_cast<unsigned int>(-1);
}

bool _SFCONT_DB_BASE::_LIST::IsFilled() const
{
  return dwKey != static_cast<unsigned int>(-1);
}

unsigned int _SFCONT_DB_BASE::_LIST::GetEffectCode() const
{
  return (dwKey & 0x0FFFFFFF) >> 26;
}

unsigned int _SFCONT_DB_BASE::_LIST::GetEffectIndex() const
{
  return (dwKey & 0x03FFFFFF) >> 16;
}

unsigned int _SFCONT_DB_BASE::_LIST::GetLv() const
{
  return static_cast<unsigned int>(static_cast<unsigned __int16>(dwKey) >> 12);
}

unsigned int _SFCONT_DB_BASE::_LIST::GetLeftTime() const
{
  return dwKey & 0x0FFF;
}

unsigned int _SFCONT_DB_BASE::_LIST::GetOrder() const
{
  return dwKey >> 28;
}

void _SFCONT_DB_BASE::_LIST::SetKey(
  unsigned __int8 byOrder,
  unsigned __int8 byEffectCode,
  unsigned __int16 wEffectIndex,
  unsigned __int8 byLv,
  unsigned __int16 wLeftTime)
{
  dwKey = 0;
  dwKey |= static_cast<unsigned int>(byOrder) << 28;
  dwKey |= static_cast<unsigned int>(byEffectCode) << 26;
  dwKey |= static_cast<unsigned int>(wEffectIndex) << 16;
  dwKey |= static_cast<unsigned int>(byLv) << 12;
  dwKey |= static_cast<unsigned int>(wLeftTime);
}

void _SFCONT_DB_BASE::_LIST::SetLeftTime(unsigned __int16 wLeftTime)
{
  dwKey &= 0xFFFFF000;
  dwKey |= (static_cast<unsigned int>(wLeftTime) & 0x0FFF);
}

void _SFCONT_DB_BASE::_LIST::SetOrder(unsigned __int8 byOrder)
{
  dwKey = (dwKey & 0x0FFFFFFF) | (static_cast<unsigned int>(byOrder) << 28);
}

_STORAGE_LIST::_db_con *CPlayerDB::GetPtrItemStorage(unsigned __int16 wSerial, unsigned __int8 *pbyStorageCode)
{
  for (int j = 0; j < 8; ++j)
  {
    _STORAGE_LIST::_db_con *item = m_pStoragePtr[j]->GetPtrFromSerial(wSerial);
    if (item)
    {
      if (pbyStorageCode)
      {
        *pbyStorageCode = static_cast<unsigned __int8>(j);
      }
      return item;
    }
  }
  return nullptr;
}

void CPlayerDB::PopLink(int nLinkIndex)
{
  m_QLink[nLinkIndex].init();
}

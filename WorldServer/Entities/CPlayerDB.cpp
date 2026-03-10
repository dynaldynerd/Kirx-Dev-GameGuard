#include "pch.h"

#include "CPlayer.h"
#include "CPlayerDB.h"
#include "AutominePersonalMgr.h"
#include "CRecordData.h"
#include "CGuild.h"
#include "CLogFile.h"
#include "DqsDbStructs.h"
#include "GlobalObjects.h"
#include "LendItemMng.h"
#include "TimeItem.h"
#include "UIDGenerator.h"
#include "WorldServerUtil.h"
#include "grade_fld.h"

#include <cstdlib>
#include <ctime>

_bag_db_load::_bag_db_load()
{
  SetMemory(m_List, 0, 100, 0);
}

_equip_db_load::_equip_db_load()
{
  SetMemory(m_List, 1, 8, 0);
}

_embellish_db_load::_embellish_db_load()
{
  SetMemory(m_List, 2, 7, 0);
}

_force_db_load::_force_db_load()
{
  SetMemory(m_List, 3, 88, 0);
}

_animus_db_load::_animus_db_load()
{
  SetMemory(m_List, 4, 4, 0);
}

_trunk_db_load::_trunk_db_load()
{
  SetMemory(m_List, 5, 100, 0);
}

_personal_amine_inven_db_load::_personal_amine_inven_db_load()
{
  SetMemory(m_List, 6, 40, 0);
}

_Exttrunk_db_load::_Exttrunk_db_load()
{
  SetMemory(m_ExtList, 7, 40, 0);
}

CPlayerDB::CPlayerDB()
{
  m_wSerialCount = 0;
  m_pStoragePtr[0] = &m_dbInven;
  m_pStoragePtr[1] = &m_dbEquip;
  m_pStoragePtr[2] = &m_dbEmbellish;
  m_pStoragePtr[3] = &m_dbForce;
  m_pStoragePtr[4] = &m_dbAnimus;
  m_pStoragePtr[5] = &m_dbTrunk;
  m_pStoragePtr[6] = &m_dbPersonalAmineInven;
  m_pStoragePtr[7] = &m_dbExtTrunk;
  m_wCuttingResBuffer = nullptr;
  m_bPersonalAmineInven = false;
}

CPlayerDB::~CPlayerDB()
{
  if (m_wCuttingResBuffer)
  {
    operator delete[](m_wCuttingResBuffer);
    m_wCuttingResBuffer = nullptr;
  }
}

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

long double CPlayerDB::GetPvPCashBag()
{
  return this->m_dbChar.m_dPvPCashBag;
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

unsigned __int8 CPlayerDB::GetTrunkSlotRace(unsigned __int16 dwItemSerial)
{
  const int index = this->m_dbTrunk.GetIndexFromSerial(dwItemSerial);
  if (index == 255)
  {
    return static_cast<unsigned __int8>(-1);
  }
  return this->m_dbTrunk.m_byItemSlotRace[index];
}

unsigned __int8 CPlayerDB::GetExtTrunkSlotRace(unsigned __int16 dwItemSerial)
{
  const int index = this->m_dbExtTrunk.GetIndexFromSerial(dwItemSerial);
  if (index == 255)
  {
    return static_cast<unsigned __int8>(-1);
  }
  return this->m_dbExtTrunk.m_byItemSlotRace[index];
}

char *CPlayerDB::GetTrunkPasswdW()
{
  return this->m_wszTrunkPasswd;
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

void CPlayerDB::AddTrunkDalant(int dwPush)
{
  double trunkDalant = this->m_dTrunkDalant + static_cast<double>(dwPush);
  if (trunkDalant > static_cast<double>(MAX_TRUNK_DALANT) || this->m_dTrunkDalant > trunkDalant)
  {
    trunkDalant = static_cast<double>(MAX_TRUNK_DALANT);
  }
  this->m_dTrunkDalant = trunkDalant;
}

void CPlayerDB::AddTrunkGold(int dwPush)
{
  double trunkGold = this->m_dTrunkGold + static_cast<double>(dwPush);
  if (trunkGold > static_cast<double>(MAX_TRUNK_GOLD) || this->m_dTrunkGold > trunkGold)
  {
    trunkGold = static_cast<double>(MAX_TRUNK_GOLD);
  }
  this->m_dTrunkGold = trunkGold;
}

void CPlayerDB::SubTrunkDalant(int dwSub)
{
  double trunkDalant = this->m_dTrunkDalant - static_cast<double>(dwSub);
  if (static_cast<double>(dwSub) > this->m_dTrunkDalant)
  {
    trunkDalant = 0.0;
  }
  this->m_dTrunkDalant = trunkDalant;
}

void CPlayerDB::SubTrunkGold(int dwSub)
{
  double trunkGold = this->m_dTrunkGold - static_cast<double>(dwSub);
  if (static_cast<double>(dwSub) > this->m_dTrunkGold)
  {
    trunkGold = 0.0;
  }
  this->m_dTrunkGold = trunkGold;
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

__int64 CPlayerDB::GetHaveUnitNum()
{
  unsigned int count = 0;
  for (int index = 0; index < 4; ++index)
  {
    if (this->m_UnitDB.m_List[index].byFrame != static_cast<unsigned __int8>(-1))
    {
      ++count;
    }
  }
  return count;
}

__int64 CPlayerDB::GetInvenItemCountFromCode(char *pszItemCode)
{
  unsigned int totalCount = 0;
  for (int index = 0; index < this->m_dbInven.m_nUsedNum; ++index)
  {
    if (!this->m_dbInven.m_pStorageList[index].m_bLoad)
    {
      continue;
    }

    _base_fld *record = g_Main.m_tblItemData[this->m_dbInven.m_pStorageList[index].m_byTableCode].GetRecord(
      this->m_dbInven.m_pStorageList[index].m_wItemIndex);
    if (record && !strcmp_0(record->m_strCode, pszItemCode))
    {
      totalCount += static_cast<unsigned int>(this->m_dbInven.m_pStorageList[index].m_dwDur);
    }
  }
  return totalCount;
}

char CPlayerDB::DeleteItemCountFromCode(char *pszItemCode, int nCount)
{
  int remainCount = nCount;
  while (true)
  {
    _STORAGE_LIST::_db_con *item = nullptr;
    for (int index = 0; index < this->m_dbInven.m_nUsedNum; ++index)
    {
      if (!this->m_dbInven.m_pStorageList[index].m_bLoad)
      {
        continue;
      }

      _base_fld *record = g_Main.m_tblItemData[this->m_dbInven.m_pStorageList[index].m_byTableCode].GetRecord(
        this->m_dbInven.m_pStorageList[index].m_wItemIndex);
      if (record && !strcmp_0(record->m_strCode, pszItemCode))
      {
        item = &this->m_dbInven.m_pStorageList[index];
        break;
      }
    }

    if (!item)
    {
      return 0;
    }

    if (item->m_dwDur < static_cast<unsigned int>(remainCount))
    {
      const int deleteCount = static_cast<int>(item->m_dwDur);
      const unsigned __int16 newSerial = static_cast<unsigned __int16>(
        this->m_pThis->Emb_AlterDurPoint(0, item->m_byStorageIndex, -deleteCount, false, false));
      if (newSerial)
      {
        this->m_pThis->SendMsg_AdjustAmountInform(0, item->m_wSerial, newSerial);
      }
      else
      {
        CPlayer::s_MgrItemHistory.consume_del_item(
          this->m_pThis->m_ObjID.m_wIndex,
          item,
          this->m_pThis->m_szItemHistoryFileName);
      }
      remainCount -= deleteCount;
      continue;
    }

    const unsigned __int16 newSerial = static_cast<unsigned __int16>(
      this->m_pThis->Emb_AlterDurPoint(0, item->m_byStorageIndex, -remainCount, false, false));
    if (newSerial)
    {
      this->m_pThis->SendMsg_AdjustAmountInform(0, item->m_wSerial, newSerial);
    }
    else
    {
      CPlayer::s_MgrItemHistory.consume_del_item(this->m_pThis->m_ObjID.m_wIndex, item, this->m_pThis->m_szItemHistoryFileName);
    }
    return 1;
  }
}

_STORAGE_LIST::_db_con *CPlayerDB::GetItem(unsigned __int8 byInvenIndex)
{
  const unsigned __int8 maxSlot = static_cast<unsigned __int8>(GetUseSlot());
  if (byInvenIndex > maxSlot)
  {
    return nullptr;
  }

  _STORAGE_LIST::_db_con *item = &this->m_dbInven.m_pStorageList[byInvenIndex];
  if (item->m_bLoad)
  {
    return item;
  }
  return nullptr;
}

unsigned __int16 CPlayerDB::GetCurItemSerial()
{
  return m_wSerialCount;
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
  if (wRankRate >= 10000)
  {
    rankRate = 0;
  }

  const float rankingRate = (static_cast<float>(rankRate) / 100.0f) / 100.0f;

  char levelCode[48]{};
  _itoa(byLv, levelCode, 10);

  _grade_fld *record = reinterpret_cast<_grade_fld *>(g_Main.m_tblGrade.GetRecord(levelCode));
  while (record)
  {
    if (record->m_fRanking >= rankingRate)
    {
      return static_cast<char>(record->m_nGrade);
    }

    record = reinterpret_cast<_grade_fld *>(g_Main.m_tblGrade.GetRecord(record->m_dwIndex + 1));
    if (!record)
    {
      return 0;
    }

    if (strcmp_0(record->m_strCode, levelCode))
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

char CPlayerDB::GetResBufferNum()
{
  char count = 0;
  for (int index = 0; index < GetMaxResKind(); ++index)
  {
    if (this->m_wCuttingResBuffer[index])
    {
      ++count;
    }
  }
  return count;
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

char CPlayerDB::ConvertAvatorDB(_AVATOR_DATA *pData)
{
  strcpy_0(this->m_dbChar.m_wszCharID, pData->dbAvator.m_wszAvatorName);
  W2M(this->m_dbChar.m_wszCharID, this->m_aszName, 17);
  this->m_byNameLen = static_cast<unsigned __int8>(strlen_0(this->m_dbChar.m_wszCharID));
  this->m_dbChar.m_dwSerial = pData->dbAvator.m_dwRecordNum;
  this->m_dbChar.m_byRaceSexCode = pData->dbAvator.m_byRaceSexCode;
  this->m_dbChar.m_dwHP = pData->dbAvator.m_dwHP;
  this->m_dbChar.m_dwFP = pData->dbAvator.m_dwFP;
  this->m_dbChar.m_dwSP = pData->dbAvator.m_dwSP;
  this->m_dbChar.m_dwDP = pData->dbAvator.m_dwDP;
  this->m_dbChar.m_dExp = pData->dbAvator.m_dExp;
  this->m_dbChar.m_dLossExp = pData->dbAvator.m_dLossExp;
  this->m_dbChar.m_dwDalant = pData->dbAvator.m_dwDalant;
  this->m_dbChar.m_dwGold = pData->dbAvator.m_dwGold;
  this->m_dbChar.m_dwRank = pData->dbAvator.m_dwPvpRank;
  this->m_dbChar.m_wRankRate = pData->dbAvator.m_wRankRate;
  this->m_dbChar.m_dPvPPoint = pData->dbAvator.m_dPvPPoint;
  this->m_dbChar.m_dPvPCashBag = pData->dbAvator.m_dPvPCashBag;

  for (int index = 0; index < 5; ++index)
  {
    const unsigned int part = (pData->dbAvator.m_dwBaseShape >> (4 * index)) & 0xF;
    this->m_dbChar.m_byDftPart[index] =
      static_cast<unsigned __int8>(16 * pData->dbAvator.m_byRaceSexCode + part);
  }

  const unsigned int face = (pData->dbAvator.m_dwBaseShape >> 20) & 0xF;
  this->m_dbChar.m_byDftPart_Face =
    static_cast<unsigned __int8>(16 * pData->dbAvator.m_byRaceSexCode + face);

  this->m_dbChar.m_byUseBagNum = pData->dbAvator.m_byBagNum;
  this->m_dbChar.m_byLevel = pData->dbAvator.m_byLevel;
  this->m_dbChar.m_sStartMapCode = pData->dbAvator.m_byMapCode;
  memcpy_0(this->m_dbChar.m_fStartPos, pData->dbAvator.m_fStartPos, sizeof(this->m_dbChar.m_fStartPos));
  this->m_dbChar.m_byMaxLevel = pData->dbAvator.m_byMaxLevel;
  this->m_dbInven.SetUseListNum(20 * pData->dbAvator.m_byBagNum);

  this->m_pClassData = reinterpret_cast<_class_fld *>(g_Main.m_tblClass.GetRecord(pData->dbAvator.m_szClassCode));
  if (!this->m_pClassData)
  {
    return 0;
  }

  this->m_nMakeTrapMaxNum = this->m_pClassData->m_nMakeTrapMaxNum;
  for (int index = 0; index < 3 && pData->dbAvator.m_zClassHistory[index] != -1; ++index)
  {
    _class_fld *record = reinterpret_cast<_class_fld *>(
      g_Main.m_tblClass.GetRecord(pData->dbAvator.m_zClassHistory[index]));
    if (!record)
    {
      return 0;
    }

    this->m_pClassHistory[index] = record;
    this->m_nMakeTrapMaxNum =
      (this->m_nMakeTrapMaxNum <= record->m_nMakeTrapMaxNum) ? record->m_nMakeTrapMaxNum : this->m_nMakeTrapMaxNum;
  }

  this->m_byPvPGrade = CPlayerDB::CalcCharGrade(this->m_dbChar.m_byLevel, this->m_dbChar.m_wRankRate);
  this->m_pGuild = nullptr;
  this->m_pGuildMemPtr = nullptr;

  if (pData->dbAvator.m_dwGuildSerial == static_cast<unsigned int>(-1))
  {
    this->m_pGuild = nullptr;
    this->m_pGuildMemPtr = nullptr;
  }
  else
  {
    for (int index = 0; index < MAX_GUILD; ++index)
    {
      CGuild *guild = &g_Guild[index];
      if (guild->IsFill() && guild->m_dwSerial == pData->dbAvator.m_dwGuildSerial)
      {
        this->m_pGuild = guild;
        this->m_pGuildMemPtr = guild->GetMemberFromSerial(this->m_dbChar.m_dwSerial);
        if (!this->m_pGuildMemPtr)
        {
          g_Main.m_logSystemError.Write(
            "Guild Member Fail: guild:%s, name:%s",
            this->m_pGuild->m_aszName,
            this->m_aszName);
          this->m_pGuild = nullptr;
        }
        this->m_byClassInGuild = pData->dbAvator.m_byClassInGuild;
        return 1;
      }
    }
  }

  return 1;
}

char CPlayerDB::ConvertGeneralDB(_AVATOR_DATA *pData, _AVATOR_DATA *pOutData)
{
  __time32_t now{};
  _time32(&now);
  const unsigned int nowSec = static_cast<unsigned int>(now);

  const int maxInven = 20 * pData->dbAvator.m_byBagNum;
  for (int index = 0; index < maxInven; ++index)
  {
    _INVEN_DB_BASE::_LIST &inven = pData->dbInven.m_List[index];
    if (!inven.Key.IsFilled())
    {
      continue;
    }

    _STORAGE_LIST::_db_con *storage = &this->m_dbInven.m_pStorageList[index];
    storage->m_byTableCode = inven.Key.byTableCode;
    storage->m_byClientIndex = inven.Key.bySlotIndex;
    storage->m_wItemIndex = inven.Key.wItemIndex;
    storage->m_dwDur = inven.dwDur;
    storage->m_dwLv = inven.dwUpt;
    storage->m_bLoad = 1;
    storage->SetSerialNumber(inven.dwItemETSerial);

    pOutData->dbInven.m_List[index].dwItemETSerial = storage->GetSerialNumber();
    if (inven.lnUID)
    {
      storage->m_lnUID = inven.lnUID;
    }
    else
    {
      storage->m_lnUID = UIDGenerator::getuid(g_Main.m_byWorldCode);
      pOutData->dbInven.m_List[index].lnUID = storage->m_lnUID;
    }

    const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(storage->m_byTableCode, storage->m_wItemIndex);
    if (timeRec && timeRec->m_nCheckType)
    {
      if (timeRec->m_nCheckType == 1)
      {
        storage->m_dwT = inven.dwT + nowSec;
      }
      else if (timeRec->m_nCheckType == 2)
      {
        storage->m_dwT = inven.dwT;
      }

      inven.byCsMethod = timeRec->m_nCheckType;
      storage->m_byCsMethod = inven.byCsMethod;
      inven.dwLendRegdTime = nowSec;
      storage->m_dwLendRegdTime = nowSec;
      pOutData->dbInven.m_List[index].byCsMethod = storage->m_byCsMethod;
      pOutData->dbInven.m_List[index].dwT = storage->m_dwT;

      LendItemMng::Instance()->InsertLink(this->m_pThis->m_ObjID.m_wIndex, 0, storage);
    }
    else
    {
      inven.byCsMethod = 0;
      storage->m_byCsMethod = 0;
    }
  }

  for (int index = 0; index < 8; ++index)
  {
    _EQUIPKEY &equipKey = pData->dbAvator.m_EquipKey[index];
    if (!equipKey.IsFilled())
    {
      continue;
    }

    _STORAGE_LIST::_db_con *storage = &this->m_dbEquip.m_pStorageList[index];
    storage->m_byTableCode = static_cast<unsigned __int8>(index);
    storage->m_byClientIndex = static_cast<unsigned __int8>(index);
    storage->m_wItemIndex = static_cast<unsigned __int16>(equipKey.zItemIndex);
    storage->m_dwDur = 0;
    storage->m_dwLv = pData->dbAvator.m_dwFixEquipLv[index];
    storage->m_bLoad = 1;
    storage->m_bLock = 0;
    storage->SetSerialNumber(pData->dbAvator.m_dwItemETSerial[index]);

    pOutData->dbAvator.m_dwItemETSerial[index] = storage->GetSerialNumber();
    if (pData->dbAvator.m_lnUID[index])
    {
      storage->m_lnUID = pData->dbAvator.m_lnUID[index];
    }
    else
    {
      storage->m_lnUID = UIDGenerator::getuid(g_Main.m_byWorldCode);
      pOutData->dbAvator.m_lnUID[index] = storage->m_lnUID;
    }

    const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(storage->m_byTableCode, storage->m_wItemIndex);
    if (timeRec && timeRec->m_nCheckType)
    {
      if (timeRec->m_nCheckType == 1)
      {
        storage->m_dwT = pData->dbAvator.m_dwET[index] + nowSec;
      }
      else if (timeRec->m_nCheckType == 2)
      {
        storage->m_dwT = pData->dbAvator.m_dwET[index];
      }

      pData->dbAvator.m_byCsMethod[index] = timeRec->m_nCheckType;
      storage->m_byCsMethod = pData->dbAvator.m_byCsMethod[index];
      pData->dbAvator.m_dwLendRegdTime[index] = nowSec;
      storage->m_dwLendRegdTime = nowSec;

      LendItemMng::Instance()->InsertLink(this->m_pThis->m_ObjID.m_wIndex, 1u, storage);
    }
    else
    {
      pData->dbAvator.m_byCsMethod[index] = 0;
      storage->m_byCsMethod = 0;
    }

    this->m_pThis->SetEffectEquipCode(1u, static_cast<unsigned __int8>(index), 2u);
  }

  for (int index = 0; index < 7; ++index)
  {
    _EQUIP_DB_BASE::_EMBELLISH_LIST &embellish = pData->dbEquip.m_EmbellishList[index];
    if (!embellish.Key.IsFilled())
    {
      continue;
    }

    _STORAGE_LIST::_db_con *storage = &this->m_dbEmbellish.m_pStorageList[index];
    storage->m_byTableCode = embellish.Key.byTableCode;
    storage->m_byClientIndex = embellish.Key.bySlotIndex;
    storage->m_wItemIndex = embellish.Key.wItemIndex;
    storage->m_dwDur = embellish.wAmount;
    storage->m_dwLv = 268435455;
    storage->m_bLoad = 1;
    storage->m_bLock = 0;
    storage->SetSerialNumber(embellish.dwItemETSerial);

    pOutData->dbEquip.m_EmbellishList[index].dwItemETSerial = storage->GetSerialNumber();
    if (embellish.lnUID)
    {
      storage->m_lnUID = embellish.lnUID;
    }
    else
    {
      storage->m_lnUID = UIDGenerator::getuid(g_Main.m_byWorldCode);
      pOutData->dbEquip.m_EmbellishList[index].lnUID = storage->m_lnUID;
    }

    const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(storage->m_byTableCode, storage->m_wItemIndex);
    if (timeRec && timeRec->m_nCheckType)
    {
      if (timeRec->m_nCheckType == 1)
      {
        storage->m_dwT = embellish.dwT + nowSec;
      }
      else if (timeRec->m_nCheckType == 2)
      {
        storage->m_dwT = embellish.dwT;
      }

      embellish.byCsMethod = timeRec->m_nCheckType;
      storage->m_byCsMethod = embellish.byCsMethod;
      embellish.dwLendRegdTime = nowSec;
      storage->m_dwLendRegdTime = nowSec;

      LendItemMng::Instance()->InsertLink(this->m_pThis->m_ObjID.m_wIndex, 2u, storage);
    }
    else
    {
      embellish.byCsMethod = 0;
      storage->m_byCsMethod = 0;
    }

    this->m_pThis->SetEffectEquipCode(2u, static_cast<unsigned __int8>(index), 2u);
  }

  for (int index = 0; index < 88; ++index)
  {
    _FORCE_DB_BASE::_LIST &force = pData->dbForce.m_List[index];
    if (!force.Key.IsFilled())
    {
      continue;
    }

    _STORAGE_LIST::_db_con *storage = &this->m_dbForce.m_pStorageList[index];
    storage->m_byTableCode = 15;
    storage->m_wItemIndex = force.Key.GetIndex();
    storage->m_dwDur = force.Key.GetStat();
    storage->m_dwLv = 268435455;
    storage->m_bLoad = 1;
    storage->m_bLock = 0;
    storage->SetSerialNumber(force.dwItemETSerial);

    pOutData->dbForce.m_List[index].dwItemETSerial = storage->GetSerialNumber();
    if (force.lnUID)
    {
      storage->m_lnUID = force.lnUID;
    }
    else
    {
      storage->m_lnUID = UIDGenerator::getuid(g_Main.m_byWorldCode);
      pOutData->dbForce.m_List[index].lnUID = storage->m_lnUID;
    }

    const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(storage->m_byTableCode, storage->m_wItemIndex);
    if (timeRec && timeRec->m_nCheckType)
    {
      if (timeRec->m_nCheckType == 1)
      {
        storage->m_dwT = force.dwT + nowSec;
      }
      else if (timeRec->m_nCheckType == 2)
      {
        storage->m_dwT = force.dwT;
      }

      force.byCsMethod = timeRec->m_nCheckType;
      storage->m_byCsMethod = force.byCsMethod;
      force.m_dwLendRegdTime = nowSec;
      storage->m_dwLendRegdTime = nowSec;

      LendItemMng::Instance()->InsertLink(this->m_pThis->m_ObjID.m_wIndex, 3u, storage);
    }
    else
    {
      force.byCsMethod = 0;
      storage->m_byCsMethod = 0;
    }
  }

  for (int index = 0; index < 20; ++index)
  {
    if (pData->dbCutting.m_List[index].Key.IsFilled())
    {
      this->m_wCuttingResBuffer[pData->dbCutting.m_List[index].Key.wItemIndex] =
        pData->dbCutting.m_List[index].dwDur;
    }
  }

  memcpy_0(&this->m_QuestDB, &pData->dbQuest, sizeof(this->m_QuestDB));
  memcpy_0(&this->m_UnitDB, &pData->dbUnit, sizeof(this->m_UnitDB));
  memcpy_0(&this->m_ItemCombineDB, &pData->dbItemCombineEx, sizeof(this->m_ItemCombineDB));

  for (int index = 0; index < 4; ++index)
  {
    _ANIMUS_DB_BASE::_LIST &animus = pData->dbAnimus.m_List[index];
    if (!animus.Key.IsFilled())
    {
      continue;
    }

    _STORAGE_LIST::_db_con *storage = &this->m_dbAnimus.m_pStorageList[index];
    storage->m_byTableCode = 24;
    storage->m_wItemIndex = animus.Key.byItemIndex;
    storage->m_dwDur = animus.dwExp;
    storage->m_dwLv = animus.dwParam;
    storage->m_bLoad = 1;
    storage->m_bLock = 0;
    storage->SetSerialNumber(animus.dwItemETSerial);

    pOutData->dbAnimus.m_List[index].dwItemETSerial = storage->GetSerialNumber();
    if (animus.lnUID)
    {
      storage->m_lnUID = animus.lnUID;
    }
    else
    {
      storage->m_lnUID = UIDGenerator::getuid(g_Main.m_byWorldCode);
      pOutData->dbAnimus.m_List[index].lnUID = storage->m_lnUID;
    }

    const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(storage->m_byTableCode, storage->m_wItemIndex);
    if (timeRec && timeRec->m_nCheckType)
    {
      if (timeRec->m_nCheckType == 1)
      {
        storage->m_dwT = animus.dwT + nowSec;
      }
      else if (timeRec->m_nCheckType == 2)
      {
        storage->m_dwT = animus.dwT;
      }

      animus.byCsMethod = timeRec->m_nCheckType;
      storage->m_byCsMethod = animus.byCsMethod;
      animus.dwLendRegdTime = nowSec;
      storage->m_dwLendRegdTime = nowSec;

      LendItemMng::Instance()->InsertLink(this->m_pThis->m_ObjID.m_wIndex, 4u, storage);
    }
    else
    {
      animus.byCsMethod = 0;
      storage->m_byCsMethod = 0;
    }
  }

  strcpy_0(this->m_wszTrunkPasswd, pData->dbTrunk.wszPasswd);
  this->m_dTrunkDalant = pData->dbTrunk.dDalant;
  this->m_dTrunkGold = pData->dbTrunk.dGold;
  this->m_byTrunkHintIndex = pData->dbTrunk.byHintIndex;
  strcpy_0(this->m_wszTrunkHintAnswer, pData->dbTrunk.wszHintAnswer);
  this->m_byTrunkSlotNum = pData->dbTrunk.bySlotNum;

  for (int index = 0; index < this->m_byTrunkSlotNum; ++index)
  {
    _TRUNK_DB_BASE::_LIST &trunk = pData->dbTrunk.m_List[index];
    if (!trunk.Key.IsFilled())
    {
      continue;
    }

    _STORAGE_LIST::_db_con *storage = &this->m_dbTrunk.m_pStorageList[index];
    storage->m_byTableCode = trunk.Key.byTableCode;
    storage->m_byClientIndex = trunk.Key.bySlotIndex;
    storage->m_wItemIndex = trunk.Key.wItemIndex;
    storage->m_dwDur = trunk.dwDur;
    storage->m_dwLv = trunk.dwUpt;
    storage->m_bLoad = 1;
    storage->m_bLock = 0;
    storage->SetSerialNumber(trunk.dwItemETSerial);

    pOutData->dbTrunk.m_List[index].dwItemETSerial = storage->GetSerialNumber();
    this->m_dbTrunk.m_byItemSlotRace[index] = trunk.byRace;

    if (trunk.lnUID)
    {
      storage->m_lnUID = trunk.lnUID;
    }
    else
    {
      storage->m_lnUID = UIDGenerator::getuid(g_Main.m_byWorldCode);
      pOutData->dbTrunk.m_List[index].lnUID = storage->m_lnUID;
    }

    const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(storage->m_byTableCode, storage->m_wItemIndex);
    if (timeRec && timeRec->m_nCheckType)
    {
      if (timeRec->m_nCheckType == 1)
      {
        storage->m_dwT = trunk.dwT + nowSec;
      }
      else if (timeRec->m_nCheckType == 2)
      {
        storage->m_dwT = trunk.dwT;
      }

      trunk.byCsMethod = timeRec->m_nCheckType;
      storage->m_byCsMethod = trunk.byCsMethod;
      trunk.dwLendRegdTime = nowSec;
      storage->m_dwLendRegdTime = nowSec;

      LendItemMng::Instance()->InsertLink(this->m_pThis->m_ObjID.m_wIndex, 5u, storage);
    }
    else
    {
      trunk.byCsMethod = 0;
      storage->m_byCsMethod = 0;
    }
  }

  this->m_byExtTrunkSlotNum = pData->dbTrunk.byExtSlotNum;
  for (int index = 0; index < this->m_byExtTrunkSlotNum; ++index)
  {
    _TRUNK_DB_BASE::_LIST &extTrunk = pData->dbTrunk.m_ExtList[index];
    if (!extTrunk.Key.IsFilled())
    {
      continue;
    }

    _STORAGE_LIST::_db_con *storage = &this->m_dbExtTrunk.m_pStorageList[index];
    storage->m_byTableCode = extTrunk.Key.byTableCode;
    storage->m_byClientIndex = extTrunk.Key.bySlotIndex;
    storage->m_wItemIndex = extTrunk.Key.wItemIndex;
    storage->m_dwDur = extTrunk.dwDur;
    storage->m_dwLv = extTrunk.dwUpt;
    storage->m_bLoad = 1;
    storage->m_bLock = 0;
    storage->SetSerialNumber(extTrunk.dwItemETSerial);

    pOutData->dbTrunk.m_ExtList[index].dwItemETSerial = storage->GetSerialNumber();
    this->m_dbExtTrunk.m_byItemSlotRace[index] = extTrunk.byRace;

    if (extTrunk.lnUID)
    {
      storage->m_lnUID = extTrunk.lnUID;
    }
    else
    {
      storage->m_lnUID = UIDGenerator::getuid(g_Main.m_byWorldCode);
      pOutData->dbTrunk.m_ExtList[index].lnUID = storage->m_lnUID;
    }

    const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(storage->m_byTableCode, storage->m_wItemIndex);
    if (timeRec && timeRec->m_nCheckType)
    {
      if (timeRec->m_nCheckType == 1)
      {
        storage->m_dwT = extTrunk.dwT + nowSec;
      }
      else if (timeRec->m_nCheckType == 2)
      {
        storage->m_dwT = extTrunk.dwT;
      }

      extTrunk.byCsMethod = timeRec->m_nCheckType;
      storage->m_byCsMethod = extTrunk.byCsMethod;
      extTrunk.dwLendRegdTime = nowSec;
      storage->m_dwLendRegdTime = nowSec;

      LendItemMng::Instance()->InsertLink(this->m_pThis->m_ObjID.m_wIndex, 7u, storage);
    }
    else
    {
      extTrunk.byCsMethod = 0;
      storage->m_byCsMethod = 0;
    }
  }

  this->m_bTrunkOpen = false;
  this->m_dbTrunk.SetUseListNum(this->m_byTrunkSlotNum);
  this->m_dbExtTrunk.SetUseListNum(this->m_byExtTrunkSlotNum);

  this->m_bPersonalAmineInven = pData->dbPersonalAmineInven.bUsable;
  for (int index = 0; index < 40; ++index)
  {
    _PERSONALAMINE_INVEN_DB_BASE::_LIST &ampItem = pData->dbPersonalAmineInven.m_List[index];
    if (!ampItem.Key.IsFilled())
    {
      continue;
    }

    _STORAGE_LIST::_db_con *storage = &this->m_dbPersonalAmineInven.m_pStorageList[index];
    storage->m_byTableCode = ampItem.Key.byTableCode;
    storage->m_byClientIndex = ampItem.Key.bySlotIndex;
    storage->m_wItemIndex = ampItem.Key.wItemIndex;
    storage->m_dwDur = ampItem.dwDur;
    storage->m_bLoad = 1;
    storage->m_bLock = 0;
  }

  if (this->m_pAPM)
  {
    this->m_pAPM->LoadDBComplete();
  }

  AppointSerialStorageItem();

  for (int index = 0; index < 50; ++index)
  {
    _LINK_DB_BASE::_LIST &link = pData->dbLink.m_LinkList[index];
    if (!link.Key.IsFilled() || link.Key.GetCode() != 4)
    {
      continue;
    }

    const unsigned __int16 linkIndex = link.Key.GetIndex();
    const unsigned __int8 storageCode = static_cast<unsigned __int8>(linkIndex >> 8);
    const unsigned __int8 storageIndex = static_cast<unsigned __int8>(linkIndex);
    if (IsStorageRange(storageCode, storageIndex))
    {
      _STORAGE_LIST::_db_con *storage = &this->m_pStoragePtr[storageCode]->m_pStorageList[storageIndex];
      if (storage->m_bLoad)
      {
        if (!PushLink(index, storage->m_wSerial, true))
        {
          link.Init();
        }
        continue;
      }
    }

    link.Init();
  }

  this->m_dPvpPointLeak = pData->dbSupplement.dPvpPointLeak;
  return 1;
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

void CPlayerDB::AppointSerialStorageItem()
{
  for (int index = 0; index < 8; ++index)
  {
    if (this->m_dbEquip.m_pStorageList[index].m_bLoad)
    {
      this->m_dbEquip.m_pStorageList[index].m_wSerial = this->GetNewItemSerial();
    }
  }

  for (int index = 0; index < 7; ++index)
  {
    if (this->m_dbEmbellish.m_pStorageList[index].m_bLoad)
    {
      this->m_dbEmbellish.m_pStorageList[index].m_wSerial = this->GetNewItemSerial();
    }
  }

  for (int index = 0; index < 100; ++index)
  {
    if (this->m_dbInven.m_pStorageList[index].m_bLoad)
    {
      this->m_dbInven.m_pStorageList[index].m_wSerial = this->GetNewItemSerial();
    }
  }

  for (int index = 0; index < 88; ++index)
  {
    if (this->m_dbForce.m_pStorageList[index].m_bLoad)
    {
      this->m_dbForce.m_pStorageList[index].m_wSerial = this->GetNewItemSerial();
    }
  }

  for (int index = 0; index < 4; ++index)
  {
    if (this->m_dbAnimus.m_pStorageList[index].m_bLoad)
    {
      this->m_dbAnimus.m_pStorageList[index].m_wSerial = this->GetNewItemSerial();
    }
  }

  for (int index = 0; index < 40; ++index)
  {
    if (this->m_dbPersonalAmineInven.m_pStorageList[index].m_bLoad)
    {
      this->m_dbPersonalAmineInven.m_pStorageList[index].m_wSerial = this->GetNewItemSerial();
    }
  }

  for (int index = 0; index < 100; ++index)
  {
    if (this->m_dbTrunk.m_pStorageList[index].m_bLoad)
    {
      this->m_dbTrunk.m_pStorageList[index].m_wSerial = this->GetNewItemSerial();
    }
  }

  for (int index = 0; index < 40; ++index)
  {
    if (this->m_dbExtTrunk.m_pStorageList[index].m_bLoad)
    {
      this->m_dbExtTrunk.m_pStorageList[index].m_wSerial = this->GetNewItemSerial();
    }
  }
}

char CPlayerDB::PushLink(int nLinkIndex, unsigned __int16 wSerial, bool bInit)
{
  if (bInit)
  {
    for (int index = 0; index < 50; ++index)
    {
      if (this->m_QLink[index].byLinkIndex != 255 && this->m_QLink[index].wSerial == wSerial)
      {
        return 0;
      }
    }
  }

  this->m_QLink[nLinkIndex].byLinkIndex = static_cast<unsigned __int8>(nLinkIndex);
  this->m_QLink[nLinkIndex].wSerial = wSerial;
  return 1;
}

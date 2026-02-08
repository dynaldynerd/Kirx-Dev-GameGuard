#include "pch.h"

#include "CMainThread.h"

#include <ctime>
#include <new>

#include <mmsystem.h>

#include "AutominePersonalMgr.h"
#include "CCheckSumCharacAccountTrunkData.h"
#include "CLogFile.h"
#include "CRFWorldDatabase.h"
#include "DqsDbStructs.h"
#include "GlobalObjects.h"
#include "TimeItem.h"
#include "TimeItem_fld.h"
#include "WorldServerUtil.h"

unsigned __int8 CMainThread::db_Load_Avator(
  unsigned int dwSerial,
  unsigned int dwAccountSerial,
  _AVATOR_DATA *pData,
  bool *pbAddItem,
  unsigned int *pdwAddDalant,
  unsigned int *pdwAddGold,
  bool *pbTrunkAddItem,
  unsigned __int8 *pbyTrunkOldSlot,
  long double *pdTrunkOldDalant,
  long double *pdTrunkOldGold,
  bool *pbCreateTrunkFree,
  bool *pbExtTrunkAddItem,
  unsigned __int8 *pbyExtTrunkOldSlot,
  bool bAll,
  unsigned int *pdwCheckSum)
{
  unsigned __int8 result = 0;
  if (pdwAddDalant)
  {
    *pdwAddDalant = 0;
  }
  if (pdwAddGold)
  {
    *pdwAddGold = 0;
  }

  if (bAll)
  {
    result = _db_Load_Base(dwSerial, pData);
    if (result)
    {
      m_logSystemError.Write( "_db_Load_Base(%d) => failed ..{%d}", dwSerial, result);
      return result;
    }
  }
  else
  {
    CRFWorldDatabase::Select_CheckSumValue(m_pWorldDB, dwSerial, pdwCheckSum);
  }

  pData->dbAvator.m_dwAccountSerial = dwAccountSerial;
  result = _db_Load_General(dwSerial, pData->dbAvator.m_byRaceSexCode / 2, pData);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_General(%d) => failed ..{%d}", dwSerial, result);
    return result;
  }

  result = _db_Load_Supplement(dwSerial, &pData->dbSupplement);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_Supplement(%d) => failed ..{%d}", dwSerial, result);
    return result;
  }

  result = _db_Load_TimeLimitInfo(dwAccountSerial, &pData->dbTimeLimitInfo);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_TimeLimitInfo(%d) => failed ..{%d}", dwAccountSerial, result);
    return result;
  }

  result = _db_Load_Inven(dwSerial, pData->dbAvator.m_byBagNum, &pData->dbInven);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_Inven(%d) => failed ..{%d}", dwSerial, result);
    return result;
  }

  if (!(static_cast<int>(pData->dbAvator.m_byRaceSexCode) >> 1))
  {
    result = _db_Load_Unit(dwSerial, &pData->dbUnit);
    if (result)
    {
      m_logSystemError.Write( "_db_Load_Unit(%d) => failed ..{%d}", dwSerial, result);
      return result;
    }
  }

  result = _db_Load_UI(dwSerial, &pData->dbLink, &pData->dbSfcont);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_UI(%d) => failed ..{%d}", dwSerial, result);
    return result;
  }

  result = _db_Load_Quest(dwSerial, &pData->dbQuest);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_Quest(%d) => failed ..{%d}", dwSerial, result);
    return result;
  }

  result = _db_Load_NpcQuest_History(dwSerial, &pData->dbQuest);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_NpcQuest_History(%d) => failed ..{%d}", dwSerial, result);
    return result;
  }

  result = _db_Load_Start_NpcQuest_History(dwSerial, pData->dbAvator.m_byRaceSexCode / 2, &pData->dbQuest);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_Start_NpcQuest_History(%d) => failed ..{%d}", dwSerial, result);
    return result;
  }

  result = _db_Load_Buddy(dwSerial, &pData->dbBuddy);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_Buddy(%d) => failed ..{%d}", dwSerial, result);
    return result;
  }

  if (dwAccountSerial)
  {
    result = _db_Load_Trunk(dwSerial, dwAccountSerial, pData->dbAvator.m_byRaceSexCode / 2, &pData->dbTrunk);
    if (result)
    {
      m_logSystemError.Write( "_db_Load_Trunk(%d) => failed ..{%d}", dwSerial, result);
      return result;
    }
  }

  result = _db_Load_Trade(pData->dbAvator.m_byRaceSexCode / 2, dwSerial, &pData->dbTrade);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_Trade(%d) => failed ..{%d}", dwSerial, result);
    return result;
  }

  result = _db_Load_ItemCombineEx(dwSerial, &pData->dbItemCombineEx);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_ItemCombineEx(%d) => failed ..{%d}", dwSerial, result);
    return result;
  }

  result = _db_Load_MacroData(dwSerial, &pData->dbMacro);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_MacroData(%d) => failed..{%d}", dwSerial, result);
    return result;
  }

  pData->m_byHSKTime = static_cast<unsigned __int8>(-1);
  pData->m_byCristalBattleDBInfo = 3;
  pData->m_iPvpPoint = 0;
  pData->m_wKillPoint = 0;
  pData->m_wDiePoint = 0;

  result = _db_Load_PvpPointLimitData(dwSerial, &pData->dbPvpPointLimit);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_PvpPointLimitData(%d) => failed..{%d}", dwSerial, result);
    return result;
  }

  result = _db_Load_CryMsg(dwSerial, &pData->dbBossCry);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_CryMsg(%d) => failed ..{%d}", dwSerial, result);
    return result;
  }

  result = _db_Load_PvpOrderView(dwSerial, &pData->dbPvpOrderView);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_PvpOrderViewPo(%d) => failed..{%d}", dwSerial, result);
    return result;
  }

  result = _db_Load_SFDelayData(dwSerial, &pData->dbSFDelay);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_SFDelayData(%d) => failed ..{%d}", dwSerial, result);
    return result;
  }

  result = _db_Load_PrimiumPlayTime(dwAccountSerial, &pData->dbPlayTimeInPcbang);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_PrimiumPlayTime(%d) => failed..{%d}", dwAccountSerial, result);
    return result;
  }

  result = _db_Load_PotionDelay(dwSerial, &pData->dbPotionNextUseTime);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_PotionDelay(%d) => failed..{%d}", dwSerial, result);
    return result;
  }

  result = _db_Load_OreCutting(dwSerial, &pData->dbCutting);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_OreCutting(%d) => failed..{%d}", dwSerial, result);
    return result;
  }

  result = _db_Load_PcBangFavor(dwSerial, &pData->dbPcBangFavorItem);
  if (result)
  {
    m_logSystemError.Write( "_db_Load_PcBangFavor(%d) => failed..{%d}", dwSerial, result);
    return result;
  }

  if (!bAll && _db_Check_NpcData(dwSerial, pData) == 40)
  {
    return 40;
  }

  char szToday[44]{};
  GetTodayStr(szToday);

  unsigned int pdwRank[8]{};
  unsigned __int16 pwRankRate[8]{};
  pwRankRate[0] = static_cast<unsigned __int16>(-1);

  unsigned __int8 pvpRate = CRFWorldDatabase::Select_PvpRate(m_pWorldDB, dwSerial, szToday, pdwRank, pwRankRate);
  if (pvpRate)
  {
    if (pvpRate == 2)
    {
      pData->dbAvator.m_dwPvpRank = static_cast<unsigned int>(-1);
      pData->dbAvator.m_wRankRate = static_cast<unsigned __int16>(-1);
    }
    else if (pvpRate == 1)
    {
      m_logSystemError.Write( "Select_PvpRate(%d) => failed ..{%d}", dwSerial, result);
      pData->dbAvator.m_dwPvpRank = 0;
      pData->dbAvator.m_wRankRate = static_cast<unsigned __int16>(-1);
    }
  }
  else
  {
    pData->dbAvator.m_dwPvpRank = pdwRank[0];
    pData->dbAvator.m_wRankRate = pwRankRate[0];
  }

  result = _db_load_raceboss(dwSerial, pData);
  if (result)
  {
    m_logSystemError.Write(
      "CMainThread::db_Load_Avator(...) : Ret(%u) _db_load_raceboss(dwSerial(%u), pData) Fail!",
      result,
      dwSerial);
    return result;
  }

  AutominePersonalMgr *autoMineMgr = AutominePersonalMgr::instance();
  pData->dbPersonalAmineInven.bUsable = AutominePersonalMgr::db_load_inven(autoMineMgr, dwSerial, &pData->dbPersonalAmineInven);

  result = _db_load_punishment(dwSerial, pData);
  if (result)
  {
    m_logSystemError.Write(
      "CMainThread::_db_load_punishment(...) : Ret(%u) _db_load_punishment(dwSerial(%u), pData) Fail!",
      result,
      dwSerial);
    return result;
  }

  if (!pbAddItem || !pdwAddDalant || !pdwAddGold)
  {
    return result;
  }
  if (!pData->dbAvator.m_byLevel)
  {
    return result;
  }
  if (pData->dbAvator.m_byLevel >= 0x19u)
  {
    result = _db_load_losebattlecount(dwSerial, pData);
    if (result)
    {
      m_logSystemError.Write(
        "CMainThread::db_Load_Avator(...) : Ret(%u) _db_load_losebattlecount(dwSerial(%u), pData) Fail!",
        result,
        dwSerial);
      return result;
    }
  }

  int piTime[8]{};
  unsigned __int8 pbyType[20]{};
  pbyType[0] = static_cast<unsigned __int8>(-1);
  unsigned int pDwItemCode_K[8]{};
  unsigned int pdwItemCode_U[8]{};
  unsigned int dwItemChargeIndex[9]{};
  unsigned __int64 pDwItemCode_D[3]{};

  bool itemAdded = false;
  while (CRFWorldDatabase::Select_ItemCharge(
           m_pWorldDB,
           dwSerial,
           pbyType,
           pDwItemCode_K,
           pDwItemCode_D,
           pdwItemCode_U,
           dwItemChargeIndex,
           piTime))
  {
    if (pbyType[0])
    {
      if (pbyType[0] == 1)
      {
        *pdwAddDalant += static_cast<unsigned int>(pDwItemCode_D[0]);
        CRFWorldDatabase::Delete_ItemCharge(m_pWorldDB, dwItemChargeIndex[0]);
      }
      else
      {
        if (pbyType[0] == 2)
        {
          *pdwAddGold += static_cast<unsigned int>(pDwItemCode_D[0]);
        }
        CRFWorldDatabase::Delete_ItemCharge(m_pWorldDB, dwItemChargeIndex[0]);
      }
    }
    else
    {
      itemAdded = false;
      for (unsigned int j = 0; j < 20u * pData->dbAvator.m_byBagNum; ++j)
      {
        if (!pData->dbInven.m_List[j].Key.IsFilled())
        {
          pData->dbInven.m_List[j].Key.LoadDBKey(pDwItemCode_K[0]);
          pData->dbInven.m_List[j].dwDur = pDwItemCode_D[0];
          pData->dbInven.m_List[j].dwUpt = pdwItemCode_U[0];
          const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(
            pData->dbInven.m_List[j].Key.byTableCode,
            pData->dbInven.m_List[j].Key.wItemIndex);
          if (timeRec && timeRec->m_nCheckType)
          {
            if (piTime[0] <= 0)
            {
              piTime[0] = timeRec->m_nUseTime;
            }
            __time32_t timeNow[9]{};
            _time32(timeNow);
            pData->dbInven.m_List[j].byCsMethod = timeRec->m_nCheckType;
            if (timeRec->m_nCheckType == 1)
            {
              pData->dbInven.m_List[j].dwT = piTime[0];
            }
            else if (timeRec->m_nCheckType == 2)
            {
              pData->dbInven.m_List[j].dwT = piTime[0] + timeNow[0];
            }
            pData->dbInven.m_List[j].dwLendRegdTime = timeNow[0];
          }
          CRFWorldDatabase::Delete_ItemCharge(m_pWorldDB, dwItemChargeIndex[0]);
          pbAddItem[j] = true;
          itemAdded = true;
          break;
        }
      }
      if (!itemAdded)
      {
        break;
      }
    }
  }

  if (pdTrunkOldDalant)
  {
    *pdTrunkOldDalant = pData->dbTrunk.dDalant;
  }
  if (pdTrunkOldGold)
  {
    *pdTrunkOldGold = pData->dbTrunk.dGold;
  }
  if (pbyTrunkOldSlot)
  {
    *pbyTrunkOldSlot = pData->dbTrunk.bySlotNum;
  }
  if (pbyExtTrunkOldSlot)
  {
    *pbyExtTrunkOldSlot = pData->dbTrunk.byExtSlotNum;
  }

  if (!pbTrunkAddItem || !pbyTrunkOldSlot || !pdTrunkOldDalant || !pdTrunkOldGold || !pbExtTrunkAddItem
      || !pbyExtTrunkOldSlot)
  {
    return result;
  }

  long double pdMoney[3]{};
  unsigned __int8 pbyRace[20]{};
  pbyRace[0] = static_cast<unsigned __int8>(-1);
  pbyType[0] = 1;

  if (!pData->dbTrunk.bySlotNum)
  {
    if (pbCreateTrunkFree
        && CRFWorldDatabase::Select_AccountItemCharge(
          m_pWorldDB,
          dwAccountSerial,
          pbyType,
          pdMoney,
          pDwItemCode_K,
          pDwItemCode_D,
          pdwItemCode_U,
          pbyRace,
          dwItemChargeIndex,
          piTime))
    {
      *pbCreateTrunkFree = true;
    }
    return result;
  }

  for (unsigned int j = 0;
       j < 10000
       && CRFWorldDatabase::Select_AccountItemCharge(
         m_pWorldDB,
         dwAccountSerial,
         pbyType,
         pdMoney,
         pDwItemCode_K,
         pDwItemCode_D,
         pdwItemCode_U,
         pbyRace,
         dwItemChargeIndex,
         piTime);
       ++j)
  {
    if ((pDwItemCode_D[0] != 20 && pDwItemCode_D[0] != 40 && pDwItemCode_D[0] != 60 && pDwItemCode_D[0] != 80
         && pDwItemCode_D[0] != 100)
        || pDwItemCode_D[0] == 20)
    {
      CRFWorldDatabase::Delete_TrunkItemCharge(m_pWorldDB, dwItemChargeIndex[0]);
      continue;
    }

    pDwItemCode_D[0] -= 20;
    if (pData->dbTrunk.bySlotNum != 100)
    {
      pData->dbTrunk.bySlotNum += static_cast<unsigned __int8>(pDwItemCode_D[0]);
      if (pData->dbTrunk.bySlotNum > 0x64u)
      {
        unsigned int excess = pData->dbTrunk.bySlotNum - 100;
        double addDalant = static_cast<double>(static_cast<int>(500000 * (excess / 0x14)));
        pData->dbTrunk.dDalant = pData->dbTrunk.dDalant + addDalant;
        pData->dbTrunk.bySlotNum = 100;
      }
      CRFWorldDatabase::Delete_TrunkItemCharge(m_pWorldDB, dwItemChargeIndex[0]);
      continue;
    }

    long long addValue = 500000LL * (pDwItemCode_D[0] / 0x14);
    double add = static_cast<double>(static_cast<int>(addValue));
    if (addValue < 0)
    {
      add = add + 1.844674407370955e19;
    }
    pData->dbTrunk.dDalant = pData->dbTrunk.dDalant + add;
    CRFWorldDatabase::Delete_TrunkItemCharge(m_pWorldDB, dwItemChargeIndex[0]);
  }

  pbyType[0] = 2;
  pbyRace[0] = static_cast<unsigned __int8>(pData->dbAvator.m_byRaceSexCode >> 1);
  for (unsigned int j = 0;
       j < 10000
       && CRFWorldDatabase::Select_AccountItemCharge(
         m_pWorldDB,
         dwAccountSerial,
         pbyType,
         pdMoney,
         pDwItemCode_K,
         pDwItemCode_D,
         pdwItemCode_U,
         pbyRace,
         dwItemChargeIndex,
         piTime);
       ++j)
  {
    pData->dbTrunk.dDalant = pData->dbTrunk.dDalant + pdMoney[0];
    CRFWorldDatabase::Delete_TrunkItemCharge(m_pWorldDB, dwItemChargeIndex[0]);
  }

  pbyType[0] = 3;
  pbyRace[0] = static_cast<unsigned __int8>(pData->dbAvator.m_byRaceSexCode >> 1);
  for (unsigned int j = 0;
       j < 10000
       && CRFWorldDatabase::Select_AccountItemCharge(
         m_pWorldDB,
         dwAccountSerial,
         pbyType,
         pdMoney,
         pDwItemCode_K,
         pDwItemCode_D,
         pdwItemCode_U,
         pbyRace,
         dwItemChargeIndex,
         piTime);
       ++j)
  {
    pData->dbTrunk.dGold = pData->dbTrunk.dGold + pdMoney[0];
    CRFWorldDatabase::Delete_TrunkItemCharge(m_pWorldDB, dwItemChargeIndex[0]);
  }

  pbyType[0] = 4;
  pbyRace[0] = static_cast<unsigned __int8>(-1);
  bool hasExtendCharge = false;
  if (!pData->dbTrunk.byExtSlotNum && pData->dbTrunk.bySlotNum)
  {
    hasExtendCharge = CRFWorldDatabase::Select_AccountItemCharge(
      m_pWorldDB,
      dwAccountSerial,
      pbyType,
      pdMoney,
      pDwItemCode_K,
      pDwItemCode_D,
      pdwItemCode_U,
      pbyRace,
      dwItemChargeIndex,
      piTime);
  }
  if (hasExtendCharge)
  {
    for (unsigned int j = 0;
         j < 10000
         && CRFWorldDatabase::Select_AccountItemCharge(
           m_pWorldDB,
           dwAccountSerial,
           pbyType,
           pdMoney,
           pDwItemCode_K,
           pDwItemCode_D,
           pdwItemCode_U,
           pbyRace,
           dwItemChargeIndex,
           piTime);
         ++j)
    {
      if (pDwItemCode_D[0] == 20 || pDwItemCode_D[0] == 40)
      {
        pData->dbTrunk.bySlotNum = 100;
        pData->dbTrunk.byExtSlotNum += static_cast<unsigned __int8>(pDwItemCode_D[0]);
        if (pData->dbTrunk.byExtSlotNum > 0x28u)
        {
          pData->dbTrunk.byExtSlotNum = 40;
        }
      }
      CRFWorldDatabase::Delete_TrunkItemCharge(m_pWorldDB, dwItemChargeIndex[0]);
    }
  }

  pbyType[0] = 0;
  pbyRace[0] = static_cast<unsigned __int8>(-1);
  for (unsigned __int8 k = 0; k < static_cast<unsigned __int8>(pData->dbTrunk.bySlotNum); ++k)
  {
    if (!pData->dbTrunk.m_List[k].Key.IsFilled())
    {
      if (!CRFWorldDatabase::Select_AccountItemCharge(
            m_pWorldDB,
            dwAccountSerial,
            pbyType,
            pdMoney,
            pDwItemCode_K,
            pDwItemCode_D,
            pdwItemCode_U,
            pbyRace,
            dwItemChargeIndex,
            piTime))
      {
        return result;
      }
      pData->dbTrunk.m_List[k].Key.LoadDBKey(pDwItemCode_K[0]);
      pData->dbTrunk.m_List[k].dwDur = pDwItemCode_D[0];
      pData->dbTrunk.m_List[k].dwUpt = pdwItemCode_U[0];
      pData->dbTrunk.m_List[k].byRace = pbyRace[0];
      const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(
        pData->dbTrunk.m_List[k].Key.byTableCode,
        pData->dbTrunk.m_List[k].Key.wItemIndex);
      if (timeRec && timeRec->m_nCheckType)
      {
        if (piTime[0] <= 0)
        {
          piTime[0] = timeRec->m_nUseTime;
        }
        __time32_t timeNow[7]{};
        _time32(timeNow);
        pData->dbTrunk.m_List[k].byCsMethod = timeRec->m_nCheckType;
        if (timeRec->m_nCheckType == 1)
        {
          pData->dbTrunk.m_List[k].dwT = piTime[0];
        }
        else if (timeRec->m_nCheckType == 2)
        {
          pData->dbTrunk.m_List[k].dwT = piTime[0] + timeNow[0];
        }
        pData->dbTrunk.m_List[k].dwLendRegdTime = timeNow[0];
      }
      pbTrunkAddItem[k] = true;
      CRFWorldDatabase::Delete_TrunkItemCharge(m_pWorldDB, dwItemChargeIndex[0]);
    }
  }

  return result;
}

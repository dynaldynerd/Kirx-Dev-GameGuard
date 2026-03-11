#include "pch.h"

#include "WorldServerUtil.h"

#include "CUserDB.h"
#include "CLogFile.h"
#include "GlobalObjects.h"

char CUserDB::Update_TakeLastMentalTicket(unsigned int dwMentalTicket)
{
  m_AvatorData.dbAvator.m_dwTakeLastMentalTicket = dwMentalTicket;
  m_bDataUpdate = true;
  return 1;
}

char CUserDB::Update_TrunkSlotNum(unsigned __int8 bySlotNum)
{
  this->m_AvatorData.dbTrunk.bySlotNum = bySlotNum;
  this->m_bDataUpdate = 1;
  return 1;
}

char CUserDB::Update_ExtTrunkSlotNum(unsigned __int8 byExtSlotNum)
{
  this->m_AvatorData.dbTrunk.byExtSlotNum = byExtSlotNum;
  this->m_bDataUpdate = 1;
  return 1;
}

char CUserDB::Update_TrunkPassword(char *pwszPassword)
{
  std::strcpy(this->m_AvatorData.dbTrunk.wszPasswd, pwszPassword);
  this->m_bDataUpdate = 1;
  return 1;
}

char CUserDB::Update_TrunkHint(unsigned __int8 byHintIndex, char *pwszHintAnswer)
{
  this->m_AvatorData.dbTrunk.byHintIndex = byHintIndex;
  std::strcpy(this->m_AvatorData.dbTrunk.wszHintAnswer, pwszHintAnswer);
  this->m_bDataUpdate = 1;
  return 1;
}

char CUserDB::Update_TrunkMoney(long double dGold, long double dDalant)
{
  this->m_AvatorData.dbTrunk.dGold = dGold;
  this->m_AvatorData.dbTrunk.dDalant = dDalant;
  this->m_bDataUpdate = 1;
  return 1;
}

char CUserDB::Update_ItemUpgrade(unsigned __int8 storage, unsigned __int8 slot, unsigned int upg, bool bUpdate)
{
  if (!IsStorageRange(storage, slot))
  {
    g_Main.m_logSystemError.Write(
      "%s : Update_Upgrade(CODE) : scode : %d, icode : %d  ",
      this->m_aszAvatorName,
      storage,
      slot);
    return 0;
  }

  if (storage)
  {
    switch (storage)
    {
      case 1:
      {
        _EQUIPKEY *key = &this->m_AvatorData.dbAvator.m_EquipKey[slot];
        if (!key->IsFilled())
        {
          g_Main.m_logSystemError.Write("%s:Update_Upgrade(EQUIP, Idx:%d)", this->m_aszAvatorName, slot);
          return 0;
        }
        this->m_AvatorData.dbAvator.m_dwFixEquipLv[slot] = upg;
        break;
      }
      case 4:
      {
        _ANIMUSKEY *key = &this->m_AvatorData.dbAnimus.m_List[slot].Key;
        if (!key->IsFilled())
        {
          g_Main.m_logSystemError.Write("%s:Update_Upgrade(ANIMUS, Idx:%d)", this->m_aszAvatorName, slot);
          return 0;
        }
        *reinterpret_cast<unsigned int *>(&key[9].byItemIndex) = upg;
        break;
      }
      case 5:
      {
        _INVENKEY *key = &this->m_AvatorData.dbTrunk.m_List[slot].Key;
        if (!key->IsFilled())
        {
          g_Main.m_logSystemError.Write("%s:Update_Upgrade(TRUNK, Idx:%d)", this->m_aszAvatorName, slot);
          return 0;
        }
        std::memcpy(&key[3], &upg, sizeof(upg));
        break;
      }
      case 7:
      {
        _INVENKEY *key = &this->m_AvatorData.dbTrunk.m_ExtList[slot].Key;
        if (!key->IsFilled())
        {
          g_Main.m_logSystemError.Write("%s:Update_Upgrade(EXT_TRUNK, Idx:%d)", this->m_aszAvatorName, slot);
          return 0;
        }
        std::memcpy(&key[3], &upg, sizeof(upg));
        break;
      }
      default:
        g_Main.m_logSystemError.Write(
          "%s:Update_Upgrade(NOT EQUAL(%d), Idx:%d)",
          this->m_aszAvatorName,
          storage,
          slot);
        return 0;
    }
  }
  else
  {
    _INVENKEY *key = &this->m_AvatorData.dbInven.m_List[slot].Key;
    if (!key->IsFilled())
    {
      g_Main.m_logSystemError.Write("%s:Update_Upgrade(INVEN, Idx:%d)", this->m_aszAvatorName, slot);
      return 0;
    }
    std::memcpy(&key[3], &upg, sizeof(upg));
  }

  if (bUpdate)
  {
    this->m_bDataUpdate = 1;
  }
  return 1;
}

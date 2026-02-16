#include "pch.h"

#include "CRaceBuffInfoByHolyQuest.h"

#include "CMainThread.h"
#include "CLogFile.h"
#include "WorldServerUtil.h"
#include "CNetworkEX.h"

#include <cstdio>
#include <cstring>

CRaceBuffInfoByHolyQuest::CRaceBuffInfoByHolyQuest(_skill_fld *data, unsigned __int8 lv)
  : m_pData(data),
    m_byLv(lv)
{
}

CRaceBuffInfoByHolyQuest::~CRaceBuffInfoByHolyQuest()
{
  // this is not a stub
}

CRaceBuffInfoByHolyQuest *CRaceBuffInfoByHolyQuest::Create(unsigned int uiNTh, char *szItemName)
{
  _skill_fld *field = nullptr;
  unsigned __int8 lv = 0;
  if (!LoadINISubProcLoadCode(uiNTh, szItemName, &field, &lv))
  {
    return nullptr;
  }

  return new CRaceBuffInfoByHolyQuest(field, lv);
}

bool CRaceBuffInfoByHolyQuest::LoadINISubProcLoadCode(
  unsigned int uiTh,
  char *szItemName,
  _skill_fld **ppFld,
  unsigned __int8 *byLv)
{
  char buffer[1024]{};
  char returnedString[1024]{};
  char szRecordCode[128]{};
  char *ppszDst[2] = {szRecordCode, &szRecordCode[64]};

  sprintf_s(buffer, sizeof(buffer), "%s%u", szItemName, uiTh);
  GetPrivateProfileStringA(
    "RaceBuff",
    buffer,
    "NULL",
    returnedString,
    static_cast<DWORD>(sizeof(returnedString)),
    ".\\Initialize\\NewHolySystem.ini");

  if (strcmp(returnedString, "NULL") == 0)
  {
    g_Main.m_logLoadingError.Write(
      "CRaceBuffInfoByHolyQuestList::LoadINISubProcLoadCode() : th(%u) %s NULL!",
      uiTh,
      buffer);
    return false;
  }

  szRecordCode[0] = 0;
  szRecordCode[64] = 0;
  const int parsed = ParsingCommandA(returnedString, 2, ppszDst, 64);
  if (parsed == 0)
  {
    g_Main.m_logLoadingError.Write(
      "CRaceBuffInfoByHolyQuestList::LoadINISubProcLoadCode() : th(%u) %s ::ParsingCommandA NULL!",
      uiTh,
      buffer);
    return false;
  }

  *ppFld = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[0].GetRecord(szRecordCode));
  if (*ppFld == nullptr)
  {
    g_Main.m_logLoadingError.Write(
      "CRaceBuffInfoByHolyQuestList::LoadINISubProcLoadCode() : th(%u) %s = %s Skill Not Exist!",
      uiTh,
      buffer,
      szRecordCode);
    return false;
  }

  *byLv = static_cast<unsigned __int8>(atoi(&szRecordCode[64]));
  if (*byLv == 0 || *byLv > 7)
  {
    g_Main.m_logLoadingError.Write(
      "CRaceBuffInfoByHolyQuestList::LoadINISubProcLoadCode() : th(%u) %s = %s %s SkillLv Invalid! max_skill_level(%d)",
      uiTh,
      buffer,
      szRecordCode,
      &szRecordCode[64],
      7);
  }

  return true;
}

bool CRaceBuffInfoByHolyQuest::Apply(CPlayer *pkDest)
{
  if (!ApplyEffect(pkDest, true))
  {
    return false;
  }
  NotifySetBuff(pkDest);
  return true;
}

bool CRaceBuffInfoByHolyQuest::Release(CPlayer *pkDest)
{
  if (!ApplyEffect(pkDest, false))
  {
    return false;
  }
  NotifyReleaseBuff(pkDest->m_ObjID.m_wIndex);
  return true;
}

bool CRaceBuffInfoByHolyQuest::CreateComplete(CPlayer *pkDest)
{
  if (!ApplyEffect(pkDest, true))
  {
    return false;
  }
  NotifyLogInSetBuff(pkDest->m_ObjID.m_wIndex);
  return true;
}

bool CRaceBuffInfoByHolyQuest::ApplyEffect(CPlayer *pkDest, bool bAdd)
{
  if (!m_pData || m_byLv >= 7u || !pkDest)
  {
    return false;
  }

  for (int j = 0; j < 5; ++j)
  {
    _cont_param_list *param = &m_pData->m_ContParamList[j];
    if (param->m_nContParamCode == -1)
    {
      break;
    }

    switch (param->m_nContParamCode)
    {
      case 0:
        pkDest->m_EP.SetEff_Rate(
          static_cast<unsigned int>(param->m_nContParamIndex),
          param->m_fContValue[m_byLv - 1],
          bAdd);
        break;
      case 1:
        pkDest->m_EP.SetEff_Plus(
          static_cast<unsigned int>(param->m_nContParamIndex),
          param->m_fContValue[m_byLv - 1],
          bAdd);
        break;
      case 2:
        pkDest->m_EP.SetEff_State(static_cast<unsigned int>(param->m_nContParamIndex), bAdd);
        break;
      default:
        break;
    }
  }

  return true;
}

void CRaceBuffInfoByHolyQuest::NotifySetBuff(CPlayer *pkDest)
{
  _notify_set_race_buff_by_holy_quest_zocl msg{};

  msg.byLv = m_byLv;
  msg.wEffectIndex = static_cast<unsigned __int16>(m_pData->m_dwIndex);
  msg.dwDestSerial = pkDest->m_dwObjSerial;

  unsigned __int8 type[2]{};
  type[0] = 17;
  type[1] = 36;
  pkDest->CircleReport(type, reinterpret_cast<char *>(&msg), 7, true);
}

void CRaceBuffInfoByHolyQuest::NotifyReleaseBuff(unsigned __int16 wUserInx)
{
  unsigned int msg = 0;
  unsigned __int8 type[2]{};
  type[0] = 17;
  type[1] = 37;
  g_Network.m_pProcess[0]->LoadSendMsg(wUserInx, type, reinterpret_cast<char *>(&msg), 4u);
}

void CRaceBuffInfoByHolyQuest::NotifyLogInSetBuff(unsigned __int16 wUserInx)
{
  _effect_start_inform_zocl msg{};

  msg.byLv = m_byLv;
  msg.wEffectCode = static_cast<unsigned __int16>(m_pData->m_dwIndex);
  msg.wDurSec = 0;

  unsigned __int8 type[2]{};
  type[0] = 17;
  type[1] = 25;
  g_Network.m_pProcess[0]->LoadSendMsg(wUserInx, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

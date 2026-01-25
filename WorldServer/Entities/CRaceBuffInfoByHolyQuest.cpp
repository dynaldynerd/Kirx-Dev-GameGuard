#include "pch.h"

#include "CRaceBuffInfoByHolyQuest.h"

#include "CMainThread.h"
#include "CLogFile.h"
#include "WorldServerUtil.h"

#include <cstdio>
#include <cstring>

CRaceBuffInfoByHolyQuest::CRaceBuffInfoByHolyQuest(_skill_fld *data, unsigned __int8 lv)
  : m_pData(data),
    m_byLv(lv)
{
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

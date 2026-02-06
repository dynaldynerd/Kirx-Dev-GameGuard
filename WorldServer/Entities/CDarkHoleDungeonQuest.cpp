#include "pch.h"

#include "CDarkHoleDungeonQuest.h"

#include <mmsystem.h>

namespace
{
  static const char kDarkHoleQuestMatchSection[] = "MATCH";
  static const char kDarkHoleQuestDefaultValue[] = "default";
  static const char kDarkHoleQuestIndexIni[] = ".\\Quest\\Index.ini";
  static const char kDarkHoleQuestNoMatch[] = "X";
  static const char kDarkHoleQuestLoadErrorTitle[] = "Quest Dungeon Load Error";
  static const char kDarkHoleQuestItemNoMatchFmt[] = "%s item no match quest";
  static const char kDarkHoleQuestPathFmt[] = ".\\Quest\\%s.txt";
  static const char kDarkHoleQuestErrorFmt[] = "%s";
}

CDarkHoleDungeonQuest g_DarkHoleQuest;

bool CDarkHoleDungeonQuest::LoadDarkHoleQuest()
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 100; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }

  for (int n = 0;; ++n)
  {
    const int recordNum = static_cast<int>(g_Main.m_tblItemData[23].GetRecordNum());
    if (n >= recordNum)
      break;
    _base_fld *record = g_Main.m_tblItemData[23].GetRecord(n);
    char returnedString[160]{};
    GetPrivateProfileStringA(
      kDarkHoleQuestMatchSection,
      record->m_strCode,
      kDarkHoleQuestDefaultValue,
      returnedString,
      0x80u,
      kDarkHoleQuestIndexIni);
    if (!strcmp_0(returnedString, kDarkHoleQuestNoMatch))
      MyMessageBox(kDarkHoleQuestLoadErrorTitle, kDarkHoleQuestItemNoMatchFmt, record->m_strCode);
    char buffer[144]{};
    sprintf(buffer, kDarkHoleQuestPathFmt, returnedString);
    if (!SetupQuest(buffer))
    {
      const char *errorMsg = GetErrorMsg();
      MyMessageBox(kDarkHoleQuestLoadErrorTitle, kDarkHoleQuestErrorFmt, errorMsg);
      return false;
    }
  }
  m_bLoad = true;
  m_dwCheckLastTime = timeGetTime();
  return true;
}


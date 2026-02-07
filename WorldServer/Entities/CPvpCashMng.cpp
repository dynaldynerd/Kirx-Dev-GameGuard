#include "pch.h"

#include "CPvpCashMng.h"

#include "CRecordData.h"
#include "WorldServerUtil.h"

#include <cstdio>
#include <cstdlib>

CPvpCashMng *CPvpCashMng::Instance()
{
  static CPvpCashMng s_instance;
  return &s_instance;
}

bool CPvpCashMng::LoadData()
{
  char parseBuffers[2][64];
  char *tokens[3] = {parseBuffers[0], parseBuffers[1], nullptr};
  memset_0(parseBuffers, 0, sizeof(parseBuffers));

  m_TalikList.byTalikNum =
    GetPrivateProfileIntA("TalikRecorver", "TalikNum", -1, ".\\Initialize\\PvpCashPoint.ini");
  if (m_TalikList.byTalikNum)
  {
    for (int index = 0; index < m_TalikList.byTalikNum; ++index)
    {
      if (!Parsing("TalikRecorver", "talik", tokens, index, 0))
      {
        MyMessageBox("CPvpCashPoint::LoadData()", "talik%d Error", index);
        return false;
      }
    }

    const int classCount = GetPrivateProfileIntA("ClassValue", "ClassNum", -1, ".\\Initialize\\PvpCashPoint.ini");
    if (classCount > 0)
    {
      memset_0(parseBuffers, 0, sizeof(parseBuffers));
      for (int index = 0; index < classCount; ++index)
      {
        if (!Parsing("ClassValue", "Class", tokens, index, 1u))
        {
          MyMessageBox("CPvpCashPoint::LoadData()", "Class%d Error", index);
          return false;
        }
      }

      const int levelCount =
        GetPrivateProfileIntA("LimitPvpPoint", "LevelCnt", -1, ".\\Initialize\\PvpCashPoint.ini");
      if (levelCount > 0)
      {
        const int startLv =
          GetPrivateProfileIntA("LimitPvpPoint", "StartLv", -1, ".\\Initialize\\PvpCashPoint.ini");
        if (startLv > 0)
        {
          char buffer[96]{};
          for (int index = 0; index < levelCount; ++index)
          {
            const int level = startLv + index;
            sprintf(buffer, "lv%d", level);
            m_LimitPoint[index].nLv = level;
            m_LimitPoint[index].nMaxPoint =
              GetPrivateProfileIntA("LimitPvpPoint", buffer, -1, ".\\Initialize\\PvpCashPoint.ini");
            if (m_LimitPoint[index].nMaxPoint <= 0)
            {
              MyMessageBox("CPvpCashPoint::LoadData()", "LimitPvpPoint StartLv Info Error");
              return false;
            }
            m_LimitPoint[index].nMinPoint = -m_LimitPoint[index].nMaxPoint;
          }

          for (int index = 0; index < levelCount; ++index)
          {
            const int level = startLv + index;
            sprintf(buffer, "lv%d", level);
            m_LimitPoint[index].nPremiumMaxPoint = GetPrivateProfileIntA(
              "PremiumLimitPvpPoint",
              buffer,
              -1,
              ".\\Initialize\\PvpCashPoint.ini");
            if (m_LimitPoint[index].nMaxPoint <= 0)
            {
              MyMessageBox("CPvpCashPoint::LoadData()", "PremiunLimitPvpPoint StartLv Info Error");
              return false;
            }
          }
          return true;
        }

        MyMessageBox("CPvpCashPoint::LoadData()", "LimitPvpPoint StartLv Info Error");
        return false;
      }

      MyMessageBox("CPvpCashPoint::LoadData()", "LimitPvpPoint LevelCnt Info Error");
      return false;
    }

    MyMessageBox("CPvpCashPoint::LoadData()", "No Class Value Info");
    return false;
  }

  MyMessageBox(
    "CPvpCashPoint::LoadData()",
    "m_TalikList.byTalikNum Error(%d)",
    m_TalikList.byTalikNum);
  return false;
}

bool CPvpCashMng::Parsing(
  const char *szTitle,
  const char *szItem,
  char **szBuff,
  int nInx,
  unsigned __int8 byParseType)
{
  char buffer[1056];
  char returned[2052];

  memset_0(buffer, 0, sizeof(buffer));
  memset_0(returned, 0, sizeof(returned));
  sprintf(buffer, "%s%d", szItem, nInx);
  GetPrivateProfileStringA(szTitle, buffer, "X", returned, 0x800u, ".\\Initialize\\PvpCashPoint.ini");
  if (!strcmp_0(returned, "X"))
  {
    return false;
  }
  const int parsed = ParsingCommandA(returned, 2, szBuff, 64);
  if (parsed < 2)
  {
    return false;
  }

  if (byParseType)
  {
    if (byParseType == 1)
    {
      _class_fld *record = static_cast<_class_fld *>(g_Main.m_tblClass.GetRecord(*szBuff));
      if (!record)
      {
        return false;
      }
      m_ClassValList[nInx].m_pFld = record;
      m_ClassValList[nInx].byClassVal = static_cast<unsigned __int8>(atoi(szBuff[1]));
    }
  }
  else
  {
    if (!SetItem(*szBuff, nInx))
    {
      return false;
    }
    m_TalikList.TalikInfo[nInx].nRecvrPoint = atoi(szBuff[1]);
  }

  return true;
}

bool CPvpCashMng::SetItem(char *szItemCode, int nInx)
{
  _base_fld *record = nullptr;
  unsigned __int8 tableCode = 0;
  for (tableCode = 0; ; ++tableCode)
  {
    if (tableCode >= 0x25u)
    {
      return false;
    }
    record = g_Main.m_tblItemData[tableCode].GetRecord(szItemCode);
    if (record)
    {
      break;
    }
  }
  if (GetItemKindCode(tableCode))
  {
    return false;
  }
  m_TalikList.TalikInfo[nInx].byTableCode = tableCode;
  m_TalikList.TalikInfo[nInx].m_pFld = record;
  return true;
}

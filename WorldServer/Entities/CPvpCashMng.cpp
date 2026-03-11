#include "pch.h"

#include "base_fld.h"

#include "GlobalObjects.h"

#include "CPvpCashMng.h"

#include "CRecordData.h"
#include "WorldServerUtil.h"

#include <cstdio>
#include <cstdlib>

CPvpCashMng *CPvpCashMng::_pkInstance = nullptr;

CPvpCashMng::_talik_recvr_list::_talik_recvr_list()
{
  std::memset(this, 0, sizeof(_talik_recvr_list));
}

void CPvpCashMng::_class_value::init(_class_value *self)
{
  std::memset(self, 0, sizeof(_class_value));
}

CPvpCashMng::_class_value::_class_value()
{
  init(this);
}

void CPvpCashMng::_max_point::init(_max_point *self)
{
  std::memset(self, 0, sizeof(_max_point));
}

CPvpCashMng::_max_point::_max_point()
{
  init(this);
}

CPvpCashMng::CPvpCashMng() = default;

CPvpCashMng::~CPvpCashMng()
{
// this is not a stub
}

CPvpCashMng *CPvpCashMng::Instance()
{
  if (!_pkInstance)
  {
    _pkInstance = new CPvpCashMng();
  }
  return _pkInstance;
}

void CPvpCashMng::Release()
{
  if (_pkInstance)
  {
    delete _pkInstance;
    _pkInstance = nullptr;
  }
}

bool CPvpCashMng::LoadData()
{
  char parseBuffers[2][64];
  char *tokens[3] = {parseBuffers[0], parseBuffers[1], nullptr};
  std::memset(parseBuffers, 0, sizeof(parseBuffers));

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
      std::memset(parseBuffers, 0, sizeof(parseBuffers));
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

char CPvpCashMng::IsTalikItem(const char *strCode)
{
  for (int j = 0; j < 14; ++j)
  {
    if (!std::strcmp(m_TalikList.TalikInfo[j].m_pFld->m_strCode, strCode))
    {
      return 1;
    }
  }
  return 0;
}

int CPvpCashMng::GetTalikRecvrPoint(unsigned __int8 byTblCode, unsigned int dwIndex)
{
  for (int j = 0; j < m_TalikList.byTalikNum; ++j)
  {
    if (m_TalikList.TalikInfo[j].byTableCode == byTblCode
        && m_TalikList.TalikInfo[j].m_pFld->m_dwIndex == dwIndex)
    {
      return m_TalikList.TalikInfo[j].nRecvrPoint;
    }
  }
  return 0;
}

int CPvpCashMng::GetTalikRecvrPoint(int i)
{
  return m_TalikList.TalikInfo[i].nRecvrPoint;
}

unsigned __int8 CPvpCashMng::GetTalikNum()
{
  return m_TalikList.byTalikNum;
}

int CPvpCashMng::GetMyClassVal(char *pClass)
{
  for (int index = 0; index < 50; ++index)
  {
    if (m_ClassValList[index].m_pFld && !std::strcmp(m_ClassValList[index].m_pFld->m_strCode, pClass))
    {
      return m_ClassValList[index].byClassVal;
    }
  }

  return 0;
}

int CPvpCashMng::GetMaxTempPoint(unsigned __int8 nLv, bool bPremium)
{
  int index = 0;
  for (; index < 30; ++index)
  {
    if (m_LimitPoint[index].nLv && m_LimitPoint[index].nLv == nLv)
    {
      break;
    }
  }

  if (index >= 30)
  {
    return 0;
  }

  if (bPremium)
  {
    return m_LimitPoint[index].nPremiumMaxPoint;
  }
  return m_LimitPoint[index].nMaxPoint;
}

int CPvpCashMng::GetMinTempPoint(unsigned __int8 nLv)
{
  for (int index = 0; index < 30; ++index)
  {
    if (m_LimitPoint[index].nLv && m_LimitPoint[index].nLv == nLv)
    {
      return m_LimitPoint[index].nMinPoint;
    }
  }

  return 0;
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

  std::memset(buffer, 0, sizeof(buffer));
  std::memset(returned, 0, sizeof(returned));
  sprintf(buffer, "%s%d", szItem, nInx);
  GetPrivateProfileStringA(szTitle, buffer, "X", returned, 2048, ".\\Initialize\\PvpCashPoint.ini");
  if (!std::strcmp(returned, "X"))
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
    if (tableCode >= 37)
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

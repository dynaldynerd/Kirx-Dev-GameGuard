#include "pch.h"

#include "CSUItemSystem.h"

CSUItemSystem *CSUItemSystem::Instance()
{
  static CSUItemSystem s_instance;
  return &s_instance;
}

CSUItemSystem::CSUItemSystem()
  : m_SetItemType()
{
  Class_Init();
}

CSUItemSystem::~CSUItemSystem()
{
  Class_Init();
}

bool CSUItemSystem::SUItemSystem_Init()
{
  char pszErrMsg[144] = {};

  Class_Init();
  if (m_SUOrigin[0].ReadRecord(".\\script\\SetItemEff.dat", 0x404u, pszErrMsg))
  {
    m_bLoadData[0] = true;
    if (m_SetItemType.SetItemType_Init(m_SUOrigin))
    {
      m_bChangeData[0] = true;
      ++m_bySUItemCount;
      return true;
    }

    MyMessageBox("Failed Interpret Script data File : CSUItemSystem_Init()", "Error Interpeting Of SetItemType Data");
    return false;
  }

  MyMessageBox("Failed Load Script Data File : CSUItemSystem_Init()", pszErrMsg);
  return false;
}

bool CSUItemSystem::SUItemSystem_CheckData()
{
  if (m_SUOrigin[0].GetRecordNum() <= 0)
  {
    return false;
  }

  const int effectTypeCount = m_SetItemType.GetEffectTypeCount();
  if (effectTypeCount <= 0)
  {
    return false;
  }

  for (int set_pos = 0; set_pos < effectTypeCount; ++set_pos)
  {
    si_interpret *interpret = m_SetItemType.Getsi_interpret(set_pos);
    if (interpret == nullptr)
    {
      return false;
    }
    const unsigned __int8 count = interpret->GetEffectTypeCount();
    if (count > 8u)
    {
      return false;
    }
    for (int idx = 0; idx < count; ++idx)
    {
      if (interpret->GetCountOfItem(static_cast<unsigned int>(idx)) > 0xCu)
      {
        return false;
      }
      if (interpret->GetCountOfEffect(static_cast<unsigned int>(idx)) > 8u)
      {
        return false;
      }
    }
  }

  return true;
}

void CSUItemSystem::Class_Init()
{
  m_bySUItemCount = 0;
  m_bLoadData[0] = false;
  m_bChangeData[0] = false;
}

CSetItemType *CSUItemSystem::GetCSetItemType()
{
  if (!m_bySUItemCount)
  {
    return nullptr;
  }
  if (!m_bLoadData[0])
  {
    return nullptr;
  }
  if (m_bChangeData[0])
  {
    return &m_SetItemType;
  }
  return nullptr;
}

int CSUItemSystem::GetSetItemTableInfo(int dwSetItemEff, char *pStrCode, int nBufSize)
{
  _base_fld *record = m_SUOrigin[0].GetRecord(dwSetItemEff);
  if (!record)
  {
    return -1;
  }

  if (!strcmp_0(&record[1].m_strCode[60], "-1"))
  {
    if (!strcmp_0(&record[2].m_strCode[56], "-1"))
    {
      if (!strcmp_0(&record[3].m_strCode[52], "-1"))
      {
        if (!strcmp_0(&record[4].m_strCode[48], "-1"))
        {
          if (!strcmp_0(&record[5].m_strCode[44], "-1"))
          {
            if (!strcmp_0(&record[6].m_strCode[40], "-1"))
            {
              if (!strcmp_0(&record[7].m_strCode[36], "-1"))
              {
                if (!strcmp_0(&record[8].m_strCode[32], "-1") && !strcmp_0(&record[8].m_strCode[32], "-1"))
                {
                  if (!strcmp_0(&record[10].m_strCode[24], "-1"))
                  {
                    if (!strcmp_0(&record[11].m_strCode[20], "-1"))
                    {
                      if (!strcmp_0(&record[12].m_strCode[16], "-1"))
                      {
                        return -1;
                      }
                      strcpy_s(pStrCode, nBufSize, &record[12].m_strCode[16]);
                      return 7;
                    }
                    strcpy_s(pStrCode, nBufSize, &record[11].m_strCode[20]);
                    return 8;
                  }
                  strcpy_s(pStrCode, nBufSize, &record[10].m_strCode[24]);
                  return 8;
                }
                strcpy_s(pStrCode, nBufSize, &record[8].m_strCode[32]);
                return 9;
              }
              strcpy_s(pStrCode, nBufSize, &record[7].m_strCode[36]);
              return 5;
            }
            strcpy_s(pStrCode, nBufSize, &record[6].m_strCode[40]);
            return 6;
          }
          strcpy_s(pStrCode, nBufSize, &record[5].m_strCode[44]);
          return 2;
        }
        strcpy_s(pStrCode, nBufSize, &record[4].m_strCode[48]);
        return 3;
      }
      strcpy_s(pStrCode, nBufSize, &record[3].m_strCode[52]);
      return 1;
    }
    strcpy_s(pStrCode, nBufSize, &record[2].m_strCode[56]);
    return 0;
  }

  strcpy_s(pStrCode, nBufSize, &record[1].m_strCode[60]);
  return 4;
}

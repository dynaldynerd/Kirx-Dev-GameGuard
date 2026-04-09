#include "pch.h"

#include "WorldServerUtil.h"

#include "CSUItemSystem.h"
#include "SetItemEff_fld.h"

CSUItemSystem *CSUItemSystem::SCSUITEMSYSTEM = nullptr;

CSUItemSystem *CSUItemSystem::Instance()
{
  if (!SCSUITEMSYSTEM)
  {
    SCSUITEMSYSTEM = new CSUItemSystem();
  }
  return SCSUITEMSYSTEM;
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
  if (m_SUOrigin[0].ReadRecord(".\\script\\SetItemEff.dat", static_cast<int>(sizeof(_SetItemEff_fld)), pszErrMsg))
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
      if (interpret->GetCountOfItem(static_cast<unsigned int>(idx)) > 12)
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

bool CSUItemSystem::SUItemSystem_UnInit()
{
  Class_Init();
  return true;
}

CSUItemSystem *CSUItemSystem::GetCRecordData_SetItem()
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
    return this;
  }
  return nullptr;
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
  _SetItemEff_fld *record = reinterpret_cast<_SetItemEff_fld *>(m_SUOrigin[0].GetRecord(dwSetItemEff));
  if (!record)
  {
    return -1;
  }

  const char *code0 = record->m_strset_head;
  const char *code1 = record->m_strset_upper;
  const char *code2 = record->m_strset_lower;
  const char *code3 = record->m_strset_shoes;
  const char *code4 = record->m_strset_gauntlet;
  const char *code5 = record->m_strset_weapon;
  const char *code6 = record->m_strset_shield;
  const char *code7 = record->m_strset_amulet1;
  const char *code8 = record->m_strset_amulet2;
  const char *code9 = record->m_strset_ring1;
  const char *code10 = record->m_strset_ring2;
  const char *code11 = record->m_strset_cloak;

  if (!std::strcmp(code0, "-1"))
  {
    if (!std::strcmp(code1, "-1"))
    {
      if (!std::strcmp(code2, "-1"))
      {
        if (!std::strcmp(code3, "-1"))
        {
          if (!std::strcmp(code4, "-1"))
          {
            if (!std::strcmp(code5, "-1"))
            {
              if (!std::strcmp(code6, "-1"))
              {
                if (!std::strcmp(code7, "-1") && !std::strcmp(code8, "-1"))
                {
                  if (!std::strcmp(code9, "-1"))
                  {
                    if (!std::strcmp(code10, "-1"))
                    {
                      if (!std::strcmp(code11, "-1"))
                      {
                        return -1;
                      }
                      strcpy_s(pStrCode, nBufSize, code11);
                      return 7;
                    }
                    strcpy_s(pStrCode, nBufSize, code10);
                    return 8;
                  }
                  strcpy_s(pStrCode, nBufSize, code9);
                  return 8;
                }
                strcpy_s(pStrCode, nBufSize, code7);
                return 9;
              }
              strcpy_s(pStrCode, nBufSize, code6);
              return 5;
            }
            strcpy_s(pStrCode, nBufSize, code5);
            return 6;
          }
          strcpy_s(pStrCode, nBufSize, code4);
          return 2;
        }
        strcpy_s(pStrCode, nBufSize, code3);
        return 3;
      }
      strcpy_s(pStrCode, nBufSize, code2);
      return 1;
    }
    strcpy_s(pStrCode, nBufSize, code1);
    return 0;
  }

  strcpy_s(pStrCode, nBufSize, code0);
  return 4;
}

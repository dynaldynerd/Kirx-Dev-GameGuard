#include "pch.h"

#include "CNationCodeStrTable.h"

#include "WorldServerUtil.h"

bool CNationCodeStrTable::Init()
{
  const int ret = RegistCode();
  if (ret)
  {
    MyMessageBox("CNationCodeStrTable::Init()", "Regist NationCode(%d) Fail!", ret);
  }
  return true;
}

int CNationCodeStrTable::RegistCode()
{
  CNationCodeStr *entry = nullptr;

  entry = new CNationCodeStr(410, "KR");
  if (m_kTable.regist(entry))
  {
    return -1;
  }

  entry = new CNationCodeStr(826, "GB");
  if (m_kTable.regist(entry))
  {
    return -1;
  }

  entry = new CNationCodeStr(360, "ID");
  if (m_kTable.regist(entry))
  {
    return -1;
  }

  entry = new CNationCodeStr(392, "JP");
  if (m_kTable.regist(entry))
  {
    return -1;
  }

  entry = new CNationCodeStr(608, "PH");
  if (m_kTable.regist(entry))
  {
    return -1;
  }

  entry = new CNationCodeStr(643, "RU");
  if (m_kTable.regist(entry))
  {
    return -1;
  }

  entry = new CNationCodeStr(76, "BR");
  if (m_kTable.regist(entry))
  {
    return -1;
  }

  entry = new CNationCodeStr(158, "TW");
  if (m_kTable.regist(entry))
  {
    return -1;
  }

  entry = new CNationCodeStr(156, "CN");
  if (m_kTable.regist(entry))
  {
    return -1;
  }

  entry = new CNationCodeStr(840, "US");
  if (m_kTable.regist(entry))
  {
    return -1;
  }

  entry = new CNationCodeStr(724, "ES");
  if (m_kTable.regist(entry))
  {
    return -1;
  }

  entry = new CNationCodeStr(764, "TH");
  if (m_kTable.regist(entry))
  {
    return -1;
  }

  return 0;
}

int CNationCodeStrTable::GetCode(const char *code)
{
  int outCode = -1;
  CNationCodeStr lookup(-1, code);
  if (m_kTable.findkey(&lookup, &outCode))
  {
    return outCode;
  }
  return -1;
}

const char *CNationCodeStrTable::GetStr(int code)
{
  CNationCodeStr *entry = nullptr;
  if (m_kTable.get(code, &entry))
  {
    return entry->GetStr();
  }
  return nullptr;
}

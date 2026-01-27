#include "pch.h"

#include "AutominePersonalMgr.h"

#include "CMainThread.h"
#include "KorLocalTime.h"

#include <cstdio>
#include <cstring>

namespace
{
  const char *s_szAMPOreKind[5] = {
    "ioblu03",
    "iored03",
    "ioyel03",
    "iogre03",
    "iobla03",
  };
}

AutominePersonalMgr *AutominePersonalMgr::instance()
{
  static AutominePersonalMgr s_instance;
  return &s_instance;
}

bool AutominePersonalMgr::initialize()
{
  char dest[128]{};
  const unsigned int now = GetKorLocalTime();
  std::memset(dest, 0, sizeof(dest));

  sprintf_s(dest, sizeof(dest), "..\\ZoneServerLog\\SystemLog\\Concession\\ampersonalmgr_%u.log", now);
  m_logService.SetWriteLogFile(dest, 1, 0, 1, 1);

  const unsigned int now2 = GetKorLocalTime();
  sprintf_s(dest, sizeof(dest), "..\\ZoneServerLog\\systemlog\\log_ampersonalmgr_%u.log", now2);
  m_logError.SetWriteLogFile(dest, 1, 0, 1, 1);

  for (int j = 0; j < 5; ++j)
  {
    unsigned __int16 *record =
      reinterpret_cast<unsigned __int16 *>(g_Main.m_tblItemData[17].GetRecord(s_szAMPOreKind[j]));
    if (record == nullptr)
    {
      return false;
    }
    m_wOreIndex[j] = *record;
  }

  if (init_objects())
  {
    return true;
  }

  m_logError.Write("AutominePersonalMgr::initialize() >> init_objects() failed.");
  return false;
}

bool AutominePersonalMgr::init_objects()
{
  m_pMachine = new AutominePersonal[MAX_PLAYER];
  if (m_pMachine == nullptr)
  {
    m_logError.Write(
      "AutominePersonalMgr::init_Objects() >> Failed allocate memory of AutoMinePersonal[MAX_PLAYER].");
    return false;
  }

  for (unsigned int j = 0; j < MAX_PLAYER; ++j)
  {
    if (!m_pMachine[j].initialize(static_cast<unsigned __int16>(j)))
    {
      m_logError.Write(
        "AutominePersonalMgr::init_Objects() >> m_Machine[%d].initialize(%d) failed",
        j,
        j);
      return false;
    }
  }

  return true;
}

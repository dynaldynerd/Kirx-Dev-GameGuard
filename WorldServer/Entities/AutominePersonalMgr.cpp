#include "pch.h"

#include "AutominePersonalMgr.h"

#include "CMainThread.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CRFWorldDatabase.h"
#include "KorLocalTime.h"
#include "GlobalObjects.h"
#include "DqsDbStructs.h"
#include "personal_amine_errmsg_zocl.h"
#include "personal_amine_make_storage_zocl.h"

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

AutominePersonal *AutominePersonalMgr::get_machine(unsigned int nIdx)
{
  if (nIdx >= MAX_PLAYER)
  {
    return nullptr;
  }
  return &m_pMachine[nIdx];
}

char AutominePersonalMgr::db_load_inven(unsigned int dwSerial, _PERSONALAMINE_INVEN_DB_BASE *pCon)
{
  _personal_amine_inven inven{};
  if (g_Main.m_pWorldDB->select_amine_personal(dwSerial, &inven))
  {
    return 0;
  }

  for (int j = 0; j < 40; ++j)
  {
    if (inven.list[j].lK == -1)
    {
      pCon->m_List[j].Key.SetRelease();
      pCon->m_List[j].dwDur = 0;
    }
    else
    {
      memcpy_0(&pCon->m_List[j], &inven.list[j], 4u);
      pCon->m_List[j].dwDur = inven.list[j].byNum;
    }
  }
  return 1;
}

unsigned __int8 AutominePersonalMgr::request_query(char *pdata)
{
  if (!pdata)
  {
    return 24;
  }

  if (*pdata)
  {
    if (*pdata == 1)
    {
      char buffer[10244]{};
      std::memset(buffer, 0, 10240);
      sprintf(buffer, "update [dbo].[tbl_aminepersonal_inven] set ");

      int len = static_cast<int>(strlen_0(buffer));
      for (int j = 0; j < static_cast<unsigned __int8>(pdata[12]); ++j)
      {
        int num = static_cast<unsigned __int8>(pdata[12 * j + 24]);
        int slot = static_cast<unsigned __int8>(pdata[12 * j + 16]);
        char *dest = &buffer[len];
        sprintf(dest, ",K%d=%d,N%d=%d,", slot, *reinterpret_cast<unsigned int *>(&pdata[12 * j + 20]), slot, num);
      }

      len = static_cast<int>(strlen_0(buffer));
      sprintf(&buffer[len - 1], " where avatorserial = %d", *reinterpret_cast<unsigned int *>(pdata + 8));
      if (g_Main.m_pWorldDB->update_amine_personal(buffer))
      {
        return 0;
      }
      return 24;
    }
    return 24;
  }

  if (g_Main.m_pWorldDB->select_amine_personal(*reinterpret_cast<unsigned int *>(pdata + 8)) != 2
      || g_Main.m_pWorldDB->insert_amine_personal(*reinterpret_cast<unsigned int *>(pdata + 8)))
  {
    return 0;
  }
  return 24;
}

void AutominePersonalMgr::result_query(unsigned __int8 byRet, char *pdata)
{
  if (!pdata)
  {
    return;
  }

  if (!*pdata)
  {
    pop_dqs_makestorage(byRet, pdata);
  }
}

void AutominePersonalMgr::pop_dqs_makestorage(unsigned __int8 byRet, char *pdata)
{
  CPlayer *player = &g_Player[*reinterpret_cast<int *>(pdata + 4)];
  if (player->m_bOper)
  {
    unsigned int charSerial = player->m_Param.GetCharSerial();
    if (charSerial == *reinterpret_cast<unsigned int *>(pdata + 8))
    {
      if (byRet == 24)
      {
        player->AlterGold(250.0);
        send_ecode(player->m_ObjID.m_wIndex, 0x11u);
      }
      else
      {
        player->m_Param.SetHaveBoxOfAMP(true);
        _personal_amine_make_storage_zocl msg{};
        msg.dwAvatorSerial = player->m_Param.GetCharSerial();
        msg.dwTotGold = player->m_Param.GetGold();
        unsigned __int8 pbyType[2] = {14, 43};
        const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
        g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), nLen);
      }
    }
  }
}

void AutominePersonalMgr::send_ecode(unsigned int n, unsigned __int8 byCode)
{
  _personal_amine_errmsg_zocl msg{};
  msg.byErrCode = byCode;
  unsigned __int8 pbyType[2] = {14, 62};
  const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, pbyType, reinterpret_cast<char *>(&msg), nLen);
}

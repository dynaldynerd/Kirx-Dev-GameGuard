#include "pch.h"

#include "CMainThread.h"

#include "CBattleTournamentInfo.h"
#include "CCryptor.h"
#include "CGoldenBoxItemMgr.h"
#include "GlobalObjects.h"
#include "qry_case_select_golden_box_item.h"
#include "worlddb_golden_box_item.h"
#include "WorldServerUtil.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

bool CMainThread::db_LoadGreetingMsg()
{
  unsigned __int8 ret = this->m_pWorldDB->Select_CheckGreetRecord(255);
  if (ret == 2
      && !this->m_pWorldDB->Insert_GreetingRecord(255, "GM", "Welcome to RF World "))
  {
    return false;
  }

  for (int nUseType = 0; nUseType < 3; ++nUseType)
  {
    ret = this->m_pWorldDB->Select_CheckGreetRecord(nUseType);
    if (ret == 2
        && !this->m_pWorldDB->Insert_GreetingRecord(nUseType, "RaceBoss", "There is no Message "))
    {
      return false;
    }
  }

  return this->m_pWorldDB->LoadGreetingMsg(
           this->m_wszMainGreetingMsg,
           this->m_wszRaceGreetingMsg[0],
           this->m_wszRaceGreetingMsg[1],
           this->m_wszRaceGreetingMsg[2],
           this->m_wszGMName,
           this->m_wszBossName[0],
           this->m_wszBossName[1],
           this->m_wszBossName[2])
         != 0;
}

void CMainThread::CreateSelectCharacterLogTable(unsigned __int8 byMonth)
{
  __time64_t now = 0;
  _time64(&now);
  tm *timeInfo = _localtime64(&now);
  if (!timeInfo)
  {
    return;
  }

  int year = timeInfo->tm_year + 1900;
  int month = timeInfo->tm_mon + 1;
  char buffer[48]{};
  char tableName[44]{};

  for (unsigned __int8 j = 0; j < byMonth; ++j)
  {
    sprintf_s(buffer, sizeof(buffer), "%04d%02d", year, month);
    sprintf_s(tableName, sizeof(tableName), "tbl_characterselect_log_%s", buffer);
    if (!this->m_pWorldDB->TableExist(tableName))
    {
      if (!this->m_pWorldDB->CreateCharacterSelectLogTable(tableName))
      {
        MyMessageBox("CMainThread::_GameDataBaseInit()", "Create %s Table Failed!", tableName);
        return;
      }
      this->m_logLoadingError.Write("Character Select Log Table(%s) Make Complete!!", tableName);
      if (++month > 12)
      {
        month = 1;
        ++year;
      }
    }
  }
}

void CMainThread::_db_Load_BattleTournamentInfo()
{
  TournamentWinner winners[48]{};

  const int maxCount = 48;
  if (this->m_pWorldDB->Select_BattleTournamentInfo(winners, maxCount))
  {
    this->m_BattleTournamentInfo.SetLoad(1);
    for (int j = 0; j < maxCount && winners[j].dwSerial != static_cast<unsigned int>(-1); ++j)
    {
      if (!this->m_BattleTournamentInfo.SetWinnerInfo(
            winners[j].dwSerial,
            winners[j].wszCharName,
            winners[j].byGrade))
      {
        this->m_logSystemError.Write("Battle Tournament Winner Info Setting Fail! Winner Info Disable!");
        this->m_BattleTournamentInfo.SetLoad(0);
        return;
      }
    }
  }
  else
  {
    this->m_logSystemError.Write("Can't Battle Tournament Winner Info Getting From DB!");
  }
}

char CMainThread::LoadLimitInfo()
{
  unsigned __int8 data[130]{};
  unsigned __int8 ret = this->m_pWorldDB->Select_LimitInfo(data, sizeof(data));
  if (ret == 1)
  {
    return 0;
  }

  unsigned __int8 hash[44]{};
  CCryptor *cryptor = CTSingleton<CCryptor>::Instance();
  if (!cryptor->MakeHash(g_cbHashVerify, 32, hash, 32))
  {
    return 0;
  }

  if (!std::memcmp(data, hash, 32))
  {
    unsigned int dwTerm = 1000 * (rand() % 21 + 10);
    this->m_tmCheckForceClose.BeginTimer(dwTerm);
    unsigned int sec = dwTerm / 1000 % 60;
    OutputDebugLog(
      "CMainThread::LoadLimitInfo() : Set Force Close Time( %u Min %u Sec )",
      dwTerm / 1000 / 60,
      sec);
  }

  return 1;
}

unsigned __int8 CMainThread::_db_Load_GoldBoxItem(qry_case_select_golden_box_item *pDbGoldenboxitem, int *pnDBSerial)
{
  _worlddb_golden_box_item goldenboxitem{};
  int pnSerial[4]{};

  unsigned __int8 ret = this->m_pWorldDB->Select_GodenBoxItem(&goldenboxitem, pnSerial);
  if (ret == 1)
  {
    return 24;
  }
  if (ret == 2)
  {
    if (!this->m_pWorldDB->Insert_GoldenBoxItem())
    {
      return 24;
    }
    if (this->m_pWorldDB->Select_GodenBoxItem(&goldenboxitem, pnSerial))
    {
      return 24;
    }
  }

  *pnDBSerial = pnSerial[0];
  pDbGoldenboxitem->bydck = goldenboxitem.bydck;
  pDbGoldenboxitem->dwStarterBoxCnt = goldenboxitem.dwStarterBoxCnt;

  for (int j = 0; ; ++j)
  {
    const unsigned __int8 loopCount = CGoldenBoxItemMgr::Instance()->GetLoopCount();
    if (j >= loopCount)
    {
      break;
    }

    pDbGoldenboxitem->nBoxcode[j] = goldenboxitem.nBox_item_code[j];
    pDbGoldenboxitem->wBoxMax[j] = goldenboxitem.wBox_item_max[j];
    pDbGoldenboxitem->bygolden_item_num[j] = goldenboxitem.bygolden_item_num[j];
    for (int k = 0; k < goldenboxitem.bygolden_item_num[j]; ++k)
    {
      pDbGoldenboxitem->List[j][k].ncode = goldenboxitem.List[j][k].nGoldencode;
      pDbGoldenboxitem->List[j][k].wcount = goldenboxitem.List[j][k].wGoldencount;
    }
  }

  return 0;
}


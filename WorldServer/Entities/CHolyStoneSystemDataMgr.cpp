#include "pch.h"

#include "CHolyStoneSystemDataMgr.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "CHolyStoneSystem.h"
#include "CMapOperation.h"
#include "CMapData.h"
#include "CHolyScheduleData.h"
#include "CHolyStoneSaveData.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

static const char *c_TempPair[7] =
{
  "HSS_SCENE_CHANGE_TERM",
  "HSS_SCENE_BATTLE_TERM",
  "HSS_SCENE_ATTACKABLE_KEEPER_TERM",
  "HSS_SCENE_DESATTACKABLE_KEEPER_TERM",
  "HSS_SCENE_CHAOS_KEEPER_TERM",
  "HSS_SCENE_BASE_MINIE_TERM",
  "HSS_SCENE_TOUCH_DOWN_ADD_TERM"
};

bool CHolyStoneSystemDataMgr::LoadIni(CHolyStoneSystem *clsHolyStoneSystem)
{
  char pszErrMsg[160]{};
  if (!clsHolyStoneSystem->m_tblQuest.ReadRecord(
        ".\\script\\HolyStoneKeepperQuest.dat",
        2408,
        pszErrMsg))
  {
    MyMessageBox("CHolyStoneSystem Data init", pszErrMsg);
    ServerProgramExit("CHolyStoneSystem Data init", 0);
  }

  char logPath[128]{};
  unsigned int now = GetKorLocalTime();
  sprintf_s(logPath, "..\\ZoneServerLog\\ServiceLog\\HolyStoneSystem%u.log", now);
  clsHolyStoneSystem->m_logQuest.SetWriteLogFile(logPath, 1, 0, 1, 1);

  now = GetKorLocalTime();
  sprintf(logPath, "..\\ZoneServerLog\\ServiceLog\\HolyStoneDestroy%u.log", now);
  clsHolyStoneSystem->m_logQuestDestroy.SetWriteLogFile(logPath, 1, 0, 1, 1);

  float keeperRate = static_cast<float>(GetPrivateProfileIntA(
    "HolySystem",
    "KeeperHPRate",
    100,
    ".\\Initialize\\NewHolySystem.ini"));
  clsHolyStoneSystem->m_fKeeperHPRate = keeperRate / 100.0f;

  float firstKeeperRate = static_cast<float>(GetPrivateProfileIntA(
    "HolySystem",
    "FirstKeeperHPRate",
    100,
    ".\\Initialize\\NewHolySystem.ini"));
  clsHolyStoneSystem->m_fFirstKeeperHPRate = firstKeeperRate / 100.0f;

  char returned[68]{};
  GetPrivateProfileStringA(
    "HolySystem",
    "KeeperCreateMap",
    "-1",
    returned,
    64,
    ".\\Initialize\\NewHolySystem.ini");
  if (strncmp(returned, "-1", 2) == 0)
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "Keeper Map Code Error(%s) : no defined", returned);
    return false;
  }

  clsHolyStoneSystem->m_HolyKeeperData.pCreateMap = g_MapOper.GetMap(returned);
  if (!clsHolyStoneSystem->m_HolyKeeperData.pCreateMap)
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "Keeper Map Code Error(%s) : unregistered map", returned);
    return false;
  }
  if (clsHolyStoneSystem->m_HolyKeeperData.pCreateMap->m_pMapSet->m_nMapType)
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "Keeper Map Code Error(%s) : no standard map", returned);
    return false;
  }

  GetPrivateProfileStringA(
    "HolySystem",
    "KeeperCreateDummy",
    "-1",
    returned,
    64,
    ".\\Initialize\\NewHolySystem.ini");
  if (strncmp(returned, "-1", 2) == 0)
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "Keeper create-dummy Code Error(%s) : no defined", returned);
    return false;
  }
  if (!clsHolyStoneSystem->m_HolyKeeperData.pCreateMap->LoadHolySystemDummy(returned, &clsHolyStoneSystem->m_HolyKeeperData.CreateDummy))
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "Keeper create-dummy Code Error(%s) : dummy error", returned);
    return false;
  }

  GetPrivateProfileStringA(
    "HolySystem",
    "KeeperActiveDummy",
    "-1",
    returned,
    64,
    ".\\Initialize\\NewHolySystem.ini");
  if (strncmp(returned, "-1", 2) == 0)
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "Keeper active-dummy Code Error(%s) : no defined", returned);
    return false;
  }
  if (!clsHolyStoneSystem->m_HolyKeeperData.pCreateMap->LoadHolySystemDummy(returned, &clsHolyStoneSystem->m_HolyKeeperData.ActiveDummy))
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "Keeper active-dummy Code Error(%s) : dummy error", returned);
    return false;
  }

  GetPrivateProfileStringA(
    "HolySystem",
    "KeeperCenterDummy",
    "-1",
    returned,
    64,
    ".\\Initialize\\NewHolySystem.ini");
  if (strncmp(returned, "-1", 2) == 0)
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "Keeper center-dummy Code Error(%s) : no defined", returned);
    return false;
  }
  if (!clsHolyStoneSystem->m_HolyKeeperData.pCreateMap->LoadHolySystemDummy(returned, &clsHolyStoneSystem->m_HolyKeeperData.CenterDummy))
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "Keeper center-dummy Code Error(%s) : dummy error", returned);
    return false;
  }

  GetPrivateProfileStringA(
    "HolySystem",
    "KeeperMonsterCode",
    "-1",
    returned,
    64,
    ".\\Initialize\\NewHolySystem.ini");
  if (strncmp(returned, "-1", 2) == 0)
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "Keeper Code Error(%s) :  no defined", returned);
    return false;
  }
  clsHolyStoneSystem->m_HolyKeeperData.pRec =
    reinterpret_cast<_monster_fld *>(g_Main.m_tblMonster.GetRecord(returned));
  if (!clsHolyStoneSystem->m_HolyKeeperData.pRec)
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "Keeper Code Error(%s) : unregistered code", returned);
    return false;
  }

  clsHolyStoneSystem->m_nHolyStoneNum = GetPrivateProfileIntA(
    "HolySystem",
    "StoneNum",
    0,
    ".\\Initialize\\NewHolySystem.ini");
  if (clsHolyStoneSystem->m_nHolyStoneNum > 3)
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "number of stone error(%d)", clsHolyStoneSystem->m_nHolyStoneNum);
    return false;
  }

  GetPrivateProfileStringA(
    "HolyWarFreeMining",
    "FreeMining",
    "FALSE",
    returned,
    64,
    ".\\Initialize\\NewHolySystem.ini");
  clsHolyStoneSystem->bFreeMining = strcmp_0(returned, "TRUE") == 0;

  GetPrivateProfileStringA(
    "HolySystem",
    "MentalPass",
    "FALSE",
    returned,
    64,
    ".\\Initialize\\NewHolySystem.ini");
  clsHolyStoneSystem->m_pMentalPass = strcmp_0(returned, "TRUE") == 0;

  for (int j = 0; j < clsHolyStoneSystem->m_nHolyStoneNum; ++j)
  {
    char key[96]{};
    sprintf(key, "StoneCreateMap%d", j);
    GetPrivateProfileStringA("HolySystem", key, "-1", returned, 64, ".\\Initialize\\NewHolySystem.ini");
    if (strncmp(returned, "-1", 2) == 0)
    {
      MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "%dth stone create-map code error(%s) : no defined", j, returned);
      return false;
    }
    __holy_stone_data *stone = &clsHolyStoneSystem->m_HolyStoneData[j];
    stone->pCreateMap = g_MapOper.GetMap(returned);
    if (!stone->pCreateMap)
    {
      MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "%dth stone create-map code error(%s) : unregistered map", j, returned);
      return false;
    }
    if (stone->pCreateMap->m_pMapSet->m_nMapType)
    {
      MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "%dth stone create-map code error(%s) : no standard map", j, returned);
      return false;
    }

    sprintf(key, "StoneCreateDummy%d", j);
    GetPrivateProfileStringA("HolySystem", key, "-1", returned, 64, ".\\Initialize\\NewHolySystem.ini");
    if (strncmp(returned, "-1", 2) == 0)
    {
      MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "%dth stone create-dummy code error(%s) : no defined", j, returned);
      return false;
    }
    if (!stone->pCreateMap->LoadHolySystemDummy(returned, &stone->CreateDummy))
    {
      MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "%dth stone create-dummy code error(%s) : dummy error", j, returned);
      return false;
    }

    sprintf(key, "StoneMonsterCode%d", j);
    GetPrivateProfileStringA("HolySystem", key, "-1", returned, 64, ".\\Initialize\\NewHolySystem.ini");
    if (strncmp(returned, "-1", 2) == 0)
    {
      MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "%dth stone code error(%s) : no defined", j, returned);
      return false;
    }
    stone->pRec = reinterpret_cast<_monster_fld *>(g_Main.m_tblMonster.GetRecord(returned));
    if (!stone->pRec)
    {
      MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "%dth stone code error(%s) : unregistered code", j, returned);
      return false;
    }

    sprintf(key, "StoneMasterRace%d", j);
    stone->nRace = GetPrivateProfileIntA("HolySystem", key, -1, ".\\Initialize\\NewHolySystem.ini");
    if (stone->nRace > 2)
    {
      MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "%dth stone master-race code error(%d)", j, stone->nRace);
      return false;
    }
  }

  GetPrivateProfileStringA(
    "HolySystem",
    "HolyMental",
    "-1",
    clsHolyStoneSystem->m_strHolyMental,
    64,
    ".\\Initialize\\NewHolySystem.ini");
  if (strncmp(returned, "-1", 2) == 0)
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "HolyMental Item not Setting (%s) : no defined", clsHolyStoneSystem->m_strHolyMental);
    return false;
  }

  int itemTable = GetItemTableCode(clsHolyStoneSystem->m_strHolyMental);
  if (itemTable == -1)
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "HolyMental Item not Setting (%s) : no Invalid code", clsHolyStoneSystem->m_strHolyMental);
    return false;
  }
  if (!g_Main.m_tblItemData[itemTable].GetRecord(clsHolyStoneSystem->m_strHolyMental))
  {
    MyMessageBox("CHolyStoneSystemDataeMgr::LoadIni()", "HolyMental Item not Setting (%s) : not find", clsHolyStoneSystem->m_strHolyMental);
    return false;
  }

  if (clsHolyStoneSystem->m_HolyKeeperData.pCreateMap)
  {
    char portalName[80]{};
    memset_0(portalName, 0, 64);
    GetPrivateProfileStringA("PortalDummyName", "BellaDummyName", "NULL", portalName, 64, ".\\Initialize\\NewHolySystem.ini");
    if (!strcmp_0("NULL", portalName))
    {
      MyMessageBox("CHolyStoneSystemDataMgr::LoadIni()", "Holy PortalDummy Setting Error<No Ini Portal Info:Bella>");
      return false;
    }
    clsHolyStoneSystem->m_pPortalDummy[0] = clsHolyStoneSystem->m_HolyKeeperData.pCreateMap->GetPortal(portalName);
    if (!clsHolyStoneSystem->m_pPortalDummy[0])
    {
      MyMessageBox("CHolyStoneSystemDataMgr::LoadIni()", "Holy PortalDummy Setting Error<No Ini Portal Info:Bella>");
      return false;
    }

    memset_0(portalName, 0, 64);
    GetPrivateProfileStringA("PortalDummyName", "CoraDummyName", "NULL", portalName, 64, ".\\Initialize\\NewHolySystem.ini");
    if (!strcmp_0("NULL", portalName))
    {
      MyMessageBox("CHolyStoneSystemDataMgr::LoadIni()", "Holy PortalDummy Setting Error<No Ini Portal Info:Cora>");
      return false;
    }
    clsHolyStoneSystem->m_pPortalDummy[1] = clsHolyStoneSystem->m_HolyKeeperData.pCreateMap->GetPortal(portalName);
    if (!clsHolyStoneSystem->m_pPortalDummy[1])
    {
      MyMessageBox("CHolyStoneSystemDataMgr::LoadIni()", "Holy PortalDummy Setting Error<No Ini Portal Info:Cora>");
      return false;
    }

    memset_0(portalName, 0, 64);
    GetPrivateProfileStringA("PortalDummyName", "AccDummyName", "NULL", portalName, 64, ".\\Initialize\\NewHolySystem.ini");
    if (!strcmp_0("NULL", portalName))
    {
      MyMessageBox("CHolyStoneSystemDataMgr::LoadIni()", "Holy PortalDummy Setting Error<No Ini Portal Info:Acc>");
      return false;
    }
    clsHolyStoneSystem->m_pPortalDummy[2] = clsHolyStoneSystem->m_HolyKeeperData.pCreateMap->GetPortal(portalName);
    if (!clsHolyStoneSystem->m_pPortalDummy[2])
    {
      MyMessageBox("CHolyStoneSystemDataMgr::LoadIni()", "Holy PortalDummy Setting Error<No Ini Portal Info:Acc>");
      return false;
    }
  }

  clsHolyStoneSystem->m_nRaceBattlePoint[0][1] = GetPrivateProfileIntA(
    "RaceBattlePoint",
    "WinEmptyScaner",
    3000,
    ".\\Initialize\\NewHolySystem.ini");
  clsHolyStoneSystem->m_nRaceBattlePoint[1][0] = GetPrivateProfileIntA(
    "RaceBattlePoint",
    "FailGetScaner",
    -1000,
    ".\\Initialize\\NewHolySystem.ini");
  clsHolyStoneSystem->m_nRaceBattlePoint[1][1] = GetPrivateProfileIntA(
    "RaceBattlePoint",
    "FailEmptyScaner",
    -2500,
    ".\\Initialize\\NewHolySystem.ini");
  clsHolyStoneSystem->m_nRaceBattlePoint[2][0] = GetPrivateProfileIntA(
    "RaceBattlePoint",
    "LoseGetScaner",
    -2000,
    ".\\Initialize\\NewHolySystem.ini");
  clsHolyStoneSystem->m_nRaceBattlePoint[2][1] = GetPrivateProfileIntA(
    "RaceBattlePoint",
    "LoseEmptyScaner",
    -5000,
    ".\\Initialize\\NewHolySystem.ini");
  return true;
}

bool CHolyStoneSystemDataMgr::LoadSceduleData(CHolyScheduleData *clsDummy)
{
  CHolyScheduleData::__HolyScheduleNode *scheduleNode = nullptr;
  char sectionName[68]{};
  int scheduleIndex = 0;
  int timeIndex = 0;
  char failCode = 0;
  UINT scheduleValue = static_cast<UINT>(-1);

  clsDummy->Init();
  clsDummy->m_nTotalSchedule = GetPrivateProfileIntA(
    "total_schedule_count",
    "schedule_count",
    0,
    ".\\Initialize\\NewHolySystem.ini");

  if (clsDummy->m_nTotalSchedule > 0)
  {
    const unsigned __int64 totalSchedule = static_cast<unsigned __int64>(clsDummy->m_nTotalSchedule);
    clsDummy->m_pSchedule = new CHolyScheduleData::__HolyScheduleNode[totalSchedule];
    for (scheduleIndex = 0; scheduleIndex < clsDummy->m_nTotalSchedule && !failCode; ++scheduleIndex)
    {
      scheduleValue = static_cast<UINT>(-1);
      sprintf(sectionName, "Schedule_%d", scheduleIndex);
      scheduleNode = &clsDummy->m_pSchedule[scheduleIndex];
      for (timeIndex = 0; timeIndex < 7; ++timeIndex)
      {
        scheduleValue = GetPrivateProfileIntA(sectionName, c_TempPair[timeIndex], -1, ".\\Initialize\\NewHolySystem.ini");
        if (scheduleValue == static_cast<UINT>(-1))
        {
          failCode = static_cast<char>(timeIndex + 2);
          break;
        }
        scheduleNode->m_nSceneTime[timeIndex] = static_cast<int>(scheduleValue);
      }
    }
  }
  else
  {
    failCode = 1;
  }

  if (failCode)
  {
    clsDummy->Init();
    return false;
  }

  clsDummy->m_bSet = true;
  return true;
}

bool CHolyStoneSystemDataMgr::LoadStateData(CHolyStoneSaveData *clsSaveDummy)
{
  clsSaveDummy->m_nSceneCode = GetPrivateProfileIntA("HSK", "scene", -1, "..\\SystemSave\\ServerState.ini");
  if (clsSaveDummy->m_nSceneCode == -1)
  {
    clsSaveDummy->m_nSceneCode = 0;
    g_Main.m_logLoadingError.Write("CHolyStoneSystemDataeMgr::LoadStateData() >> m_nSceneCode fail");
    return false;
  }

  clsSaveDummy->m_dwPassTimeInScene = GetPrivateProfileIntA("HSK", "pass", -1, "..\\SystemSave\\ServerState.ini");
  if (clsSaveDummy->m_dwPassTimeInScene == static_cast<unsigned int>(-1))
  {
    g_Main.m_logLoadingError.Write("CHolyStoneSystemDataeMgr::LoadStateData() >> m_dwPassTimeInScene fail");
    return false;
  }

  for (int j = 0; j < 3; ++j)
  {
    char key[44]{};
    sprintf(key, "%d S_hp", j);
    clsSaveDummy->m_nStoneHP_Buffer[j] = GetPrivateProfileIntA("HSK", key, -1, "..\\SystemSave\\ServerState.ini");
  }

  clsSaveDummy->m_nStartStoneHP = GetPrivateProfileIntA("HSK", "starthp", 1000000, "..\\SystemSave\\ServerState.ini");
  clsSaveDummy->m_nHolyMasterRace = GetPrivateProfileIntA("HSK", "master", 100, "..\\SystemSave\\ServerState.ini");
  if (clsSaveDummy->m_nHolyMasterRace == 100)
  {
    g_Main.m_logLoadingError.Write("CHolyStoneSystemDataeMgr::LoadStateData() >> m_nHolyMasterRace fail");
    return false;
  }

  clsSaveDummy->m_nDestroyStoneRace = GetPrivateProfileIntA("HSK", "destroyedrace", 100, "..\\SystemSave\\ServerState.ini");
  if (clsSaveDummy->m_nDestroyStoneRace == 100)
  {
    g_Main.m_logLoadingError.Write("CHolyStoneSystemDataeMgr::LoadStateData() >> m_byDestroyStoneRace fail");
    return false;
  }

  clsSaveDummy->m_dwTerm[0] = GetPrivateProfileIntA("HSK", "plustime", -1, "..\\SystemSave\\ServerState.ini");
  if (clsSaveDummy->m_dwTerm[0] == static_cast<unsigned int>(-1))
  {
    g_Main.m_logLoadingError.Write("CHolyStoneSystemDataeMgr::LoadStateData() >> m_dwKeeperPlusTime fail");
    return false;
  }

  clsSaveDummy->m_dwTerm[1] = GetPrivateProfileIntA("HSK", "controlterm", 0, "..\\SystemSave\\ServerState.ini");
  if (!clsSaveDummy->m_dwTerm[1])
  {
    g_Main.m_logLoadingError.Write("CHolyStoneSystemDataeMgr::LoadStateData() >> m_dwChaosTerm fail");
    return false;
  }

  clsSaveDummy->m_byNumOfTime = GetPrivateProfileIntA("HSK", "numoftime", 255, "..\\SystemSave\\ServerState.ini");
  clsSaveDummy->m_dwCumPlayerNum = GetPrivateProfileIntA("HSK", "cumplayernum", 0, "..\\SystemSave\\ServerState.ini");
  clsSaveDummy->m_dwCumCount = GetPrivateProfileIntA("HSK", "cumcount", 0, "..\\SystemSave\\ServerState.ini");

  const INT currentYear = GetCurrentYear();
  clsSaveDummy->m_wStartYear = GetPrivateProfileIntA("HSK", "startyear", currentYear, "..\\SystemSave\\ServerState.ini");
  const INT currentMonth = GetCurrentMonth();
  clsSaveDummy->m_byStartMonth = GetPrivateProfileIntA("HSK", "startmonth", currentMonth, "..\\SystemSave\\ServerState.ini");
  const INT currentDay = GetCurrentDay();
  clsSaveDummy->m_byStartDay = GetPrivateProfileIntA("HSK", "startday", currentDay, "..\\SystemSave\\ServerState.ini");
  const INT currentHour = GetCurrentHour();
  clsSaveDummy->m_byStartHour = GetPrivateProfileIntA("HSK", "starthour", currentHour, "..\\SystemSave\\ServerState.ini");
  const INT currentMin = GetCurrentMin();
  clsSaveDummy->m_byStartMin = GetPrivateProfileIntA("HSK", "startmin", currentMin, "..\\SystemSave\\ServerState.ini");

  UINT destroyer = GetPrivateProfileIntA("HSK", "destroyer", -1, "..\\SystemSave\\ServerState.ini");
  if (destroyer != static_cast<UINT>(-1))
    clsSaveDummy->m_dwDestroyerSerial = destroyer;

  UINT destroyerState = GetPrivateProfileIntA("HSK", "destroyerstate", 3, "..\\SystemSave\\ServerState.ini");
  if (destroyerState <= 2)
    clsSaveDummy->m_eDestroyerState = destroyerState;

  clsSaveDummy->m_dwOreRemainAmount = GetPrivateProfileIntA("HSK", "oreremain", -1, "..\\SystemSave\\ServerState.ini");
  clsSaveDummy->m_dwOreTotalAmount = GetPrivateProfileIntA("HSK", "oretotal", -1, "..\\SystemSave\\ServerState.ini");
  clsSaveDummy->m_dwDestroyerGuildSerial = GetPrivateProfileIntA("HSK", "destroyerguild", -1, "..\\SystemSave\\ServerState.ini");
  clsSaveDummy->m_byOreTransferCount = GetPrivateProfileIntA("HSK", "oretransfercount", 0, "..\\SystemSave\\ServerState.ini");
  clsSaveDummy->m_dwOreTransferAmount = GetPrivateProfileIntA("HSK", "oretransferamount", 0, "..\\SystemSave\\ServerState.ini");
  return true;
}

bool CHolyStoneSystemDataMgr::SaveStateData(CHolyStoneSaveData *clsSaveDummy)
{
  char buffer[44]{};

  _itoa(clsSaveDummy->m_nSceneCode, buffer, 10);
  WritePrivateProfileStringA("HSK", "scene", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_dwPassTimeInScene, buffer, 10);
  WritePrivateProfileStringA("HSK", "pass", buffer, "..\\SystemSave\\ServerState.ini");

  for (int j = 0; j < 3; ++j)
  {
    if (g_Stone[j].m_bOper)
    {
      const int hp = static_cast<int>(g_Stone[j].GetHP());
      _itoa(hp, buffer, 10);
      char key[56]{};
      sprintf(key, "%d S_hp", j);
      WritePrivateProfileStringA("HSK", key, buffer, "..\\SystemSave\\ServerState.ini");
    }
  }

  _itoa(clsSaveDummy->m_nStartStoneHP, buffer, 10);
  WritePrivateProfileStringA("HSK", "starthp", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_nHolyMasterRace, buffer, 10);
  WritePrivateProfileStringA("HSK", "master", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_nDestroyStoneRace, buffer, 10);
  WritePrivateProfileStringA("HSK", "destroyedrace", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_dwTerm[0], buffer, 10);
  WritePrivateProfileStringA("HSK", "plustime", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_dwTerm[1], buffer, 10);
  WritePrivateProfileStringA("HSK", "controlterm", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_byNumOfTime, buffer, 10);
  WritePrivateProfileStringA("HSK", "numoftime", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_dwCumPlayerNum, buffer, 10);
  WritePrivateProfileStringA("HSK", "cumplayernum", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_dwCumCount, buffer, 10);
  WritePrivateProfileStringA("HSK", "cumcount", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_wStartYear, buffer, 10);
  WritePrivateProfileStringA("HSK", "startyear", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_byStartMonth, buffer, 10);
  WritePrivateProfileStringA("HSK", "startmonth", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_byStartDay, buffer, 10);
  WritePrivateProfileStringA("HSK", "startday", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_byStartHour, buffer, 10);
  WritePrivateProfileStringA("HSK", "starthour", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_byStartMin, buffer, 10);
  WritePrivateProfileStringA("HSK", "startmin", buffer, "..\\SystemSave\\ServerState.ini");
  sprintf(buffer, "%u", clsSaveDummy->m_dwDestroyerSerial);
  WritePrivateProfileStringA("HSK", "destroyer", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_eDestroyerState, buffer, 10);
  WritePrivateProfileStringA("HSK", "destroyerstate", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_dwOreRemainAmount, buffer, 10);
  WritePrivateProfileStringA("HSK", "oreremain", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_dwOreTotalAmount, buffer, 10);
  WritePrivateProfileStringA("HSK", "oretotal", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_dwDestroyerGuildSerial, buffer, 10);
  WritePrivateProfileStringA("HSK", "destroyerguild", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_byOreTransferCount, buffer, 10);
  WritePrivateProfileStringA("HSK", "oretransfercount", buffer, "..\\SystemSave\\ServerState.ini");
  _itoa(clsSaveDummy->m_dwOreTransferAmount, buffer, 10);
  WritePrivateProfileStringA("HSK", "oretransferamount", buffer, "..\\SystemSave\\ServerState.ini");
  return true;
}

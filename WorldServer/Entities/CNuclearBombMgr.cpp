#include "pch.h"

#include "CNuclearBombMgr.h"

#include "CMapOperation.h"
#include "CNetworkEX.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "nuclear_create_setdata.h"
#include "nuclear_position_result_zocl.h"
#include "nuclear_result_code_zocl.h"

namespace
{
int damCnt[3][3]{};
int StartNum[3][3]{};
int LoopCnt[3][3]{};
}

int _nuclear_bomb_current_state_zocl::size() const
{
  return 127 - 14 * (9 - nNum);
}

CNuclearBombMgr *CNuclearBombMgr::Instance()
{
  static CNuclearBombMgr s_instance;
  return &s_instance;
}

void CNuclearBombMgr::Loop()
{
  for (int raceIndex = 0; raceIndex < 3; ++raceIndex)
  {
    for (int missileIndex = 0; missileIndex < 3; ++missileIndex)
    {
      CNuclearBomb *missile = &m_Missile[raceIndex][missileIndex];
      if (!missile->m_bLive)
      {
        continue;
      }

      if (missile->GetBombStatus() <= 3u)
      {
        damCnt[raceIndex][missileIndex] = 0;
        StartNum[raceIndex][missileIndex] = 0;
        LoopCnt[raceIndex][missileIndex] = 0;
      }

      if (missile->GetBombStatus() == 4)
      {
        damCnt[raceIndex][missileIndex] = missile->GetDamagedObjNum();
        missile->SetBombStatus();
        LoopCnt[raceIndex][missileIndex] = damCnt[raceIndex][missileIndex] / 30;
      }

      if (missile->GetBombStatus() >= 5u)
      {
        const int remainCount = damCnt[raceIndex][missileIndex] % 30;
        if (StartNum[raceIndex][missileIndex] <= LoopCnt[raceIndex][missileIndex])
        {
          if (StartNum[raceIndex][missileIndex] >= LoopCnt[raceIndex][missileIndex])
          {
            missile->Attack(StartNum[raceIndex][missileIndex], remainCount);
          }
          else
          {
            missile->Attack(StartNum[raceIndex][missileIndex], 30);
          }
          ++StartNum[raceIndex][missileIndex];
        }
      }
    }
  }
}

bool CNuclearBombMgr::MissileInit()
{
  for (int j = 0; j < 3; ++j)
  {
    _object_id objectId(0, 0xCu, static_cast<unsigned __int16>(j));
    for (int k = 0; k < 3; ++k)
    {
      if (!m_Missile[j][k].Init(&objectId))
      {
        return false;
      }
    }
  }
  return LoadIni();
}

bool CNuclearBombMgr::LoadIni()
{
  GetPrivateProfileStringA(
    "NuclearBomb",
    "NeedItem",
    "-1",
    m_szStickCode,
    0x40u,
    ".\\Initialize\\NuclearBomb.ini");

  if (!strncmp_0(m_szStickCode, "-1", 2))
  {
    MyMessageBox("CNuclearBombMgr::LoadIni()", "NeedItem Code Error(%s) : no defined", m_szStickCode);
    return false;
  }

  m_dwWarnTime =
    GetPrivateProfileIntA("NuclearBomb", "NuclearWarningTime", 10, ".\\Initialize\\NuclearBomb.ini");
  if (!m_dwWarnTime)
  {
    MyMessageBox("CNuclearBombMgrData init", "NuclearWarningTime is 0");
    ServerProgramExit("CNuclearBombMgrData Data init", 0);
  }

  m_dwAttInformTime =
    GetPrivateProfileIntA("NuclearBomb", "NuclearDropPosInform ", 6, ".\\Initialize\\NuclearBomb.ini");
  if (!m_dwAttInformTime)
  {
    MyMessageBox("CNuclearBombMgrData init", "NuclearDropPosInform is 0");
    ServerProgramExit("CNuclearBombMgrData Data init", 0);
  }

  m_dwAttStartTime =
    GetPrivateProfileIntA("NuclearBomb", "NuclearBombShowTime ", 4, ".\\Initialize\\NuclearBomb.ini");
  if (!m_dwAttStartTime)
  {
    MyMessageBox("CNuclearBombMgrData init", "NuclearBombShowTime is 0");
    ServerProgramExit("CNuclearBombMgrData Data init", 0);
  }

  return true;
}

void CNuclearBombMgr::CheckNuclearState(CPlayer *pOne)
{
  if (strcmp_0(pOne->m_pCurMap->m_pMapSet->m_strCode, "resources") != 0)
  {
    return;
  }

  _nuclear_bomb_current_state_zocl msg{};
  msg.nNum = 0;

  for (int j = 0; j < 3; ++j)
  {
    for (int k = 0; k < 3; ++k)
    {
      if (m_Missile[j][k].GetUse() && m_Missile[j][k].GetBombStatus() < 5u)
      {
        msg.nuclear[msg.nNum].byRaceCode = static_cast<unsigned __int8>(j);
        msg.nuclear[msg.nNum].byUseClass = static_cast<unsigned __int8>(k);
        float *pos = m_Missile[j][k].GetMissilePos();
        msg.nuclear[msg.nNum].zPos[0] = pos[0];
        msg.nuclear[msg.nNum].zPos[1] = pos[1];
        msg.nuclear[msg.nNum].zPos[2] = pos[2];
        ++msg.nNum;
      }
    }
  }

  if (msg.nNum)
  {
    unsigned __int8 type[2] = {60, 10};
    const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
    g_Network.m_pProcess[0]->LoadSendMsg(pOne->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
  }
}

unsigned __int8 CNuclearBombMgr::GetBossType(unsigned __int8 byRace, unsigned int dwSerial)
{
  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  const unsigned __int8 bossType = ranking->GetBossType(byRace, dwSerial);
  if (bossType == 0)
  {
    return 0;
  }
  if (bossType == 1)
  {
    return 1;
  }
  if (bossType == 5)
  {
    return 2;
  }
  return 3;
}

bool CNuclearBombMgr::IsPatriarch(CPlayer *pOne)
{
  const unsigned __int8 raceCode = pOne->m_Param.GetRaceCode();
  const unsigned int charSerial = pOne->m_Param.GetCharSerial();
  return GetBossType(raceCode, charSerial) < 3u;
}

void CNuclearBombMgr::SendMsg_Result(unsigned int n, unsigned __int8 byCode)
{
  _nuclear_result_code_zocl msg{};
  msg.byRetCode = byCode;

  unsigned __int8 type[2]{60, 0};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    type,
    reinterpret_cast<char *>(&msg),
    msg.size());
}

bool CNuclearBombMgr::CreateMissile(
  CPlayer *pMaster,
  float *fPos,
  unsigned int warnTime,
  unsigned int informTime,
  unsigned int startTime)
{
  const unsigned __int8 raceCode = pMaster->m_Param.GetRaceCode();
  const unsigned int charSerial = pMaster->m_Param.GetCharSerial();
  const unsigned __int8 bossType = GetBossType(raceCode, charSerial);

  _nuclear_create_setdata createData{};
  createData.m_pMap = pMaster->m_pCurMap;
  createData.pMaster = pMaster;
  createData.m_pRecordSet = g_Main.m_tblItemData[26].GetRecord(m_Missile[raceCode][bossType].GetItemIndex());
  if (!createData.m_pRecordSet)
  {
    return false;
  }

  memcpy_0(createData.m_fStartPos, fPos, sizeof(createData.m_fStartPos));
  createData.m_fStartPos[1] = pMaster->m_fCurPos[1];
  createData.m_dwWarnTime = warnTime;
  createData.m_dwAttInformTime = informTime;
  createData.m_dwAttStartTime = startTime;
  return m_Missile[raceCode][bossType].Create(&createData);
}

char CNuclearBombMgr::Request_EnableNuclearControl(int n, char *pMsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 0;
  }

  const unsigned __int8 raceCode = player->m_Param.GetRaceCode();
  const unsigned int charSerial = player->m_Param.GetCharSerial();
  const unsigned __int8 bossType = GetBossType(raceCode, charSerial);
  if (!IsPatriarch(player))
  {
    SendMsg_Result(n, 5u);
    return 1;
  }

  if (strcmp_0(player->m_pCurMap->m_pMapSet->m_strCode, "resources") != 0)
  {
    SendMsg_Result(n, 9u);
    return 1;
  }

  _STORAGE_LIST::_db_con *equip = &player->m_Param.m_dbEquip.m_pStorageList[6];
  _base_fld *record = nullptr;
  if (equip && equip->m_bLoad)
  {
    record = g_Main.m_tblItemData[6].GetRecord(equip->m_wItemIndex);
  }

  if (!equip || !equip->m_bLoad || !record || strcmp_0(record->m_strCode, m_szStickCode) != 0)
  {
    SendMsg_Result(n, 7u);
    return 1;
  }

  CNuclearBomb *missile = &m_Missile[raceCode][bossType];
  if (missile->GetUse())
  {
    SendMsg_Result(n, 6u);
    return 1;
  }

  if (player->GetStealth(true))
  {
    SendMsg_Result(n, 10u);
    return 1;
  }

  const unsigned __int16 controlSerial = *reinterpret_cast<unsigned __int16 *>(pMsg);
  _STORAGE_LIST::_db_con *item = player->m_Param.m_dbInven.GetPtrFromSerial(controlSerial);
  if (!item)
  {
    SendMsg_Result(n, 11u);
    return 0;
  }

  missile->SetNuclearIndex(item->m_wItemIndex);
  missile->SetControlSerial(controlSerial);
  SendMsg_Result(n, 0u);
  return 1;
}

char CNuclearBombMgr::Request_SelectDropPosition(int n, float *pMsg)
{
  CPlayer *master = &g_Player[n];
  if (!master->m_bOper || master->m_bCorpse)
  {
    SendMsg_Result(n, 12u);
    return 1;
  }

  CMapData *map = g_MapOper.GetMap(master->m_pCurMap->m_pMapSet->m_strCode);
  if (!map || strcmp_0(map->m_pMapSet->m_strCode, "resources") != 0)
  {
    SendMsg_Result(n, 9u);
    return 1;
  }

  if (!CreateMissile(master, pMsg, m_dwWarnTime, m_dwAttInformTime, m_dwAttStartTime))
  {
    SendMsg_Result(n, 11u);
    return 1;
  }

  const unsigned __int8 raceCode = master->m_Param.GetRaceCode();
  const unsigned int charSerial = master->m_Param.GetCharSerial();
  const unsigned __int8 bossType = GetBossType(raceCode, charSerial);
  for (unsigned int clientIndex = 0; clientIndex < MAX_PLAYER; ++clientIndex)
  {
    CPlayer *player = &g_Player[clientIndex];
    if (!player->m_bOper || !player->m_bLive || player->m_bCorpse)
    {
      continue;
    }
    if (strcmp_0(player->m_pCurMap->m_pMapSet->m_strCode, "resources") != 0)
    {
      continue;
    }
    if (player->m_Param.GetRaceCode() != raceCode)
    {
      continue;
    }

    _nuclear_position_result_zocl msg{};
    msg.dwMasterSerial = master->m_Param.GetCharSerial();
    msg.zPos[0] = pMsg[0];
    msg.zPos[1] = pMsg[1];
    msg.zPos[2] = pMsg[2];
    msg.byUseClass = bossType;

    unsigned __int8 type[2]{60, 3};
    m_Missile[raceCode][bossType].SendMsg_NuclearFind(clientIndex, raceCode);
    if (GetBossType(raceCode, player->m_Param.GetCharSerial()) >= 3u)
    {
      if (player->m_Param.m_pGuild)
      {
        const unsigned int guildMasterSerial = player->m_Param.m_pGuild->GetGuildMasterSerial();
        if (guildMasterSerial == player->m_Param.GetCharSerial())
        {
          g_Network.m_pProcess[0]->LoadSendMsg(
            clientIndex,
            type,
            reinterpret_cast<char *>(&msg),
            0x11u);
        }
      }
    }
    else
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        clientIndex,
        type,
        reinterpret_cast<char *>(&msg),
        0x11u);
    }
  }

  const unsigned __int16 controlSerial = m_Missile[raceCode][bossType].GetControlSerial();
  _STORAGE_LIST::_db_con *item = master->m_Param.m_dbInven.GetPtrFromSerial(controlSerial);
  if (!item)
  {
    return 0;
  }

  if (IsOverLapItem(item->m_byTableCode))
  {
    master->Emb_AlterDurPoint(0, item->m_byStorageIndex, -1, true, true);
  }
  else
  {
    if (!master->Emb_DelStorage(
          0,
          item->m_byStorageIndex,
          false,
          true,
          "CNuclearaBombMgr::Request_SelectDropPosition"))
    {
      SendMsg_Result(n, 11u);
      return 1;
    }

    CPlayer::s_MgrItemHistory.consume_del_item(
      master->m_ObjID.m_wIndex,
      item,
      master->m_szItemHistoryFileName);
  }

  SendMsg_Result(n, 2u);
  master->SendMsg_GestureInform(0xEu);
  return 1;
}


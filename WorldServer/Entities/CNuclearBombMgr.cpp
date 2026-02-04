#include "pch.h"

#include "CNuclearBombMgr.h"

#include "CNetworkEX.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

int _nuclear_bomb_current_state_zocl::size() const
{
  return 127 - 14 * (9 - nNum);
}

CNuclearBombMgr *CNuclearBombMgr::Instance()
{
  static CNuclearBombMgr s_instance;
  return &s_instance;
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


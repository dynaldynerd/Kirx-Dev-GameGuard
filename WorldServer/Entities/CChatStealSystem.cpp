#include "pch.h"

#include "CChatStealSystem.h"

#include "CNetProcess.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "GlobalObjects.h"
#include "chat_steal_message_gm_zocl.h"

namespace
{
unsigned int _SearchPlayer(char *szCharName)
{
  CUserDB *user = SearchAvatorWithName(g_UserDB, MAX_PLAYER, szCharName);
  if (!user)
  {
    return static_cast<unsigned int>(-1);
  }

  CPlayer *player = &g_Player[user->m_idWorld.wIndex];
  if (player->m_bLive)
  {
    return CPlayerDB::GetCharSerial(&player->m_Param);
  }

  return static_cast<unsigned int>(-1);
}
} // namespace

CChatStealSystem *CChatStealSystem::ms_pInstance = nullptr;

_ChatStealTargetInfo::_ChatStealTargetInfo()
{
  m_dwTargetSerial = static_cast<unsigned int>(-1);
  m_byStealType = 0;
  m_byRaceBoss = static_cast<unsigned __int8>(-1);
}

CChatStealSystem::CChatStealSystem()
{
  m_pGM = nullptr;
  m_TargetInfo = _ChatStealTargetInfo();
}

CChatStealSystem::~CChatStealSystem() = default;

CChatStealSystem *CChatStealSystem::Instance()
{
  if (!ms_pInstance)
  {
    ms_pInstance = new CChatStealSystem();
  }
  return ms_pInstance;
}

char CChatStealSystem::SetGm(CPlayer *pGM)
{
  if (m_pGM && m_pGM != pGM && pGM)
  {
    return 0;
  }

  m_pGM = pGM;
  return 1;
}

char CChatStealSystem::SetTargetInfoFromBoss(unsigned __int8 byType, unsigned __int8 byRaceCode)
{
  m_TargetInfo.m_dwTargetSerial = static_cast<unsigned int>(-1);
  m_TargetInfo.m_byRaceBoss = byRaceCode;
  m_TargetInfo.m_byStealType = byType;
  return 1;
}

char CChatStealSystem::SetTargetInfoFromCharacter(unsigned __int8 byType, char *szCharName)
{
  const unsigned int serial = _SearchPlayer(szCharName);
  if (serial == static_cast<unsigned int>(-1))
  {
    return 0;
  }

  m_TargetInfo.m_dwTargetSerial = serial;
  m_TargetInfo.m_byStealType = byType;
  return 1;
}

char CChatStealSystem::SetTargetInfoFromRace(unsigned __int8 byType, unsigned __int8 byRaceCode)
{
  m_TargetInfo.m_dwTargetSerial = static_cast<unsigned int>(-1);
  m_TargetInfo.m_byRaceBoss = byRaceCode;
  m_TargetInfo.m_byStealType = byType;
  return 1;
}

void CChatStealSystem::StealChatMsg(CPlayer *pPlayer, unsigned __int8 byChatType, char *szChatMsg)
{
  if (!m_pGM)
  {
    return;
  }
  if (!m_TargetInfo.m_byStealType)
  {
    return;
  }

  if (m_TargetInfo.m_byStealType <= 6u && pPlayer
      && m_TargetInfo.m_dwTargetSerial == CPlayerDB::GetCharSerial(&pPlayer->m_Param))
  {
    const unsigned __int8 byRaceCode = CPlayerDB::GetRaceCode(&pPlayer->m_Param);
    char *pwszSender = CPlayerDB::GetCharNameW(&pPlayer->m_Param);
    const unsigned int dwSerial = CPlayerDB::GetCharSerial(&pPlayer->m_Param);
    SendStealMsg(m_pGM, byChatType, dwSerial, pwszSender, byRaceCode, szChatMsg);
  }
}

void CChatStealSystem::SendStealMsg(
  CPlayer *pPlayer,
  unsigned __int8 byChatType,
  unsigned int dwSenderSerial,
  char *pwszSender,
  unsigned __int8 byRaceCode,
  char *pwszMessage)
{
  if (!pPlayer || !pPlayer->m_bLive)
  {
    return;
  }

  _chat_steal_message_gm_zocl msg{};
  msg.byMessageType = byChatType;
  msg.dwSenderSerial = dwSenderSerial;
  strcpy_0(msg.wszSenderName, pwszSender);
  msg.byRaceCode = byRaceCode;
  msg.bFiltering = false;
  msg.bySize = static_cast<unsigned __int8>(strlen_0(pwszMessage));
  memcpy_0(msg.wszChatData, pwszMessage, msg.bySize);
  msg.wszChatData[msg.bySize] = 0;
  msg.byPvpGrade = pPlayer->m_Param.m_byPvPGrade;

  unsigned __int8 type[2] = {2, 15};
  const unsigned __int16 len = msg.size();
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], pPlayer->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}


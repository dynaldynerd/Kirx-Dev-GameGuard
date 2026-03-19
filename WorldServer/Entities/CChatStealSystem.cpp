#include "pch.h"

#include "CChatStealSystem.h"
#include "GlobalObjects.h"
#include "chat_steal_message_gm_zocl.h"
#include "CNetProcess.h"
#include "CPlayer.h"

namespace
{
unsigned int SearchPlayerByName(char *szCharName)
{
  CUserDB *user = SearchAvatorWithName(g_UserDB, MAX_PLAYER, szCharName);
  if (!user)
  {
    return static_cast<unsigned int>(-1);
  }

  CPlayer *player = &g_Player[user->m_idWorld.wIndex];
  if (player->m_bLive)
  {
    return player->m_Param.GetCharSerial();
  }

  return static_cast<unsigned int>(-1);
}
} // namespace

CChatStealSystem *CChatStealSystem::ms_pInstance;

CChatStealSystem::CChatStealSystem() : m_pGM(nullptr), m_TargetInfo() {}

CChatStealSystem::~CChatStealSystem()
{
  // this is not a stub
}

CChatStealSystem *CChatStealSystem::Instance()
{
  if (!ms_pInstance)
  {
    ms_pInstance = new CChatStealSystem();
  }
  return ms_pInstance;
}

bool CChatStealSystem::SetGm(CPlayer *pGM)
{
  if (m_pGM && m_pGM != pGM && pGM)
  {
    return false;
  }
  m_pGM = pGM;
  return true;
}

bool CChatStealSystem::SetTargetInfoFromCharacter(unsigned __int8 byType, char *szCharName)
{
  const unsigned int targetSerial = SearchPlayerByName(szCharName);
  if (targetSerial == static_cast<unsigned int>(-1))
  {
    return false;
  }
  m_TargetInfo.m_dwTargetSerial = targetSerial;
  m_TargetInfo.m_byStealType = byType;
  return true;
}

bool CChatStealSystem::SetTargetInfoFromRace(unsigned __int8 byType, unsigned __int8 byRaceCode)
{
  m_TargetInfo.m_dwTargetSerial = static_cast<unsigned int>(-1);
  m_TargetInfo.m_byRaceBoss = byRaceCode;
  m_TargetInfo.m_byStealType = byType;
  return true;
}

bool CChatStealSystem::SetTargetInfoFromBoss(unsigned __int8 byType, unsigned __int8 byRaceCode)
{
  m_TargetInfo.m_dwTargetSerial = static_cast<unsigned int>(-1);
  m_TargetInfo.m_byRaceBoss = byRaceCode;
  m_TargetInfo.m_byStealType = byType;
  return true;
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
  std::strcpy(msg.wszSenderName, pwszSender);
  msg.byRaceCode = byRaceCode;
  msg.bFiltering = 0;
  msg.bySize = static_cast<unsigned __int8>(std::strlen(pwszMessage));
  std::memcpy(msg.wszChatData, pwszMessage, msg.bySize);
  msg.wszChatData[msg.bySize] = 0;
  msg.byPvpGrade = pPlayer->m_Param.m_byPvPGrade;

  unsigned __int8 pbyType[28]{};
  pbyType[0] = 2;
  pbyType[1] = 15;
  const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(
    pPlayer->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    nLen);
}

void CChatStealSystem::StealChatMsg(CPlayer *pPlayer, unsigned __int8 byChatType, char *szChatMsg)
{
  if (!m_pGM || !m_TargetInfo.m_byStealType)
  {
    return;
  }

  const unsigned __int8 stealType = m_TargetInfo.m_byStealType;
  if (stealType <= 6u && pPlayer
      && m_TargetInfo.m_dwTargetSerial == pPlayer->m_Param.GetCharSerial())
  {
    const int byRaceCode = static_cast<int>(pPlayer->m_Param.GetRaceCode());
    char *pwszSender = pPlayer->m_Param.GetCharNameW();
    const unsigned int charSerial = pPlayer->m_Param.GetCharSerial();
    SendStealMsg(m_pGM, byChatType, charSerial, pwszSender, static_cast<unsigned __int8>(byRaceCode), szChatMsg);
  }
}


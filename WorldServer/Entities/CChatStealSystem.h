#pragma once

#include "IdaCompat.h"

#include "ChatStealTargetInfo.h"

class CPlayer;

class __cppobj __declspec(align(8)) CChatStealSystem
{
public:
  CChatStealSystem();
  ~CChatStealSystem();

  static CChatStealSystem *Instance();

  bool SetGm(CPlayer *pGM);
  bool SetTargetInfoFromCharacter(unsigned __int8 byType, char *szCharName);
  bool SetTargetInfoFromRace(unsigned __int8 byType, unsigned __int8 byRaceCode);
  bool SetTargetInfoFromBoss(unsigned __int8 byType, unsigned __int8 byRaceCode);
  void SendStealMsg(
    CPlayer *pPlayer,
    unsigned __int8 byChatType,
    unsigned int dwSenderSerial,
    char *pwszSender,
    unsigned __int8 byRaceCode,
    char *pwszMessage);
  void StealChatMsg(CPlayer *pPlayer, unsigned __int8 byChatType, char *szChatMsg);

  static CChatStealSystem *ms_pInstance;

  CPlayer *m_pGM;
  _ChatStealTargetInfo m_TargetInfo;
};


#pragma once

#include "IdaCompat.h"

class CPlayer;

struct __declspec(align(4)) _ChatStealTargetInfo
{
  unsigned __int8 m_byStealType;
  unsigned int m_dwTargetSerial;
  unsigned __int8 m_byRaceBoss;

  _ChatStealTargetInfo();
};

class __cppobj __declspec(align(8)) CChatStealSystem
{
public:
  static CChatStealSystem *Instance();

  CChatStealSystem();
  ~CChatStealSystem();

  char SetGm(CPlayer *pGM);
  char SetTargetInfoFromBoss(unsigned __int8 byType, unsigned __int8 byRaceCode);
  char SetTargetInfoFromCharacter(unsigned __int8 byType, char *szCharName);
  char SetTargetInfoFromRace(unsigned __int8 byType, unsigned __int8 byRaceCode);
  void StealChatMsg(CPlayer *pPlayer, unsigned __int8 byChatType, char *szChatMsg);
  void SendStealMsg(
    CPlayer *pPlayer,
    unsigned __int8 byChatType,
    unsigned int dwSenderSerial,
    char *pwszSender,
    unsigned __int8 byRaceCode,
    char *pwszMessage);

  static CChatStealSystem *ms_pInstance;

  CPlayer *m_pGM;
  _ChatStealTargetInfo m_TargetInfo;
};


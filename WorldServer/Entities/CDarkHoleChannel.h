#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"
#include "CIndexList.h"

/* 1598 */
class __cppobj __declspec(align(8)) CDarkHoleChannel
{
public:
  unsigned __int16 m_wChannelIndex;
  unsigned int m_dwChannelSerial;
  CDarkHole *m_pHoleObj;
  unsigned int m_dwHoleSerial;
  unsigned int m_dwQuestStartTime;
  _dh_quest_setup *m_pQuestSetup;
  unsigned __int16 m_wLayerIndex;
  _LAYER_SET *m_pLayerSet;
  _dh_mission_mgr m_MissionMgr;
  char m_wszOpenerName[17];
  char m_aszOpenerName[17];
  unsigned int m_dwOpenerSerial;
  int m_nOpenerDegree;
  int m_nOpenerSubDegree;
  bool m_bCheckMemberClose;
  CPartyPlayer *m_pPartyMng;
  _dh_player_mgr m_Quester[32];
  _dh_player_mgr *m_pLeaderPtr;
  unsigned int m_dwEnterOrderCounter;
  unsigned int m_dwNextCloseTime;
  unsigned int m_dwSendNewMissionMsgNextTime;
  CIndexList m_listEnterMember;
  bool m_bMoveNextMission;
  virtual ~CDarkHoleChannel() = default;
};


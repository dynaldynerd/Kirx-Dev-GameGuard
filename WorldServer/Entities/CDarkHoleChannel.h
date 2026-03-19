#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"
#include "CIndexList.h"

class CGameObject;
struct _ENTER_DUNGEON_NEW_POS;

/* 1598 */
class  CDarkHoleChannel
{
public:
  CDarkHoleChannel();
  virtual ~CDarkHoleChannel();

  static unsigned int s_dwChannelSerialCounter;

  struct  __enter_member
  {
    bool bActive;
    bool bDisnormalClose;
    unsigned int dwDisconnectTime;

    __enter_member();
    __enter_member(bool active, bool disnormal, unsigned int time);
  };

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

  void OnLoop();
  void Init();
  void OpenDungeon(_dh_quest_setup *pQuestSetup, int nLayerIndex, CPlayer *pOpener, CDarkHole *pHoleObj);
  void CreateMonster();
  void CheckWaitNextMission();
  void CheckCurrentMission();
  void CheckMember();
  void CloseDungeon();
  void CheckInnerEventDummy();
  void CheckRespawnMonster();
  void CheckSendNewMissionMsg();
  bool ClearMember(CPlayer *pMember, bool bDisconnect, _dh_player_mgr::_pos *poutPlayerPos);
  _dh_player_mgr *GetPlayerInfo(unsigned int dwSerial);
  void ChangeMonsterApparition(unsigned int nTermMSec);
  int GetMonsterNumInCurMissionArea(int nMonsterRecIndex);
  bool GotoNextMission();
  void WaitNextMission();
  bool IsOpenPartyMember(CPlayer *pOpener);
  bool IsFill();
  int GetAllMemberNum();
  int GetCurrentMemberNum();
  bool CanYouEnterHole(CPlayer *pEnter);
  bool GetEnterNewPos(_ENTER_DUNGEON_NEW_POS *pNewPos);
  bool IsReEnterable(unsigned int dwEnterSerial);
  bool PushMember(
    CPlayer *pMember,
    bool bReconnect,
    CMapData *pOldMap,
    unsigned __int16 wLastLayer,
    float *pfOldPos);
  bool CheckEvent(EM_DH_EVENT eventType, int nContentTable, int nContentIndex, int nCount, CGameObject *pObj);
  void AddMonster();
  void ChangeMonster();
  void ShareItemToMonster();
  unsigned int GetLeaderSerial();
  bool IsMoveNextMission(int nPortalIndex);
  bool IsAllMemberNearPosition(float *pfCenterPos, int nLen);
  _dh_mission_setup *SearchMissionFromPos(float *pfStartPos);
  bool GotoNextMissionByPosition(float *pfStartPos);
  void NextMissionOtherQuester(CPlayer *pLeader, _dh_mission_setup *pNextMission);
  void SendMsg_AskReEnter(CPlayer *pDst);
  void SendMsg_ChannelClose();
  void SendMsg_GateDestroy(unsigned __int8 *byType, char *pSend, unsigned __int16 nSize);
  void SendMsg_JobPass(unsigned __int8 nJobIndex);
  void SendMsg_MissionPass();
  void SendMsg_NewMission();
  void SendMsg_OpenPortalByReact(unsigned __int16 nPortalIndex);
  void SendMsg_OpenPortalByResult(int nPortalIndex);
  void SendMsg_QuestPass();
  void SendMsg_TimeOut();
  void SendMsg_JobCount(unsigned __int8 nJobIndex, unsigned __int16 nCount);
  void SendMsg_NewMember(CPlayer *pNewMember, bool bReconnect);
  void SendMsg_QuestInfo(CPlayer *pDst);
  void SendMsg_MissionInfo(CPlayer *pDst);
  void SendMsg_MemberInfo(CPlayer *pDst);
  void SendMsg_RealMsgInform(char *pMsg);
  void SendMsg_RealAddLimTime(int nAddSec, char *pMsg);
  void SendMsg_PopMember(CPlayer *pPopMember, bool bDisconnect);
  void SendMsg_LeaderChange(CPlayer *pNewLeader);
  bool _Reward();
};



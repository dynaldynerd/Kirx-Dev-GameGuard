#pragma once

#include "IdaCompat.h"
#include "CMapData.h"
#include "CMyTimer.h"
#include "CNetIndexList.h"
#include "GlobalObjectDefs.h"
#include "TicketItem_fld.h"

class CPlayer;

class __cppobj CTransportShip
{
public:
  bool InitShip(CMapData *pLinkShipMap, CMapData *pLinkMainbaseMap, CMapData *pLinkPlatformMap, int nIndex);
  void AlterState(bool bAnchor, unsigned __int8 byDirect, int nPassMin, int nNextSubEventTerm);
  void KickFreeMember();
  void KickOldMember(unsigned __int8 byKickDirectCode);
  void ExitMember(CPlayer *pExiter, bool bLogoff);
  bool Ticketting(CPlayer *pExiter);
  void SendMsg_KickForSail(unsigned int n);
  void SendMsg_TicketCheck(unsigned int n, char bPass, unsigned __int16 wTicketSerial);
  void SendMsg_TransportShipState(int n);
  void CheckTicket_Pass(CPlayer *pPtr, int nPortalIndex);
  void CheckTicket_Kick(CPlayer *pPtr, int nPortalIndex);
  int GetOutPortalIndex(int nRaceCode, unsigned __int8 byExitDirect);
  bool GetCurRideShipThisTicket(_TicketItem_fld *pTicketFld);
  bool IsMemberBeforeLogoff(unsigned int dwPlayerSerial);
  void GetStartPosInShip(float *pfPos);

  struct __cppobj __declspec(align(8)) __mgr_member
  {
    CPlayer *pPtr;
    unsigned int dwSerial;

    void init();
    bool is_fill() const;
  };

  struct __cppobj __mgr_ticket
  {
    int nCurTicketNum;
    int nReserTicketNum;
    unsigned int dwNextUpdateTime;
    _TicketItem_fld *pLinkTicketItem;

    void init();
  };

  bool m_bAnchor;
  unsigned __int8 m_byDirect;
  bool m_bHurry;
  unsigned int m_dwNextHurryTime;
  unsigned int m_dwEventCreateTime;
  unsigned __int8 m_byRaceCode_Layer;
  CMapData *m_pLinkShipMap;
  CMapData *m_pLinkPortMap[2];
  CMyTimer m_tmrCheckState;
  __mgr_member m_NewMember[MAX_PLAYER];
  __mgr_member m_OldMember[MAX_PLAYER];
  CNetIndexList m_listLogoffMember;
  __mgr_ticket m_MgrTicket[2];
};

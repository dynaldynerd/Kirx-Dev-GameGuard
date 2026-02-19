#pragma once

#include "IdaCompat.h"
#include "CGameObject.h"

class CDarkHoleChannel;
class CPlayer;
struct _darkhole_create_setdata;
struct _ENTER_DUNGEON_NEW_POS;

/* 1600 */
class  CDarkHole : public CGameObject
{
public:
  static int s_nLiveNum;
  static unsigned int s_dwSerialCounter;

  CDarkHoleChannel *m_pChannel;
  unsigned int m_dwChannelSerial;
  unsigned int m_dwOpenerSerial;
  char m_wszOpenerName[17];
  char m_aszOpenerName[17];
  unsigned int m_dwNextCloseTime;
  unsigned int m_dwNextHurryTime;
  bool m_bHurry;

  void Init(_object_id *pID);
  char Create(_darkhole_create_setdata *pParam);
  bool Destroy();
  bool EnterPlayer(
    CPlayer *pEnter,
    CMapData *pOldMap,
    unsigned __int16 wOldLayer,
    float *fOldPos,
    bool bReconnect);
  bool IsNewEnterAbleNum();
  bool IsNewEnterAblePlayer(CPlayer *pEnter);
  bool GetEnterNewPos(_ENTER_DUNGEON_NEW_POS *pNewPos);
  void Loop() override;
  void SendMsg_Create();
  void SendMsg_Destroy();
  void SendMsg_StateChange();
  void SendMsg_FixPosition(int n) override;
};

CDarkHole *SerarchEmptyDarkHole();
char IsExistDarkHoleOpenGate();



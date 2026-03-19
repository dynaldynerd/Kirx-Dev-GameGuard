#pragma once

#include "IdaCompat.h"

#include "CNuclearBomb.h"

class CPlayer;
struct _nuclear_bomb_current_state_zocl;

class  CNuclearBombMgr
{
public:
  CNuclearBombMgr();
  virtual ~CNuclearBombMgr();

  static CNuclearBombMgr *m_pkInstance;
  static CNuclearBombMgr *Instance();
  static void Destroy(CNuclearBombMgr *thisPtr);
  bool MissileInit();
  bool LoadIni();
  void Loop();
  void CheckNuclearState(CPlayer *pOne);
  bool Request_EnableNuclearControl(int n, char *pMsg);
  bool Request_SelectDropPosition(int n, float *pMsg);
  void SendMsg_Result(unsigned int n, unsigned __int8 byCode);
  unsigned __int8 GetBossType(unsigned __int8 byRace, unsigned int dwSerial);
  bool IsPatriarch(CPlayer *pOne);
  bool CreateMissile(
    CPlayer *pMaster,
    float *fPos,
    unsigned int warnTime,
    unsigned int informTime,
    unsigned int startTime);

  char m_szStickCode[64];
  CNuclearBomb m_Missile[3][3];
  unsigned int m_dwWarnTime;
  unsigned int m_dwAttInformTime;
  unsigned int m_dwAttStartTime;
};


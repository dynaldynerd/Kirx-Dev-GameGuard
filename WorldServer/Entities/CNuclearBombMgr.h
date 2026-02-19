#pragma once

#include "IdaCompat.h"

#include "CNuclearBomb.h"

class CPlayer;

#pragma pack(push, 1)
struct  _nuclear_bomb_current_state_zocl
{
  #pragma pack(push, 1)
  struct  __nuclear
  {
    unsigned __int8 byRaceCode;
    unsigned __int8 byUseClass;
    float zPos[3];
  };
  #pragma pack(pop)

  unsigned __int8 nNum;
  __nuclear nuclear[9];

  int size() const;
};
#pragma pack(pop)

class  CNuclearBombMgr
{
public:
  static CNuclearBombMgr *Instance();
  bool MissileInit();
  bool LoadIni();
  void Loop();
  void CheckNuclearState(CPlayer *pOne);
  char Request_EnableNuclearControl(int n, char *pMsg);
  char Request_SelectDropPosition(int n, float *pMsg);
  void SendMsg_Result(unsigned int n, unsigned __int8 byCode);
  unsigned __int8 GetBossType(unsigned __int8 byRace, unsigned int dwSerial);
  bool IsPatriarch(CPlayer *pOne);
  bool CreateMissile(
    CPlayer *pMaster,
    float *fPos,
    unsigned int warnTime,
    unsigned int informTime,
    unsigned int startTime);

  virtual ~CNuclearBombMgr() = default;

  char m_szStickCode[64];
  CNuclearBomb m_Missile[3][3];
  unsigned int m_dwWarnTime;
  unsigned int m_dwAttInformTime;
  unsigned int m_dwAttStartTime;
};


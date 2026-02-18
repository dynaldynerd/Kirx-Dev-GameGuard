#pragma once

#include "IdaCompat.h"

class CMonster;
class CCharacter;
class CPlayer;
class CMapData;
class CGameObject;
struct _NEAR_DATA;

struct  __declspec(align(8)) _NEAR_DATA
{
  float fLen;
  CCharacter *pChar;
  int bCanYouGoThere;

  _NEAR_DATA();
  void Init();
};

class  CMonsterHelper
{
public:
  static bool CheckPreAttackRangeTargetAbleCharacter(CMonster *pMon, CGameObject *pTarget);
  static float GetAngle(float *mon, float *plr);
  static void GetDirection(float (*cur)[3], float (*tar)[3], float (*out)[3], float deg);
  static void HierarcyHelpCast(CMonster *pMon);
  static bool IsInSector(float *chkpos, float *src, float *dest, float angle, float radius, float *pfDist);
  static CMonster *SearchNearMonsterByDistance(CMonster *pMon, int dwDist);
  static unsigned int SearchNearMonster(CMonster *pMon, _NEAR_DATA *NearChar, unsigned int dwArSize, int bTargetIgnore);
  static CPlayer *SearchNearPlayer(CMonster *pMon, int nType);
  static bool SearchPatrolMovePos(CMonster *mon, float (*NewTar)[3]);
  static bool SearchTargetMovePos_MovingTarget(CMonster *pMon, CCharacter *pTargetCharacter, float (*tarPos)[3]);
  static bool SearchTargetMovePos_StopTarget(CMonster *pMon, CCharacter *pTargetCharacter, float (*tarPos)[3]);
  static void TransPort(CMonster *mon, float *tarPos);
};

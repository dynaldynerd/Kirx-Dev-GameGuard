#pragma once

#include "IdaCompat.h"

#include <vector>

class CMoveMapLimitRight;
class CPlayer;

class __cppobj CMoveMapLimitRightInfo
{
public:
  CMoveMapLimitRightInfo();
  CMoveMapLimitRightInfo(const CMoveMapLimitRightInfo &rhs);
  ~CMoveMapLimitRightInfo();

  CMoveMapLimitRightInfo &operator=(const CMoveMapLimitRightInfo &rhs);
  bool Regist(int iType);
  void Load(CPlayer *pkPlayer);
  void LogIn(CPlayer *pkPlayer);
  void LogOut(CPlayer *pkPlayer);
  void CreateComplete(CPlayer *pkPlayer);
  bool IsHaveRight(int iType);
  void SetFlag(int iType, unsigned int iSubType, bool bFlag);

  std::vector<CMoveMapLimitRight *> m_vecRight;

private:
  void CleanUp();
};

#pragma once

#include "IdaCompat.h"

#include <vector>

#include "CMoveMapLimitRightInfo.h"

class CPlayer;

class __cppobj CMoveMapLimitRightInfoList
{
public:
  CMoveMapLimitRightInfoList();
  ~CMoveMapLimitRightInfoList();

  bool Init(const std::vector<int> *vecRightTypeList);
  CMoveMapLimitRightInfo *Get(int iInx);
  void Load(CPlayer *pkPlayer);
  void LogIn(CPlayer *pkPlayer);
  void LogOut(CPlayer *pkPlayer);
  void CreateComplete(CPlayer *pkPlayer);

  std::vector<CMoveMapLimitRightInfo> m_vecRight;
};

#pragma once

#include "IdaCompat.h"

#include <vector>

class CMoveMapLimitInfo;
class CMoveMapLimitRightInfo;
class CPlayer;

class  CMoveMapLimitInfoList
{
public:
  CMoveMapLimitInfoList();
  ~CMoveMapLimitInfoList();

  bool Init(std::vector<int> *vecRightTypeList);
  void Loop();
  CMoveMapLimitInfo *Get(int iLimitType, int iMapInx, unsigned int dwStoreRecordIndex);
  void Load(CPlayer *pkPlayer, CMoveMapLimitRightInfo *pkRight);
  void LogIn(CPlayer *pkPlayer, CMoveMapLimitRightInfo *pkRight);
  void LogOut(CPlayer *pkPlayer, CMoveMapLimitRightInfo *pkRight);
  unsigned __int8 Request(
    int iLimitType,
    unsigned int iRequetType,
    int iMapInx,
    unsigned int dwStoreRecordIndex,
    unsigned int iUserInx,
    char *pRequest,
    CMoveMapLimitRightInfo *pkRight);

  std::vector<CMoveMapLimitInfo *> m_vecLimitInfo;
};

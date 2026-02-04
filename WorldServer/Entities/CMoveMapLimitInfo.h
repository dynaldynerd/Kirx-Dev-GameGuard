#pragma once

#include "IdaCompat.h"

class CMerchant;
class CMoveMapLimitRightInfo;
class CPlayer;

class __cppobj CMoveMapLimitInfo
{
public:
  enum OBJ_TYPE : int
  {
    OT_NONE = -1,
    OT_PORTAL = 0,
    MAX_TYPE_NUM = 1,
  };

  CMoveMapLimitInfo(unsigned int uiInx, OBJ_TYPE iType);
  virtual ~CMoveMapLimitInfo() = default;
  virtual unsigned __int8 Request(int iUserInx, int iRequetType, char *pRequest, CMoveMapLimitRightInfo *pkRight) = 0;
  virtual void Load(CPlayer *pkPlayer, CMoveMapLimitRightInfo *pkRight);
  virtual void LogIn(CPlayer *pkPlayer, CMoveMapLimitRightInfo *pkRight);
  virtual void LogOut(CPlayer *pkPlayer, CMoveMapLimitRightInfo *pkRight);
  virtual bool Init() = 0;
  virtual void Loop();
  bool IsEqualLimit(int iType, int iMapInx, unsigned int dwStoreRecordIndex);
  OBJ_TYPE GetType() const { return m_eType; }
  unsigned int GetInx() const { return m_uiInx; }

  static CMoveMapLimitInfo *Create(unsigned int uiInx, int iType);

  OBJ_TYPE m_eType;
  unsigned int m_uiInx;
  int m_iMapInx;
  CMerchant *m_pStoreNPC;
};

#pragma once

#include "IdaCompat.h"
#include "ObjectCreateSetData.h"

class CPlayer;

class  CReturnGateCreateParam : public _object_create_setdata
{
public:
  CReturnGateCreateParam(CPlayer *pkOwner);
  ~CReturnGateCreateParam();
  CPlayer *GetOwner();

  CPlayer *m_pkOwner;
};

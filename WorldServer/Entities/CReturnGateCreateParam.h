#pragma once

#include "IdaCompat.h"
#include "ObjectCreateSetData.h"

class CPlayer;

class __cppobj CReturnGateCreateParam : public _object_create_setdata
{
public:
  CPlayer *m_pkOwner;
};

#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)

#include "DfnEquipItem_fld.h"

struct  _CloakItem_fld : _DfnEquipItem_fld
{
  float m_fBoosterSpeed;
};

#pragma pack(pop)

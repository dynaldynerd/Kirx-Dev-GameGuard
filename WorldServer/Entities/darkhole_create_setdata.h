#pragma once

#include "IdaCompat.h"
#include "ObjectCreateSetData.h"

class CPlayer;

struct  _darkhole_create_setdata : _object_create_setdata
{
  CPlayer *pOpener;

  _darkhole_create_setdata()
  {
    pOpener = nullptr;
  }
};

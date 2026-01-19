#pragma once

#include "IdaCompat.h"
#include "CRecordData.h"

class __cppobj CWeaponBulletLinkTable
{
public:
  CRecordData m_tblBullet;
  int **m_ppnWeaponIndex;
  int m_nBulletNum;
};

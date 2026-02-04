#include "pch.h"

#include "CPlayerDB.h"

int CPlayerDB::GetRaceCode(CPlayerDB *self)
{
  return static_cast<int>(self->m_dbChar.m_byRaceSexCode) >> 1;
}

int CPlayerDB::GetMapCode(CPlayerDB *self)
{
  return static_cast<unsigned int>(self->m_dbChar.m_sStartMapCode);
}

void CPlayerDB::SetMapCode(CPlayerDB *self, unsigned __int8 byCode)
{
  self->m_dbChar.m_sStartMapCode = byCode;
}

char *CPlayerDB::GetCharNameA(CPlayerDB *self)
{
  return self->m_aszName;
}

const char *CPlayerDB::GetCharNameW(CPlayerDB *self)
{
  return self->m_dbChar.m_wszCharID;
}

unsigned int CPlayerDB::GetCharSerial(CPlayerDB *self)
{
  return self->m_dbChar.m_dwSerial;
}

long double CPlayerDB::GetPvPPoint(CPlayerDB *self)
{
  return self->m_dbChar.m_dPvPPoint;
}

void CPlayerDB::SetPvPPoint(CPlayerDB *self, long double dPoint)
{
  self->m_dbChar.m_dPvPPoint = dPoint;
}

unsigned int CPlayerDB::GetGuildSerial(CPlayerDB *self)
{
  if (self->m_pGuild)
  {
    return self->m_pGuild->m_dwSerial;
  }
  return static_cast<unsigned int>(-1);
}

unsigned int CPlayerDB::GetLevel(CPlayerDB *self)
{
  return self->m_dbChar.m_byLevel;
}

unsigned int CPlayerDB::GetHP(CPlayerDB *self)
{
  return self->m_dbChar.m_dwHP;
}

void CPlayerDB::SetHP(CPlayerDB *self, unsigned int dwHP)
{
  self->m_dbChar.m_dwHP = dwHP;
}

unsigned int CPlayerDB::GetFP(CPlayerDB *self)
{
  return self->m_dbChar.m_dwFP;
}

void CPlayerDB::SetFP(CPlayerDB *self, unsigned int dwFP)
{
  self->m_dbChar.m_dwFP = dwFP;
}

unsigned int CPlayerDB::GetSP(CPlayerDB *self)
{
  return self->m_dbChar.m_dwSP;
}

void CPlayerDB::SetSP(CPlayerDB *self, unsigned int dwSP)
{
  self->m_dbChar.m_dwSP = dwSP;
}

unsigned int CPlayerDB::GetDalant(CPlayerDB *self)
{
  return self->m_dbChar.m_dwDalant;
}

void CPlayerDB::SetDalant(CPlayerDB *self, unsigned int dwDt)
{
  self->m_dbChar.m_dwDalant = dwDt;
}

unsigned int CPlayerDB::GetGold(CPlayerDB *self)
{
  return self->m_dbChar.m_dwGold;
}

void CPlayerDB::SetCurPos(CPlayerDB *self, float *fPos)
{
  memcpy_0(self->m_dbChar.m_fStartPos, fPos, sizeof(self->m_dbChar.m_fStartPos));
}

_SFCONT_DB_BASE::_SFCONT_DB_BASE()
{
  Init();
}

void _SFCONT_DB_BASE::Init()
{
  for (int j = 0; j < 2; ++j)
  {
    for (int k = 0; k < 8; ++k)
    {
      m_List[j][k].Init();
    }
  }
}

void _SFCONT_DB_BASE::_LIST::Init()
{
  dwKey = static_cast<unsigned int>(-1);
}

bool _SFCONT_DB_BASE::_LIST::IsFilled() const
{
  return dwKey != static_cast<unsigned int>(-1);
}

unsigned int _SFCONT_DB_BASE::_LIST::GetEffectCode() const
{
  return (dwKey & 0x0FFFFFFF) >> 26;
}

unsigned int _SFCONT_DB_BASE::_LIST::GetEffectIndex() const
{
  return (dwKey & 0x03FFFFFF) >> 16;
}

unsigned int _SFCONT_DB_BASE::_LIST::GetLv() const
{
  return static_cast<unsigned int>(static_cast<unsigned __int16>(dwKey) >> 12);
}

unsigned int _SFCONT_DB_BASE::_LIST::GetLeftTime() const
{
  return dwKey & 0x0FFF;
}

unsigned int _SFCONT_DB_BASE::_LIST::GetOrder() const
{
  return dwKey >> 28;
}

void _SFCONT_DB_BASE::_LIST::SetKey(
  unsigned __int8 byOrder,
  unsigned __int8 byEffectCode,
  unsigned __int16 wEffectIndex,
  unsigned __int8 byLv,
  unsigned __int16 wLeftTime)
{
  dwKey = 0;
  dwKey |= static_cast<unsigned int>(byOrder) << 28;
  dwKey |= static_cast<unsigned int>(byEffectCode) << 26;
  dwKey |= static_cast<unsigned int>(wEffectIndex) << 16;
  dwKey |= static_cast<unsigned int>(byLv) << 12;
  dwKey |= static_cast<unsigned int>(wLeftTime);
}

void _SFCONT_DB_BASE::_LIST::SetLeftTime(unsigned __int16 wLeftTime)
{
  dwKey &= 0xFFFFF000;
  dwKey |= (static_cast<unsigned int>(wLeftTime) & 0x0FFF);
}

void _SFCONT_DB_BASE::_LIST::SetOrder(unsigned __int8 byOrder)
{
  dwKey = (dwKey & 0x0FFFFFFF) | (static_cast<unsigned int>(byOrder) << 28);
}


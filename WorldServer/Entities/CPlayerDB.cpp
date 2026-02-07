#include "pch.h"

#include "CPlayerDB.h"

int CPlayerDB::GetRaceCode()
{
  return static_cast<int>(this->m_dbChar.m_byRaceSexCode) >> 1;
}

int CPlayerDB::GetRaceSexCode()
{
  return this->m_dbChar.m_byRaceSexCode;
}

int CPlayerDB::GetMapCode()
{
  return static_cast<unsigned int>(this->m_dbChar.m_sStartMapCode);
}

void CPlayerDB::SetMapCode(unsigned __int8 byCode)
{
  this->m_dbChar.m_sStartMapCode = byCode;
}

char *CPlayerDB::GetCharNameA()
{
  return this->m_aszName;
}

const char *CPlayerDB::GetCharNameW()
{
  return this->m_dbChar.m_wszCharID;
}

unsigned int CPlayerDB::GetCharSerial()
{
  return this->m_dbChar.m_dwSerial;
}

long double CPlayerDB::GetPvPPoint()
{
  return this->m_dbChar.m_dPvPPoint;
}

void CPlayerDB::SetPvPPoint(long double dPoint)
{
  this->m_dbChar.m_dPvPPoint = dPoint;
}

unsigned int CPlayerDB::GetGuildSerial()
{
  if (this->m_pGuild)
  {
    return this->m_pGuild->m_dwSerial;
  }
  return static_cast<unsigned int>(-1);
}

unsigned int CPlayerDB::GetLevel()
{
  return this->m_dbChar.m_byLevel;
}

unsigned int CPlayerDB::GetHP()
{
  return this->m_dbChar.m_dwHP;
}

void CPlayerDB::SetHP(unsigned int dwHP)
{
  this->m_dbChar.m_dwHP = dwHP;
}

unsigned int CPlayerDB::GetFP()
{
  return this->m_dbChar.m_dwFP;
}

void CPlayerDB::SetFP(unsigned int dwFP)
{
  this->m_dbChar.m_dwFP = dwFP;
}

unsigned int CPlayerDB::GetSP()
{
  return this->m_dbChar.m_dwSP;
}

void CPlayerDB::SetSP(unsigned int dwSP)
{
  this->m_dbChar.m_dwSP = dwSP;
}

unsigned int CPlayerDB::GetDP()
{
  return this->m_dbChar.m_dwDP;
}

void CPlayerDB::SetDP(unsigned int dwDP)
{
  this->m_dbChar.m_dwDP = dwDP;
}

unsigned int CPlayerDB::GetDalant()
{
  return this->m_dbChar.m_dwDalant;
}

void CPlayerDB::SetDalant(unsigned int dwDt)
{
  this->m_dbChar.m_dwDalant = dwDt;
}

unsigned int CPlayerDB::GetGold()
{
  return this->m_dbChar.m_dwGold;
}

void CPlayerDB::SetCurPos(float *fPos)
{
  memcpy_0(this->m_dbChar.m_fStartPos, fPos, sizeof(this->m_dbChar.m_fStartPos));
}

unsigned __int8 CPlayerDB::GetBagNum()
{
  return this->m_dbChar.m_byUseBagNum;
}

unsigned __int16 CPlayerDB::GetNewItemSerial()
{
  return this->m_wSerialCount++;
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

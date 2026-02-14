#include "pch.h"

#include "CPotionParam.h"

unsigned int _ContPotionData::ms_dwCount = 0;

_ContPotionData::_ContPotionData()
{
  m_dwID = ms_dwCount++;
  Init();
}

CPotionParam::CPotionParam()
{
  m_pMaster = nullptr;
}

CPotionParam::~CPotionParam()
{
// this is not a stub
}

void CPotionParam::Init(CPlayer *pMaster)
{
  m_pMaster = pMaster;
  for (int j = 0; j < 2; ++j)
  {
    m_ContCommonPotionData[j].Init();
  }
  m_StoneOfMovePotionData.Init();
}

void CPotionParam::SetPotionActDelay(unsigned __int8 byPotionClass, unsigned int dwCurrTime, unsigned int dwActDelay)
{
  if (byPotionClass < 0x26u)
  {
    m_dwNextUseTime[byPotionClass] = dwActDelay + dwCurrTime;
  }
}

bool CPotionParam::IsUsableActDelay(unsigned __int8 byPotionClass, unsigned int dwCurrTime)
{
  return byPotionClass < 0x26u && m_dwNextUseTime[byPotionClass] <= dwCurrTime;
}

void _ContPotionData::Init()
{
  m_dwPotionEffectIndex = static_cast<unsigned int>(-1);
  m_dwStartSec = static_cast<unsigned int>(-1);
  m_wDurCapSec = 0;
}

void _ContPotionData::Set(unsigned int dwPotionIndex, unsigned int dwStartTime, unsigned __int16 wDurCapSec)
{
  m_dwPotionEffectIndex = dwPotionIndex;
  m_dwStartSec = dwStartTime;
  m_wDurCapSec = wDurCapSec;
}

bool _ContPotionData::IsLive()
{
  return m_dwPotionEffectIndex != static_cast<unsigned int>(-1);
}

unsigned int _ContPotionData::GetEffectIndex()
{
  return m_dwPotionEffectIndex;
}

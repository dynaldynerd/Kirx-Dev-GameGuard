#include "pch.h"

#include "CBattleTournamentInfo.h"

CBattleTournamentInfo::CBattleTournamentInfo()
{
  Init();
}

CBattleTournamentInfo::~CBattleTournamentInfo()
{
  // this is not a stub
}

void CBattleTournamentInfo::Init()
{
  std::memset(this, 0, sizeof(CBattleTournamentInfo));
  for (int j = 0; j < 48; ++j)
  {
    m_WinnerInfo[j].dwSerial = static_cast<unsigned int>(-1);
    m_WinnerInfo[j].byGrade = static_cast<unsigned __int8>(-1);
  }
}

void CBattleTournamentInfo::SetLoad(bool bLoad)
{
  m_bLoad = bLoad;
}

char CBattleTournamentInfo::SetWinnerInfo(unsigned int dwSerial, char *pwszCharName, unsigned __int8 byGrade)
{
  if (m_nCurNum >= 48)
  {
    return 0;
  }

  m_WinnerInfo[m_nCurNum].dwSerial = dwSerial;
  m_WinnerInfo[m_nCurNum].byGrade = byGrade;
  strcpy_s(m_WinnerInfo[m_nCurNum].wszCharName, sizeof(m_WinnerInfo[m_nCurNum].wszCharName), pwszCharName);
  ++m_nCurNum;
  return 1;
}

unsigned __int8 CBattleTournamentInfo::GetWinnerGrade(unsigned int dwSerial, char *pwszCharName)
{
  if (!m_bLoad || m_nCurNum <= 0)
  {
    return static_cast<unsigned __int8>(-1);
  }

  for (int j = 0; j < m_nCurNum; ++j)
  {
    if (m_WinnerInfo[j].dwSerial == dwSerial && std::strcmp(m_WinnerInfo[j].wszCharName, pwszCharName) == 0)
    {
      return m_WinnerInfo[j].byGrade;
    }
  }

  return static_cast<unsigned __int8>(-1);
}

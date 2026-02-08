#include "pch.h"

#include "CPostData.h"

CPostData::CPostData()
{
  Init();
}

void CPostData::Init()
{
  m_nNumber = 0;
  m_byState = static_cast<unsigned __int8>(-1);
  m_byErrCode = 0;
  m_bySendRace = 0;
  m_dwSenderSerial = 0;
  memset_0(m_wszSendName, 0, sizeof(m_wszSendName));
  memset_0(m_wszRecvName, 0, sizeof(m_wszRecvName));
  memset_0(m_wszTitle, 0, sizeof(m_wszTitle));
  memset_0(m_wszContent, 0, sizeof(m_wszContent));
  m_Key.SetRelease();
  m_dwDur = 0;
  m_dwUpt = 0xFFFFFFF;
  m_dwGold = 0;
  m_dwPSSerial = 0;
  m_lnUID = 0;
  m_bContentLoad = false;
  m_bUpdateIndex = false;
}

unsigned __int8 CPostData::GetState()
{
  return m_byState;
}

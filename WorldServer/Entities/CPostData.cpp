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

void CPostData::SetState(unsigned __int8 byState)
{
  m_byState = byState;
}

void CPostData::SetPostContent(char *wszContent)
{
  strncpy(m_wszContent, wszContent, 0xC9u);
  m_wszContent[200] = 0;
  m_bContentLoad = true;
}

void CPostData::SetPostItemSerial(unsigned __int64 lnUID)
{
  m_lnUID = lnUID;
}

void CPostData::SetPostData(
  int nNumber,
  unsigned int dwSenderSerial,
  char *wszSendName,
  char *wszRecvName,
  char *wszTitle,
  _INVENKEY Key,
  unsigned __int64 dwDur,
  unsigned int dwUpt,
  unsigned int dwGold,
  unsigned int dwPSSerial,
  unsigned __int8 bySendRace,
  unsigned __int8 bySenderDgr)
{
  m_nNumber = nNumber;
  m_dwSenderSerial = dwSenderSerial;
  strncpy(m_wszSendName, wszSendName, 0x11u);
  strncpy(m_wszRecvName, wszRecvName, 0x11u);
  strncpy(m_wszTitle, wszTitle, 0x15u);
  m_wszSendName[16] = 0;
  m_wszRecvName[16] = 0;
  m_wszTitle[20] = 0;
  m_Key = Key;
  m_dwDur = dwDur;
  m_dwUpt = dwUpt;
  m_dwGold = dwGold;
  m_dwPSSerial = dwPSSerial;
  m_bySendRace = bySendRace;
  m_bySenderDgr = bySenderDgr;
}

void CPostData::SetPostTitleData(
  int nNumber,
  unsigned int dwPSSerial,
  unsigned __int8 byState,
  char *wszSendName,
  char *wszTitle,
  _INVENKEY Key,
  unsigned __int64 dwDur,
  unsigned int dwUpt,
  unsigned int dwGold,
  bool bUpdateIndex)
{
  m_nNumber = nNumber;
  m_dwPSSerial = dwPSSerial;
  m_byState = byState;
  m_Key = Key;
  m_dwDur = dwDur;
  m_dwUpt = dwUpt;
  m_dwGold = dwGold;
  m_bUpdateIndex = bUpdateIndex;
  strcpy_s(m_wszSendName, 0x11u, wszSendName);
  strcpy_s(m_wszTitle, 0x15u, wszTitle);
}

void CPostData::SetReturnPostData(
  unsigned __int8 byErrCode,
  unsigned int dwPostSerial,
  char *wszRecvName,
  char *wszTitle,
  char *wszContent,
  _INVENKEY Key,
  unsigned __int64 dwDur,
  unsigned int dwUpt,
  unsigned int dwGold)
{
  m_byErrCode = byErrCode;
  m_dwPSSerial = dwPostSerial;
  strcpy_s(m_wszRecvName, 0x11u, wszRecvName);
  strcpy_s(m_wszTitle, 0x15u, wszTitle);
  strcpy_s(m_wszContent, 0xC9u, wszContent);
  m_Key = Key;
  m_dwDur = dwDur;
  m_dwUpt = dwUpt;
  m_dwGold = dwGold;
}

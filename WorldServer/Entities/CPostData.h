#pragma once

#include "IdaCompat.h"

/* 1215 */
class __cppobj __declspec(align(8)) CPostData
{
  int m_nNumber;
  unsigned __int8 m_byState;
  unsigned __int8 m_byErrCode;
  unsigned __int8 m_bySendRace;
  unsigned __int8 m_bySenderDgr;
  unsigned int m_dwSenderSerial;
  char m_wszSendName[17];
  char m_wszRecvName[17];
  char m_wszTitle[21];
  char m_wszContent[201];
  _INVENKEY m_Key;
  unsigned __int64 m_dwDur;
  unsigned int m_dwUpt;
  unsigned __int64 m_lnUID;
  unsigned int m_dwGold;
  unsigned int m_dwPSSerial;
  bool m_bContentLoad;
  bool m_bUpdateIndex;
};


#pragma once

#include "IdaCompat.h"
#include "CMyCriticalSection.h"

/* 1264 */
class __cppobj CMsgData
{
public:
  CMsgData();
  explicit CMsgData(int nObjNum);
  virtual ~CMsgData();

  void Init(int nObjNum);
  char PackingMsg(unsigned int dwMessage, unsigned int dwKey1, unsigned int dwKey2, unsigned int dwKey3);
  _message *PopMsg();
  _message *PopEmptyBuf();
  void PushMsg(_message *pMsg);
  void PushEmptyBuf(_message *pMsg);
  void PumpMsgList();
  virtual void ProcessMessage(_message *pMsg);

  int m_nObjNum;
  int m_nMaxBufNum;
  _message m_gmListHead;
  _message m_gmListTail;
  _message *m_gmBuf;
  _message m_gmListEmptyHead;
  _message m_gmListEmptyTail;
  CMyCriticalSection m_csList;
  CMyCriticalSection m_csEmpty;
};


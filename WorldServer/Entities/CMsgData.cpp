#include "pch.h"

#include "CMsgData.h"

_message::_message()
  : pNext(nullptr),
    pPrev(nullptr)
{
}

_message::~_message()
{
  memset_0(this, 0, sizeof(*this));
}

void _message::SetMsg(unsigned int message, unsigned int key1, unsigned int key2, unsigned int key3)
{
  dwMessage = message;
  dwKey1 = key1;
  dwKey2 = key2;
  dwKey3 = key3;
}

unsigned int _message::GetMessageA() const
{
  return dwMessage;
}

unsigned int _message::GetKey1() const
{
  return dwKey1;
}

CMsgData::CMsgData()
  : m_gmBuf(nullptr)
{
}

CMsgData::CMsgData(int nObjNum)
  : m_gmBuf(nullptr)
{
  Init(nObjNum);
}

CMsgData::~CMsgData()
{
  if (m_gmBuf)
  {
    delete[] m_gmBuf;
    m_gmBuf = nullptr;
  }
}

void CMsgData::Init(int nObjNum)
{
  m_nObjNum = nObjNum;
  m_nMaxBufNum = 100 * nObjNum;
  m_gmBuf = new _message[m_nMaxBufNum];

  m_gmListEmptyHead.pNext = &m_gmListEmptyTail;
  m_gmListEmptyTail.pPrev = &m_gmListEmptyHead;
  m_gmListEmptyHead.pPrev = &m_gmListEmptyHead;
  m_gmListEmptyTail.pNext = &m_gmListEmptyTail;

  for (int j = 0; j < m_nMaxBufNum; ++j)
  {
    m_gmBuf[j].pNext = &m_gmListEmptyTail;
    m_gmBuf[j].pPrev = m_gmListEmptyTail.pPrev;
    m_gmListEmptyTail.pPrev->pNext = &m_gmBuf[j];
    m_gmListEmptyTail.pPrev = &m_gmBuf[j];
  }

  m_gmListHead.pNext = &m_gmListTail;
  m_gmListTail.pPrev = &m_gmListHead;
  m_gmListHead.pPrev = &m_gmListHead;
  m_gmListTail.pNext = &m_gmListTail;
}

char CMsgData::PackingMsg(unsigned int dwMessage, unsigned int dwKey1, unsigned int dwKey2, unsigned int dwKey3)
{
  _message *pMsg = PopEmptyBuf();
  if (!pMsg)
  {
    return 0;
  }

  pMsg->SetMsg(dwMessage, dwKey1, dwKey2, dwKey3);
  PushMsg(pMsg);
  return 1;
}

_message *CMsgData::PopMsg()
{
  m_csList.Lock();
  _message *pNext = m_gmListHead.pNext;
  if (pNext == &m_gmListTail)
  {
    m_csList.Unlock();
    return nullptr;
  }

  m_gmListHead.pNext = pNext->pNext;
  pNext->pNext->pPrev = &m_gmListHead;
  m_csList.Unlock();
  return pNext;
}

_message *CMsgData::PopEmptyBuf()
{
  m_csEmpty.Lock();
  _message *pNext = m_gmListEmptyHead.pNext;
  if (pNext == &m_gmListEmptyTail)
  {
    m_csEmpty.Unlock();
    return nullptr;
  }

  m_gmListEmptyHead.pNext = pNext->pNext;
  pNext->pNext->pPrev = &m_gmListEmptyHead;
  m_csEmpty.Unlock();
  return pNext;
}

void CMsgData::PushMsg(_message *pMsg)
{
  m_csList.Lock();
  pMsg->pNext = &m_gmListTail;
  pMsg->pPrev = m_gmListTail.pPrev;
  m_gmListTail.pPrev->pNext = pMsg;
  m_gmListTail.pPrev = pMsg;
  m_csList.Unlock();
}

void CMsgData::PushEmptyBuf(_message *pMsg)
{
  m_csEmpty.Lock();
  pMsg->pPrev = &m_gmListEmptyHead;
  pMsg->pNext = m_gmListEmptyHead.pNext;
  m_gmListEmptyHead.pNext->pPrev = pMsg;
  m_gmListEmptyHead.pNext = pMsg;
  m_csEmpty.Unlock();
}

void CMsgData::PumpMsgList()
{
  if (!m_gmBuf)
  {
    return;
  }

  m_csList.Lock();
  while (true)
  {
    _message *pMsg = PopMsg();
    if (!pMsg)
    {
      break;
    }

    ProcessMessage(pMsg);
    PushEmptyBuf(pMsg);
  }
  m_csList.Unlock();
}

void CMsgData::ProcessMessage(_message *pMsg)
{
  (void)pMsg;
  // this is not a stub
}

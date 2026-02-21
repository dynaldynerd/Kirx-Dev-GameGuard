#include "pch.h"

#include "CMainThread.h"

unsigned int UsRefObject::ms_uiObjects;

UsRefObject::UsRefObject()
{
  m_uiRefCount = 0;
  ++UsRefObject::ms_uiObjects;
}

UsRefObject::~UsRefObject()
{
  --UsRefObject::ms_uiObjects;
}

void UsRefObject::IncRefCount()
{
  ++m_uiRefCount;
}

void UsRefObject::DecRefCount()
{
  if (--m_uiRefCount == 0)
  {
    delete this;
  }
}

Us_FSM_Node::Us_FSM_Node()
{
  Init();
}

Us_FSM_Node::~Us_FSM_Node() = default;

void Us_FSM_Node::Init()
{
  m_pParent = nullptr;
  m_dwState = 0;
  m_bLive = 0;
  m_dwLoopTime = 500;
  m_dwLastTime = 0;
}

void Us_FSM_Node::SetState(unsigned int dwState)
{
  m_dwState = dwState;
}

unsigned int Us_FSM_Node::GetState()
{
  return m_dwState;
}

void Us_FSM_Node::SetParent(Us_FSM_Node *pParent)
{
  m_pParent = pParent;
}

void Us_FSM_Node::SetLoopTime(unsigned int dwLoopTime)
{
  m_dwLoopTime = dwLoopTime;
}

UsPoint<UsStateTBL>::UsPoint()
  : m_pObject(nullptr)
{
}

UsPoint<UsStateTBL>::UsPoint(UsStateTBL *pObject)
  : m_pObject(pObject)
{
  if (m_pObject)
  {
    m_pObject->IncRefCount();
  }
}

UsPoint<UsStateTBL>::UsPoint(const UsPoint<UsStateTBL> &other)
  : m_pObject(other.m_pObject)
{
  if (m_pObject)
  {
    m_pObject->IncRefCount();
  }
}

UsPoint<UsStateTBL>::~UsPoint()
{
  if (m_pObject)
  {
    m_pObject->DecRefCount();
  }
}

UsPoint<UsStateTBL> &UsPoint<UsStateTBL>::operator=(UsStateTBL *pObject)
{
  if (m_pObject != pObject)
  {
    if (m_pObject)
    {
      m_pObject->DecRefCount();
    }
    m_pObject = pObject;
    if (m_pObject)
    {
      m_pObject->IncRefCount();
    }
  }
  return *this;
}

UsStateTBL *UsPoint<UsStateTBL>::operator->()
{
  return m_pObject;
}

UsPoint<UsStateTBL>::operator UsStateTBL *()
{
  return m_pObject;
}

UsStateTBL::_TBLData::_TBLData()
{
  Init();
}

void UsStateTBL::_TBLData::Init()
{
  byKey = 0;
  pEvnetFun = nullptr;
}

UsStateTBL::_HFSM_Node_Info::_HFSM_Node_Info()
{
  Init();
}

void UsStateTBL::_HFSM_Node_Info::Init()
{
  m_ParentData = -1;
  m_dwStartState = 0;
  m_bUsed = 0;
  m_dwLoopTime = 500;
}

UsStateTBL::UsStateTBL()
{
  m_ppTBL = nullptr;
  m_bAlloc = 0;
  m_byStateSize = 0;
  m_byMessageSize = 0;
  m_pInitFun = nullptr;
  m_pExternFun = nullptr;
  m_pFun = UsStateTBL::OnMsgProc;
  m_IdentityKey = static_cast<unsigned int>(-1);
}

UsStateTBL::~UsStateTBL()
{
  CleanUp();
}

void UsStateTBL::CleanUp()
{
  if (m_bAlloc)
  {
    if (m_ppTBL)
    {
      for (unsigned int i = 0; i < m_byStateSize; ++i)
      {
        if (m_ppTBL[i])
        {
          operator delete[](m_ppTBL[i]);
          m_ppTBL[i] = nullptr;
        }
      }
      operator delete[](m_ppTBL);
      m_ppTBL = nullptr;
    }
    if (m_pNodeInfo)
    {
      operator delete[](m_pNodeInfo);
      m_pNodeInfo = nullptr;
    }
  }
  m_byHFSMSize = 0;
  m_byStateSize = 0;
  m_byMessageSize = 0;
  m_bAlloc = 0;
}

void UsStateTBL::Alloc(unsigned __int8 byHFSMSize, unsigned __int8 byStateSize, unsigned __int8 byMessageSize)
{
  if (m_bAlloc)
  {
    return;
  }

  m_byStateSize = byStateSize;
  m_byMessageSize = byMessageSize;
  m_byHFSMSize = byHFSMSize;

  const unsigned __int64 stateSize = m_byStateSize;
  m_ppTBL = static_cast<_TBLData **>(operator new[](saturated_mul(stateSize, 8uLL)));
  for (unsigned int j = 0; j < m_byStateSize; ++j)
  {
    const unsigned __int64 messageSize = m_byMessageSize;
    _TBLData *table = static_cast<_TBLData *>(operator new[](saturated_mul(messageSize, 0x10uLL)));
    for (unsigned int k = 0; k < m_byMessageSize; ++k)
    {
      table[k].Init();
    }
    m_ppTBL[j] = table;
    memset_0(m_ppTBL[j], 0, 16LL * m_byMessageSize);
  }
  m_bAlloc = 1;

  const unsigned __int64 hfsmSize = m_byHFSMSize;
  m_pNodeInfo = static_cast<_HFSM_Node_Info *>(operator new[](saturated_mul(hfsmSize, 0x10uLL)));
  for (unsigned int j = 0; j < m_byHFSMSize; ++j)
  {
    m_pNodeInfo[j].Init();
  }
}

int UsStateTBL::Add(
  unsigned __int8 byHFSMIndex,
  unsigned __int8 byCurrState,
  unsigned __int8 byEvent_IN,
  unsigned __int8 byNextState_OUT,
  void (__fastcall *pEvnetFun)(Us_HFSM *, unsigned int, void *))
{
  if (!m_bAlloc)
  {
    return 0;
  }
  if (byHFSMIndex >= GetHSFMSize())
  {
    return 0;
  }
  if (byEvent_IN >= m_byMessageSize)
  {
    return 0;
  }
  if (byNextState_OUT >= m_byStateSize)
  {
    return 0;
  }
  if (byCurrState >= m_byStateSize)
  {
    return 0;
  }

  m_ppTBL[byCurrState][byEvent_IN].byKey = byNextState_OUT;
  m_ppTBL[byCurrState][byEvent_IN].pEvnetFun = pEvnetFun;
  return 1;
}

void UsStateTBL::SetHFSMNode(
  int nNodeIndex,
  unsigned int dwStartState,
  unsigned int dwLoopTime,
  int ParentData,
  int bUsed)
{
  m_pNodeInfo[nNodeIndex].m_ParentData = ParentData;
  m_pNodeInfo[nNodeIndex].m_dwStartState = dwStartState;
  m_pNodeInfo[nNodeIndex].m_bUsed = bUsed;
  m_pNodeInfo[nNodeIndex].m_dwLoopTime = dwLoopTime;
}

void UsStateTBL::SetInitFunction(int (__fastcall *pInitFun)(UsStateTBL *, Us_HFSM *))
{
  m_pInitFun = pInitFun;
}

void UsStateTBL::SetExternCallFunction(void (__fastcall *pExternFun)(Us_HFSM *, unsigned int, void *, int))
{
  m_pExternFun = pExternFun;
}

void UsStateTBL::SetCallFunction(void (__fastcall *pFun)(Us_HFSM *, unsigned int, unsigned int, void *))
{
  m_pFun = pFun;
}

unsigned __int8 UsStateTBL::GetHSFMSize()
{
  return m_byHFSMSize;
}

UsStateTBL::_TBLData *UsStateTBL::GetTransState(unsigned __int8 byState, unsigned __int8 byMessage)
{
  if (byState < m_byStateSize && byState && byMessage < m_byMessageSize)
  {
    return &m_ppTBL[byState][byMessage];
  }
  return nullptr;
}

int UsStateTBL::SetHFSM(Us_HFSM *pHFSM, void *pObject)
{
  if (!pHFSM || !pObject)
  {
    return 0;
  }
  if (!pHFSM->SetMyData(this, pObject))
  {
    return 0;
  }
  if (!m_pInitFun || m_pInitFun(this, pHFSM))
  {
    return 1;
  }
  pHFSM->m_bSet = 0;
  return 0;
}

void UsStateTBL::OnMsgProc(Us_HFSM *pHFS, unsigned int dwFSMIndex, unsigned int dwMSG, void *lpParam)
{
  Us_FSM_Node *node = pHFS->GetNode(dwFSMIndex);
  if (!node)
  {
    return;
  }

  unsigned int state = node->GetState();
  UsStateTBL *stateTable = pHFS->m_spShareStateTBLPtr;
  _TBLData *transState = stateTable ? stateTable->GetTransState(state, static_cast<unsigned __int8>(dwMSG)) : nullptr;
  if (transState)
  {
    if (transState->byKey)
    {
      node->SetState(transState->byKey);
    }
    if (transState->pEvnetFun)
    {
      transState->pEvnetFun(pHFS, dwMSG, lpParam);
    }
  }
}

Us_HFSM::Us_HFSM()
{
  m_bSet = 0;
  m_pObject = nullptr;
  m_dwUsedCount = 0;
  for (unsigned int i = 0; i < 10; ++i)
  {
    m_ArNode[i].Init();
  }
  m_spShareStateTBLPtr = static_cast<UsStateTBL *>(nullptr);
  Init();
}

void Us_HFSM::__vtable_anchor() {}

Us_HFSM::~Us_HFSM()
{
  CleanUp();
}

void Us_HFSM::Init()
{
  for (unsigned int i = 0; i < 10; ++i)
  {
    m_ArNode[i].Init();
  }
  m_spShareStateTBLPtr = static_cast<UsStateTBL *>(nullptr);
  m_dwUsedCount = 0;
  m_bSet = 0;
  m_pObject = nullptr;
}

void Us_HFSM::CleanUp()
{
  m_bSet = 0;
  m_dwUsedCount = 0;
  m_spShareStateTBLPtr = static_cast<UsStateTBL *>(nullptr);
}

int Us_HFSM::SetMyData(UsStateTBL *pStateTBL, void *pObject)
{
  Init();
  if (!pStateTBL || !pStateTBL->m_bAlloc || pStateTBL->GetHSFMSize() > 0xAu)
  {
    return 0;
  }

  unsigned int maxUsedNodeIndex = 0;
  bool hasUsedNode = false;
  m_pObject = pObject;
  for (int j = 0; ; ++j)
  {
    if (j >= pStateTBL->GetHSFMSize())
    {
      break;
    }
    if (pStateTBL->m_pNodeInfo[j].m_bUsed)
    {
      m_ArNode[j].m_bLive = 1;
      m_ArNode[j].m_dwState = pStateTBL->m_pNodeInfo[j].m_dwStartState;
      m_ArNode[j].m_dwLoopTime = pStateTBL->m_pNodeInfo[j].m_dwLoopTime;
      if (pStateTBL->m_pNodeInfo[j].m_ParentData >= 0)
      {
        const int parentIndex = pStateTBL->m_pNodeInfo[j].m_ParentData;
        if (parentIndex < pStateTBL->GetHSFMSize())
        {
          m_ArNode[j].m_pParent = &m_ArNode[parentIndex];
        }
      }
      maxUsedNodeIndex = static_cast<unsigned int>(j);
      hasUsedNode = true;
    }
  }

  m_dwUsedCount = hasUsedNode ? (maxUsedNodeIndex + 1) : 0;
  m_spShareStateTBLPtr = pStateTBL;
  m_bSet = 1;
  return 1;
}

Us_FSM_Node *Us_HFSM::GetNode(unsigned int dwIndex)
{
  if (!m_bSet)
  {
    return nullptr;
  }
  if (dwIndex >= 0xA)
  {
    return nullptr;
  }
  if (m_ArNode[dwIndex].m_bLive)
  {
    return &m_ArNode[dwIndex];
  }
  return nullptr;
}

void *Us_HFSM::GetObjectA()
{
  return m_pObject;
}

unsigned int Us_HFSM::GetState(unsigned int dwIndex)
{
  Us_FSM_Node *node = GetNode(dwIndex);
  if (node)
  {
    return node->GetState();
  }
  return 0;
}

unsigned int Us_HFSM::GetIndex(Us_FSM_Node *pNode)
{
  if (pNode)
  {
    for (unsigned int j = 0; j < m_dwUsedCount; ++j)
    {
      if (pNode == &m_ArNode[j] && m_ArNode[j].m_bLive)
      {
        return j;
      }
    }
  }
  return 0xFFFFFFFFu;
}

int Us_HFSM::Link(Us_FSM_Node *pParent, Us_FSM_Node *pChild)
{
  if (!pParent || !pChild)
  {
    return 0;
  }
  pChild->SetParent(pParent);
  return 1;
}

void Us_HFSM::SendMsg(unsigned int dwFSMIndex, unsigned int dwMSG, void *lpParam)
{
  if (this == nullptr)
  {
    return;
  }
  if (!this->m_bSet)
  {
    return;
  }
  if (!dwMSG)
  {
    return;
  }
  if (!static_cast<UsStateTBL *>(this->m_spShareStateTBLPtr))
  {
    return;
  }
  if (!this->m_spShareStateTBLPtr->m_pFun)
  {
    return;
  }

  Us_FSM_Node *node = this->GetNode(dwFSMIndex);
  if (node)
  {
    if (node->m_bLive)
    {
      this->m_spShareStateTBLPtr->m_pFun(this, dwFSMIndex, dwMSG, lpParam);
    }
    if (node->m_pParent)
    {
      unsigned int parentIndex = this->GetIndex(node->m_pParent);
      if (parentIndex != static_cast<unsigned int>(-1))
      {
        this->SendMsg(parentIndex, dwMSG, lpParam);
      }
    }
  }
}

void Us_HFSM::SendExternMsg(unsigned int dwMSG, void *lpParam, unsigned int nParam)
{
  if (this && this->m_bSet && static_cast<UsStateTBL *>(this->m_spShareStateTBLPtr))
  {
    if (this->m_spShareStateTBLPtr->m_pExternFun)
    {
      this->m_spShareStateTBLPtr->m_pExternFun(this, dwMSG, lpParam, nParam);
    }
  }
}

void Us_HFSM::SetLoopTime(unsigned int nIndex, unsigned int dwLoopTime)
{
  Us_FSM_Node *node = GetNode(nIndex);
  if (node)
  {
    node->SetLoopTime(dwLoopTime);
  }
}

void Us_HFSM::AddLoopDelayTime(unsigned int nIndex, unsigned int dwAddDelay)
{
  Us_FSM_Node *node = GetNode(nIndex);
  if (node)
  {
    node->m_dwLastTime += dwAddDelay;
  }
}

void Us_HFSM::OnProcess(unsigned int dwLastTime)
{
  if (m_bSet
    && static_cast<UsStateTBL *>(m_spShareStateTBLPtr)
    && m_spShareStateTBLPtr->m_pFun)
  {
    for (unsigned int j = 0; j < m_dwUsedCount; ++j)
    {
      if (m_ArNode[j].m_bLive && dwLastTime - m_ArNode[j].m_dwLastTime >= m_ArNode[j].m_dwLoopTime)
      {
        unsigned int state = m_ArNode[j].GetState();
        UsStateTBL::_TBLData *transState = m_spShareStateTBLPtr->GetTransState(static_cast<unsigned __int8>(state), 0);
        if (transState && transState->pEvnetFun)
        {
          transState->pEvnetFun(this, 0, nullptr);
        }
        m_ArNode[j].m_dwLastTime = dwLastTime;
      }
    }
  }
}

void SF_Timer::Set(unsigned int dwTimeDelay)
{
  m_dwGapCheckTime = dwTimeDelay;
  m_dwLastCheckTime = 0;
}

bool SF_Timer::CheckTime(unsigned int dwLoopTime)
{
  if (m_dwGapCheckTime > dwLoopTime - m_dwLastCheckTime)
  {
    return false;
  }
  m_dwLastCheckTime = dwLoopTime;
  return true;
}

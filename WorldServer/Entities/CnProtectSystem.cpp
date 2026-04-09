#include "pch.h"

#include "CnProtectSystem.h"

#include <new>

#include "CAsyncLogger.h"
#include "CUserDB.h"
#include "GlobalObjects.h"
#include "NPROTECT_PARAM_VER_30.h"
#include "WorldServerUtil.h"

CnProtectSystem::CnProtectSystem()
  : m_ppNodeArray(nullptr), m_bInit(false), m_bActive(false), m_tmLoopTime(), m_dwCurrentCheckIndex(0)
{
}

CnProtectSystem::~CnProtectSystem()
{
  if (m_ppNodeArray)
  {
    for (unsigned int index = 0; index < MAX_PLAYER; ++index)
    {
      delete m_ppNodeArray[index];
      m_ppNodeArray[index] = nullptr;
    }
    delete[] m_ppNodeArray;
    m_ppNodeArray = nullptr;
  }

  if (m_bInit)
  {
    CloseCSAuth3();
  }
}

BASE_NPROTECT_PARAM *CnProtectSystem::GetParam(unsigned int n)
{
  if (!m_bInit || !m_bActive)
  {
    return nullptr;
  }
  if (n >= MAX_PLAYER)
  {
    return nullptr;
  }
  return m_ppNodeArray[n];
}

template <>
char CnProtectSystem::Init<NPROTECT_PARAM_VER_30>(bool bActive, bool bFirstChecker)
{
  m_bActive = false;
  m_bInit = false;
  NPROTECT_PARAM_VER_30::ms_bFirstChecker = false;
  if (!bActive)
  {
    return 1;
  }

  m_bActive = bActive;
  NPROTECT_PARAM_VER_30::ms_bFirstChecker = bFirstChecker;

  const unsigned int initResult = InitCSAuth3("./nProtect/");
  if (initResult)
  {
    MyMessageBox("CnProtectSystem::Init()", "InitCSAuth3(%s) Ret(%u) Failed!", "./nProtect/", initResult);
    g_Main.m_logLoadingError.Write(
      "CnProtectSystem::Init() : InitCSAuth3(%s) Ret(%u) Failed!",
      "./nProtect/",
      initResult);
    return 0;
  }

  m_ppNodeArray = new (std::nothrow) BASE_NPROTECT_PARAM *[MAX_PLAYER];
  if (!m_ppNodeArray)
  {
    return 0;
  }

  for (unsigned int index = 0; index < MAX_PLAYER; ++index)
  {
    NPROTECT_PARAM_VER_30 *param = new (std::nothrow) NPROTECT_PARAM_VER_30();
    if (!param)
    {
      return 0;
    }
    m_ppNodeArray[index] = param;
    param->Init();
  }

  m_tmLoopTime.BeginTimer(50);
  m_bInit = true;
  return 1;
}

bool CnProtectSystem::IsActive()
{
  return m_bActive;
}

bool CnProtectSystem::IsInit()
{
  return m_bInit;
}

bool CnProtectSystem::OnCheckSession_FirstVerify(int n)
{
  BASE_NPROTECT_PARAM *pParam = GetParam(static_cast<unsigned int>(n));
  return pParam && pParam->OnCheckSession_FirstVerify(n);
}

void CnProtectSystem::OnConnectSession(int n)
{
  BASE_NPROTECT_PARAM *pParam = GetParam(static_cast<unsigned int>(n));
  if (pParam)
  {
    pParam->OnConnect(n);
  }
}

void CnProtectSystem::OnDisConnectSession(int n)
{
  BASE_NPROTECT_PARAM *pParam = GetParam(static_cast<unsigned int>(n));
  if (pParam)
  {
    pParam->OnDisConnect();
  }
}

void CnProtectSystem::OnLoop()
{
  if (m_bInit && m_bActive && m_tmLoopTime.CountingTimer())
  {
    unsigned int loopCount = 0;
    while (loopCount < 10)
    {
      if (m_dwCurrentCheckIndex >= MAX_PLAYER)
      {
        m_dwCurrentCheckIndex = 0;
      }

      CUserDB *pUserDB = &g_UserDB[m_dwCurrentCheckIndex];
      if (pUserDB->m_bActive)
      {
        OnLoopSession(static_cast<int>(m_dwCurrentCheckIndex));
      }

      ++loopCount;
      ++m_dwCurrentCheckIndex;
    }
  }
}

void CnProtectSystem::OnLoopSession(int n)
{
  BASE_NPROTECT_PARAM *pParam = GetParam(static_cast<unsigned int>(n));
  if (pParam)
  {
    CUserDB *pUserDB = &g_UserDB[n];
    if (pUserDB->m_bActive && pParam->IsLogPass())
    {
      pParam->OnLoop();
    }
  }
}

bool CnProtectSystem::RecvClientLine(int n, _MSG_HEADER *pMsgHeader, char *pMsg)
{
  if (!m_bInit || !m_bActive)
  {
    return false;
  }

  BASE_NPROTECT_PARAM *pParam = GetParam(static_cast<unsigned int>(n));
  return pParam
    && pParam->OnRecvSession(
      this,
      n,
      pMsgHeader->m_byType[1],
      static_cast<unsigned __int64>(pMsgHeader->m_wSize - 4),
      pMsg);
}

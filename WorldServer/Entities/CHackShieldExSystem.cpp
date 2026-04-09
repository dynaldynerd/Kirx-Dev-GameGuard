#include "pch.h"

#include "CHackShieldExSystem.h"

#include <new>

#include "CUserDB.h"
#include "GlobalObjects.h"
#include "HACKSHEILD_PARAM_ANTICP.h"
#include "WorldServerUtil.h"

CHackShieldExSystem::CHackShieldExSystem()
  : m_ppNodeArray(nullptr), m_bInit(false), m_bActive(false), m_tmLoopTime(), m_dwCurrentCheckIndex(0)
{
}

CHackShieldExSystem::~CHackShieldExSystem()
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
    Release<HACKSHEILD_PARAM_ANTICPX_5381>();
  }
}

template <>
char CHackShieldExSystem::Init<HACKSHEILD_PARAM_ANTICPX_5381>(bool bActive)
{
  m_bActive = false;
  m_bInit = false;
  if (!bActive)
  {
    return 1;
  }

  m_bActive = bActive;
  HACKSHEILD_PARAM_ANTICPX_5381::ms_hServer = AhnHS_CreateServerObject(".\\RF.hsb");
  if (!HACKSHEILD_PARAM_ANTICPX_5381::ms_hServer)
  {
    MyMessageBox("CHackShieldExSystem::Init()", "_AhnHS_CreateServerObject(%s) Failed!", ".\\RF.hsb");
    g_Main.m_logLoadingError.Write(
      "CHackShieldMgr::Init() : _AhnHS_CreateServerObject(%s) Failed!",
      ".\\RF.hsb");
    return 0;
  }

  m_ppNodeArray = new (std::nothrow) BASE_HACKSHEILD_PARAM *[MAX_PLAYER];
  if (!m_ppNodeArray)
  {
    return 0;
  }

  for (unsigned int index = 0; index < MAX_PLAYER; ++index)
  {
    HACKSHEILD_PARAM_ANTICPX_5381 *param = new (std::nothrow) HACKSHEILD_PARAM_ANTICPX_5381();
    if (!param)
    {
      return 0;
    }
    m_ppNodeArray[index] = param;
    param->Init();
  }

  m_bInit = true;
  m_tmLoopTime.BeginTimer(50);
  return 1;
}

template <>
void CHackShieldExSystem::Release<HACKSHEILD_PARAM_ANTICPX_5381>()
{
  AhnHS_CloseServerHandle(HACKSHEILD_PARAM_ANTICPX_5381::ms_hServer);
  HACKSHEILD_PARAM_ANTICPX_5381::ms_hServer = 0;
}

BASE_HACKSHEILD_PARAM *CHackShieldExSystem::GetParam(unsigned int n)
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

bool CHackShieldExSystem::IsActive()
{
  return m_bActive;
}

bool CHackShieldExSystem::IsInit()
{
  return m_bInit;
}

bool CHackShieldExSystem::OnCheckSession_FirstVerify(int n)
{
  BASE_HACKSHEILD_PARAM *param = GetParam(static_cast<unsigned int>(n));
  return param && param->OnCheckSession_FirstVerify(n);
}

void CHackShieldExSystem::OnConnectSession(int n)
{
  BASE_HACKSHEILD_PARAM *param = GetParam(static_cast<unsigned int>(n));
  if (param)
  {
    param->OnConnect(n);
  }
}

void CHackShieldExSystem::OnDisConnectSession(int n)
{
  BASE_HACKSHEILD_PARAM *param = GetParam(static_cast<unsigned int>(n));
  if (param)
  {
    param->OnDisConnect();
  }
}

void CHackShieldExSystem::OnLoop()
{
  if (m_tmLoopTime.CountingTimer())
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

void CHackShieldExSystem::OnLoopSession(int n)
{
  BASE_HACKSHEILD_PARAM *param = GetParam(static_cast<unsigned int>(n));
  if (param && param->IsLogPass())
  {
    CUserDB *pUserDB = &g_UserDB[n];
    if (pUserDB->m_bActive)
    {
      param->OnLoop();
    }
  }
}

bool CHackShieldExSystem::RecvClientLine(int n, _MSG_HEADER *pMsgHeader, char *pMsg)
{
  if (!m_bInit || !m_bActive)
  {
    return false;
  }

  BASE_HACKSHEILD_PARAM *param = GetParam(static_cast<unsigned int>(n));
  return param && param->OnRecvSession(
                    this,
                    n,
                    pMsgHeader->m_byType[1],
                    static_cast<unsigned __int64>(pMsgHeader->m_wSize - 4),
                    pMsg);
}

#include "pch.h"

#include "CHackShieldExSystem.h"

#include <new>

#include "CNetProcess.h"
#include "CUserDB.h"
#include "GlobalObjects.h"
#include "HACKSHEILD_PARAM_ANTICP.h"
#include "WorldServerUtil.h"

namespace
{
using AntiCpInitializeFn = int(__cdecl *)(const char *crcPath);

AntiCpInitializeFn ResolveAntiCpInitialize()
{
  const char *kCandidateModules[] = {"AntiCpSvr.dll", "EhSvc.dll", "HackShield.dll"};
  for (const char *moduleName : kCandidateModules)
  {
    HMODULE module = GetModuleHandleA(moduleName);
    if (!module)
    {
      module = LoadLibraryA(moduleName);
    }
    if (!module)
    {
      continue;
    }

    FARPROC proc = GetProcAddress(module, "_AntiCpSvr_Initialize");
    if (!proc)
    {
      proc = GetProcAddress(module, "AntiCpSvr_Initialize");
    }
    if (proc)
    {
      return reinterpret_cast<AntiCpInitializeFn>(proc);
    }
  }

  return nullptr;
}
} // namespace

CHackShieldExSystem::CHackShieldExSystem()
  : m_ppNodeArray(nullptr), m_bInit(false), m_bActive(false), m_dwCurrentCheckIndex(0), m_tmLoopTime()
{
}

CHackShieldExSystem::~CHackShieldExSystem()
{
  if (!m_ppNodeArray)
  {
    return;
  }

  for (unsigned int j = 0; j < MAX_PLAYER; ++j)
  {
    delete m_ppNodeArray[j];
    m_ppNodeArray[j] = nullptr;
  }
  delete[] m_ppNodeArray;
  m_ppNodeArray = nullptr;
}

HACKSHEILD_PARAM_ANTICP *CHackShieldExSystem::GetParam(unsigned int n)
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

char CHackShieldExSystem::Init_struct(bool bActive)
{
  m_bActive = false;
  m_bInit = false;
  if (!bActive)
  {
    return 1;
  }

  m_bActive = bActive;
  m_ppNodeArray = new (std::nothrow) HACKSHEILD_PARAM_ANTICP *[MAX_PLAYER];
  if (!m_ppNodeArray)
  {
    return 0;
  }

  for (unsigned int j = 0; j < MAX_PLAYER; ++j)
  {
    m_ppNodeArray[j] = new (std::nothrow) HACKSHEILD_PARAM_ANTICP();
    if (!m_ppNodeArray[j])
    {
      return 0;
    }
    m_ppNodeArray[j]->Init();
  }

  AntiCpInitializeFn initializeFn = ResolveAntiCpInitialize();
  const int initResult = initializeFn ? initializeFn(".\\HackShield.crc") : 1;
  if (initResult)
  {
    MyMessageBox(
      "CHackShieldExSystem::Init()",
      "_AntiCpSvr_Initialize(%s) Ret(%u) Failed!",
      ".\\HackShield.crc",
      initResult);
    g_Main.m_logLoadingError.Write(
      "CHackShieldMgr::Init() : _AntiCpSvr_Initialize(%s) Ret(%u) Failed!",
      ".\\HackShield.crc",
      initResult);
    return 0;
  }

  m_bInit = true;
  m_dwCurrentCheckIndex = 0;
  m_tmLoopTime.BeginTimer(50);
  return 1;
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
  HACKSHEILD_PARAM_ANTICP *param = GetParam(static_cast<unsigned int>(n));
  return param && param->OnCheckSession_FirstVerify(n);
}

void CHackShieldExSystem::OnConnectSession(int n)
{
  HACKSHEILD_PARAM_ANTICP *param = GetParam(static_cast<unsigned int>(n));
  if (param)
  {
    param->OnConnect(n);
  }
}

void CHackShieldExSystem::OnDisConnectSession(int n)
{
  HACKSHEILD_PARAM_ANTICP *param = GetParam(static_cast<unsigned int>(n));
  if (param)
  {
    param->OnDisConnect();
  }
}

void CHackShieldExSystem::OnLoop()
{
  if (!m_tmLoopTime.CountingTimer())
  {
    return;
  }

  unsigned int loopCount = 0;
  while (loopCount < 10)
  {
    if (m_dwCurrentCheckIndex >= MAX_PLAYER)
    {
      m_dwCurrentCheckIndex = 0;
    }

    CUserDB *user = &g_UserDB[m_dwCurrentCheckIndex];
    if (user->m_bActive)
    {
      OnLoopSession(static_cast<int>(m_dwCurrentCheckIndex));
    }

    ++loopCount;
    ++m_dwCurrentCheckIndex;
  }
}

void CHackShieldExSystem::OnLoopSession(int n)
{
  HACKSHEILD_PARAM_ANTICP *param = GetParam(static_cast<unsigned int>(n));
  if (!param || !param->IsLogPass())
  {
    return;
  }

  CUserDB *user = &g_UserDB[n];
  if (user->m_bActive)
  {
    param->OnLoop();
  }
}

bool CHackShieldExSystem::RecvClientLine(int n, _MSG_HEADER *pMsgHeader, char *pMsg)
{
  if (!m_bInit || !m_bActive)
  {
    return false;
  }

  HACKSHEILD_PARAM_ANTICP *param = GetParam(static_cast<unsigned int>(n));
  return param && param->OnRecvSession(
                    this,
                    n,
                    pMsgHeader->m_byType[1],
                    static_cast<unsigned __int64>(pMsgHeader->m_wSize - 4),
                    pMsg);
}

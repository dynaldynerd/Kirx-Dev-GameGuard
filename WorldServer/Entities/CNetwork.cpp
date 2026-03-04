#include "pch.h"

#include "CNetwork.h"
#include "CNetProcess.h"

#include <cstdio>

CNetwork::CNetwork()
{
}

CNetwork::~CNetwork()
{
  // this is not a stub
}

bool CNetwork::LoadDll(char *dll_name)
{
  m_hChinaDLL = LoadLibraryA(dll_name);
  if (!m_hChinaDLL)
  {
    return false;
  }

  SetDataAnalysisFunc = reinterpret_cast<void(__fastcall *)(bool(__fastcall *)(unsigned int, unsigned int, _MSG_HEADER *, char *))>(
    GetProcAddress(m_hChinaDLL, "SetDataAnalysisFunc"));
  if (!SetDataAnalysisFunc)
  {
    return false;
  }

  Release = reinterpret_cast<void(__fastcall *)()>(GetProcAddress(m_hChinaDLL, "Release"));
  if (!Release)
  {
    return false;
  }

  LoadSendMsg = reinterpret_cast<int(__fastcall *)(unsigned int, unsigned int, unsigned __int8 *, unsigned int, char *, unsigned __int16)>(
    GetProcAddress(m_hChinaDLL, "LoadSendMsg"));
  if (!LoadSendMsg)
  {
    return false;
  }

  SetNetSystem = reinterpret_cast<bool(__fastcall *)(unsigned int, _NET_TYPE_PARAM *, char *, char *)>(
    GetProcAddress(m_hChinaDLL, "SetNetSystem"));
  if (!SetNetSystem)
  {
    return false;
  }

  OnLoop = reinterpret_cast<void(__fastcall *)()>(GetProcAddress(m_hChinaDLL, "OnLoop"));
  if (!OnLoop)
  {
    return false;
  }

  Connect = reinterpret_cast<int(__fastcall *)(unsigned int, unsigned int, unsigned int, unsigned __int16)>(
    GetProcAddress(m_hChinaDLL, "Connect"));
  if (!Connect)
  {
    return false;
  }

  CloseSocket = reinterpret_cast<void(__fastcall *)(unsigned int, unsigned int, bool)>(
    GetProcAddress(m_hChinaDLL, "CloseSocket"));
  if (!CloseSocket)
  {
    return false;
  }

  GetSocket = reinterpret_cast<_socket *(__fastcall *)(unsigned int, unsigned int)>(
    GetProcAddress(m_hChinaDLL, "GetSocket"));
  if (!GetSocket)
  {
    return false;
  }

  RecvS = reinterpret_cast<bool(__fastcall *)(char *, int, int *)>(GetProcAddress(m_hChinaDLL, "RecvS"));
  if (!RecvS)
  {
    return false;
  }

  SendS = reinterpret_cast<bool(__fastcall *)(char *, int, int *)>(GetProcAddress(m_hChinaDLL, "SendS"));
  return SendS != nullptr;
}

void CNetwork::InitNetwork()
{
  _NET_TYPE_PARAM type;
  type.m_bRealSockCheck = 0;
  type.m_bSystemLogFile = 1;
  type.m_bServer = 0;
  type.m_byRecvThreadNum = 1;
  type.m_byRecvSleepTime = 1;
  type.m_wSocketMaxNum = 1;
  type.m_bSvrToS = 1;
  type.m_bOddMsgWriteLog = 1;
  type.m_dwSendBufferSize = 1000000;
  type.m_dwRecvBufferSize = 1000000;
  type.m_bAnSyncConnect = 1;
  sprintf_s(type.m_szModuleName, sizeof(type.m_szModuleName), "%s", "ChiNetwork");
  SetNetSystem(1, &type, const_cast<char *>("ChiNetwork"), const_cast<char *>("..\\ZoneServerLog\\NetLog"));
}

void CNetwork::FreeDLL()
{
  if (m_hChinaDLL)
  {
    FreeLibrary(m_hChinaDLL);
  }
}


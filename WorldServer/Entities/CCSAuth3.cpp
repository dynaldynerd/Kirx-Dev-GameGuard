#include "pch.h"

#include "CCSAuth3.h"

#include <cstring>

namespace
{
using GlobalFunctionProc = unsigned int(__cdecl *)(void *);
using ObjectFunctionProc = unsigned int(__cdecl *)(void *);

HMODULE g_hProtocol = nullptr;
GlobalFunctionProc pfnGlobalFunction = nullptr;
ObjectFunctionProc pfnObjectFunction = nullptr;
void *CS3LogCallbackBackup = nullptr;
void *CS3UpdateInfoCallbackBackup = nullptr;
} // namespace

CCSAuth3::CCSAuth3()
  : uMagic(0x12345678)
{
  std::memset(data, 0, sizeof(data));
}

CCSAuth3::~CCSAuth3()
{
  if (pfnObjectFunction)
  {
    Close();
  }
}

unsigned int CCSAuth3::Init(int bFirstChecker)
{
  if (!pfnObjectFunction)
  {
    return 3203;
  }

  std::memset(packet, 0, sizeof(packet));

  unsigned __int64 param[6]{};
  param[0] = 0;
  param[1] = reinterpret_cast<unsigned __int64>(packet);
  param[2] = sizeof(packet);
  param[3] = reinterpret_cast<unsigned __int64>(data);
  param[4] = sizeof(data);
  param[5] = bFirstChecker != 0;
  return pfnObjectFunction(param);
}

unsigned int CCSAuth3::Close()
{
  if (!pfnObjectFunction)
  {
    return 3203;
  }

  unsigned __int64 param[5]{};
  param[0] = 1;
  param[1] = reinterpret_cast<unsigned __int64>(packet);
  param[2] = sizeof(packet);
  param[3] = reinterpret_cast<unsigned __int64>(data);
  param[4] = sizeof(data);
  return pfnObjectFunction(param);
}

unsigned int CCSAuth3::Get(unsigned int *puReturnedPacketSize)
{
  if (!pfnObjectFunction)
  {
    return 3203;
  }

  unsigned __int64 param[6]{};
  param[0] = 2;
  param[1] = reinterpret_cast<unsigned __int64>(packet);
  param[2] = sizeof(packet);
  param[3] = reinterpret_cast<unsigned __int64>(data);
  param[4] = sizeof(data);
  param[5] = reinterpret_cast<unsigned __int64>(puReturnedPacketSize);
  return pfnObjectFunction(param);
}

unsigned int CCSAuth3::Check(unsigned int uReturnedPacketSize)
{
  if (!pfnObjectFunction)
  {
    return 3203;
  }

  unsigned __int64 param[6]{};
  param[0] = 3;
  param[1] = reinterpret_cast<unsigned __int64>(packet);
  param[2] = sizeof(packet);
  param[3] = reinterpret_cast<unsigned __int64>(data);
  param[4] = sizeof(data);
  param[5] = uReturnedPacketSize;
  return pfnObjectFunction(param);
}

unsigned int InitCSAuth3(const char *pszPath)
{
  char modulePath[260]{};
  if (std::strlen(pszPath) + 15 > sizeof(modulePath))
  {
    return 3200;
  }

  sprintf_s(modulePath, "%s.dll", "ggauth64");
  g_hProtocol = LoadLibraryA(modulePath);
  if (!g_hProtocol)
  {
    return 3201;
  }

  pfnGlobalFunction = reinterpret_cast<GlobalFunctionProc>(GetProcAddress(g_hProtocol, "GlobalFunction"));
  pfnObjectFunction = reinterpret_cast<ObjectFunctionProc>(GetProcAddress(g_hProtocol, "ObjectFunction"));
  if (!pfnGlobalFunction || !pfnObjectFunction)
  {
    FreeLibrary(g_hProtocol);
    g_hProtocol = nullptr;
    pfnGlobalFunction = nullptr;
    pfnObjectFunction = nullptr;
    return 3201;
  }

  unsigned __int64 param[4]{};
  param[0] = 0;
  param[1] = reinterpret_cast<unsigned __int64>(pszPath);
  param[2] = reinterpret_cast<unsigned __int64>(CS3LogCallbackBackup);
  param[3] = reinterpret_cast<unsigned __int64>(CS3UpdateInfoCallbackBackup);
  return pfnGlobalFunction(param);
}

unsigned int CloseCSAuth3()
{
  if (!pfnGlobalFunction)
  {
    return 3202;
  }

  int op = 2;
  pfnGlobalFunction(&op);
  if (g_hProtocol)
  {
    FreeLibrary(g_hProtocol);
    g_hProtocol = nullptr;
  }
  pfnGlobalFunction = nullptr;
  pfnObjectFunction = nullptr;
  return 0;
}

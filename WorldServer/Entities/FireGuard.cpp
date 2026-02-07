#include "pch.h"

#include "FireGuard.h"

#include <new>

#include "CNetworkEX.h"

namespace
{
  struct FireGuardFns
  {
    int (__fastcall *Initialize)(int (__fastcall *)(int, void *, void *, int, void *), unsigned __int8 *, unsigned int) = nullptr;
    void (__fastcall *Uninitialize)() = nullptr;
    int (__fastcall *EncryptPacket)(void *, unsigned __int8 *, int) = nullptr;
    int (__fastcall *DecryptPacket)(void *, unsigned __int8 *, int) = nullptr;
    unsigned int (__fastcall *GetLastError)() = nullptr;
    void *(__fastcall *CreateUserContext)(
      unsigned int,
      unsigned __int8 *,
      unsigned int,
      unsigned __int8 *,
      unsigned int,
      unsigned int,
      void *) = nullptr;
    void (__fastcall *CloseUserContext)(void **) = nullptr;
  };

  HMODULE g_fgModule = nullptr;
  FireGuardFns g_fgFns{};
  unsigned int g_fgLastError = 0;

  template <typename T>
  T LoadProc(const char *name)
  {
    if (!g_fgModule)
    {
      return nullptr;
    }

    FARPROC proc = GetProcAddress(g_fgModule, name);
    if (!proc && name[0] != '_')
    {
      char decorated[128]{};
      sprintf_s(decorated, "_%s", name);
      proc = GetProcAddress(g_fgModule, decorated);
    }

    return reinterpret_cast<T>(proc);
  }

  void EnsureFireGuardLoaded()
  {
    if (g_fgModule)
    {
      return;
    }

    g_fgModule = LoadLibraryA("fgrs.dll");
    if (!g_fgModule)
    {
      g_fgLastError = GetLastError();
      return;
    }

    g_fgFns.Initialize = LoadProc<decltype(g_fgFns.Initialize)>("CcrFG_rs_Initialize");
    g_fgFns.Uninitialize = LoadProc<decltype(g_fgFns.Uninitialize)>("CcrFG_rs_Uninitialize");
    g_fgFns.EncryptPacket = LoadProc<decltype(g_fgFns.EncryptPacket)>("CcrFG_rs_EncryptPacket");
    g_fgFns.DecryptPacket = LoadProc<decltype(g_fgFns.DecryptPacket)>("CcrFG_rs_DecryptPacket");
    g_fgFns.GetLastError = LoadProc<decltype(g_fgFns.GetLastError)>("CcrFG_rs_GetLastError");
    g_fgFns.CreateUserContext = LoadProc<decltype(g_fgFns.CreateUserContext)>("CcrFG_rs_CreateUserContext");
    g_fgFns.CloseUserContext = LoadProc<decltype(g_fgFns.CloseUserContext)>("CcrFG_rs_CloseUserContext");

    if (!g_fgFns.Initialize || !g_fgFns.Uninitialize || !g_fgFns.EncryptPacket || !g_fgFns.DecryptPacket
        || !g_fgFns.GetLastError || !g_fgFns.CreateUserContext || !g_fgFns.CloseUserContext)
    {
      g_fgLastError = ERROR_PROC_NOT_FOUND;
    }
  }
}

__int64 __fastcall AddEnvVariable(const char *lpName, char *lpValue, int nValueLen)
{
  unsigned int result = 0;
  char *source = lpValue;
  void *block = nullptr;
  DWORD size = 0;
  DWORD existingSize = 0;

  if (lpName && *lpName && lpValue && *lpValue && nValueLen)
  {
    while (true)
    {
      size += 1024;
      if (block)
      {
        free(block);
      }
      block = malloc(static_cast<int>(size));
      memset_0(block, 0, static_cast<int>(size));

      existingSize = GetEnvironmentVariableA(lpName, static_cast<LPSTR>(block), size);
      if (!existingSize)
      {
        break;
      }

      if (size >= static_cast<DWORD>(nValueLen + existingSize))
      {
        if (strcmp_0(lpName, "path") && strcmp_0(lpName, "PATHEXT"))
        {
          strcat_s(static_cast<char *>(block), static_cast<int>(size), source);
        }
        else
        {
          strcat_s(static_cast<char *>(block), static_cast<int>(size), ";");
          strcat_s(static_cast<char *>(block), static_cast<int>(size), source);
        }

        result = SetEnvironmentVariableA(lpName, static_cast<LPCSTR>(block));
        break;
      }
    }
  }

  if (block)
  {
    free(block);
  }

  return result;
}

int __fastcall CcrFgCallback(
  int lCallbackCode,
  void *hUserContext,
  void *pCallbackParameter,
  int nParameterSize,
  void *pReservedParameter)
{
  (void)lCallbackCode;
  (void)nParameterSize;

  const unsigned __int16 socketIndex = *static_cast<unsigned __int16 *>(pReservedParameter);
  _socket *socket = g_FGSendData.pNetwork->GetSocket(0, socketIndex);
  if (socket)
  {
    if (socket->m_hFGContext == hUserContext)
    {
      g_FGSendData.pNetwork->ExpulsionSocket(
        g_FGSendData.pTargetProc->m_nIndex,
        socketIndex,
        5,
        pCallbackParameter);
    }
    else
    {
      g_FgLogFile.Write(
        "CallBack Warning > CcrFgCallback() : Miss Match Context, User Context >> SocketContext(%#x) != ParamContext(%#x)",
        socket->m_hFGContext,
        hUserContext);
    }
    return 1;
  }

  g_FgLogFile.Write("CallBack Error > CcrFgCallback() : Invalid Socket Index(%d)", socketIndex);
  return 1;
}

extern "C" int __fastcall _CcrFG_rs_Initialize(
  int (__fastcall *pfunc)(int, void *, void *, int, void *),
  unsigned __int8 *pszWorldName,
  unsigned int nWorldNameLen)
{
  EnsureFireGuardLoaded();
  if (!g_fgFns.Initialize)
  {
    g_fgLastError = ERROR_PROC_NOT_FOUND;
    return 0;
  }

  return g_fgFns.Initialize(pfunc, pszWorldName, nWorldNameLen);
}

extern "C" void __fastcall _CcrFG_rs_Uninitialize()
{
  EnsureFireGuardLoaded();
  if (g_fgFns.Uninitialize)
  {
    g_fgFns.Uninitialize();
  }
}

extern "C" int __fastcall _CcrFG_rs_EncryptPacket(void *context, unsigned __int8 *buffer, int size)
{
  EnsureFireGuardLoaded();
  if (!g_fgFns.EncryptPacket)
  {
    g_fgLastError = ERROR_PROC_NOT_FOUND;
    return 0;
  }

  return g_fgFns.EncryptPacket(context, buffer, size);
}

extern "C" int __fastcall _CcrFG_rs_DecryptPacket(void *context, unsigned __int8 *buffer, int size)
{
  EnsureFireGuardLoaded();
  if (!g_fgFns.DecryptPacket)
  {
    g_fgLastError = ERROR_PROC_NOT_FOUND;
    return 0;
  }

  return g_fgFns.DecryptPacket(context, buffer, size);
}

extern "C" unsigned int __fastcall _CcrFG_rs_GetLastError()
{
  if (g_fgFns.GetLastError)
  {
    return g_fgFns.GetLastError();
  }

  return g_fgLastError;
}

extern "C" void *__fastcall _CcrFG_rs_CreateUserContext(
  unsigned int dwAccountSerial,
  unsigned __int8 *pszAccount,
  unsigned int nAccountLen,
  unsigned __int8 *pszCharName,
  unsigned int nCharNameLen,
  unsigned int dwIP,
  void *pWorldId)
{
  EnsureFireGuardLoaded();
  if (!g_fgFns.CreateUserContext)
  {
    g_fgLastError = ERROR_PROC_NOT_FOUND;
    return nullptr;
  }

  return g_fgFns.CreateUserContext(
    dwAccountSerial,
    pszAccount,
    nAccountLen,
    pszCharName,
    nCharNameLen,
    dwIP,
    pWorldId);
}

extern "C" void __fastcall _CcrFG_rs_CloseUserContext(void **pContext)
{
  EnsureFireGuardLoaded();
  if (g_fgFns.CloseUserContext)
  {
    g_fgFns.CloseUserContext(pContext);
  }
}

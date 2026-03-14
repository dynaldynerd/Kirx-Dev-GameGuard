#include "pch.h"

#include "WheatyExceptionReport.h"

#include <TlHelp32.h>
#include <cstdarg>
#include <cstdio>

#pragma comment(lib, "Dbghelp.lib")

namespace
{
char szBuffer[512]{};
char verbuf[256]{};
char namebuf[256]{};
int nCount = 0;
const char *dayofweek[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

size_t RemainingSize(const char *base, const char *current, size_t total)
{
  if (!base || !current || total == 0)
  {
    return 0;
  }
  const ptrdiff_t used = current - base;
  if (used < 0)
  {
    return 0;
  }
  const size_t usedSize = static_cast<size_t>(used);
  if (usedSize >= total)
  {
    return 0;
  }
  return total - usedSize;
}

int AppendFormat(char *&cursor, const char *base, size_t total, const char *format, ...)
{
  const size_t remaining = RemainingSize(base, cursor, total);
  if (remaining == 0)
  {
    return 0;
  }
  va_list args;
  va_start(args, format);
  const int written = vsprintf_s(cursor, remaining, format, args);
  va_end(args);
  if (written > 0)
  {
    cursor += written;
  }
  return written;
}
} // namespace

int WheatyExceptionReport::m_bRunDialog = 1;
char WheatyExceptionReport::m_szFtpPwd[255]{};
char WheatyExceptionReport::m_szLogFileName[260]{};
int WheatyExceptionReport::m_bPDBExist = 0;
HANDLE WheatyExceptionReport::m_hReportFile = nullptr;
char WheatyExceptionReport::m_szFtpIp[20]{};
unsigned int WheatyExceptionReport::m_nFtpPort = 0;
LPTOP_LEVEL_EXCEPTION_FILTER WheatyExceptionReport::m_previousFilter = nullptr;
char WheatyExceptionReport::m_szDescription[10240]{};
char WheatyExceptionReport::m_szFtpId[255]{};
HANDLE WheatyExceptionReport::m_hProcess = nullptr;
char WheatyExceptionReport::m_szLogName[2048]{};
SYSTEMTIME WheatyExceptionReport::m_st{};
char WheatyExceptionReport::m_szFtpDirectory[260]{};

WheatyExceptionReport g_WheatyExceptionReport;

WheatyExceptionReport::WheatyExceptionReport()
{
  m_previousFilter = SetUnhandledExceptionFilter(WheatyExceptionReport::WheatyUnhandledExceptionFilter);
  m_hProcess = GetCurrentProcess();
  m_szLogName[0] = 0;
  m_szDescription[0] = 0;
  m_szFtpIp[0] = 0;
  m_szFtpId[0] = 0;
  m_szFtpPwd[0] = 0;
  m_szFtpDirectory[0] = 0;
  m_bPDBExist = 0;
}

WheatyExceptionReport::~WheatyExceptionReport()
{
  SetUnhandledExceptionFilter(m_previousFilter);
}

void WheatyExceptionReport::SetRunDialog(int bRun)
{
  m_bRunDialog = bRun;
}

void WheatyExceptionReport::SetLogName(const char *pszLogName)
{
  std::strcpy(m_szLogName, pszLogName);
}

void WheatyExceptionReport::SetDescription(const char *pszDescription)
{
  std::strcpy(m_szDescription, pszDescription);
}

void WheatyExceptionReport::SetFtpConnection(
  const char *pszFtpIp,
  unsigned int nFtpPort,
  const char *pszFtpId,
  const char *pszFtpPwd,
  const char *pszFtpDirectory)
{
  std::strcpy(m_szFtpIp, pszFtpIp);
  m_nFtpPort = nFtpPort;
  std::strcpy(m_szFtpId, pszFtpId);
  std::strcpy(m_szFtpPwd, pszFtpPwd);
  if (pszFtpDirectory)
  {
    std::strcpy(m_szFtpDirectory, pszFtpDirectory);
  }
}

int WheatyExceptionReport::printf(const char *format, ...)
{
  char buffer[5124]{};
  DWORD bytesWritten = 0;
  va_list args;
  va_start(args, format);
  const int writeSize = vsprintf(buffer, format, args);
  va_end(args);
  WriteFile(m_hReportFile, buffer, writeSize, &bytesWritten, nullptr);
  return writeSize;
}

bool WheatyExceptionReport::GetDisplayInfo(int nDeviceIndex, char *lpszDeviceInfo, char *lpszMonitorInfo)
{
  (void)nDeviceIndex;
  (void)lpszDeviceInfo;
  (void)lpszMonitorInfo;
  // this is not a stub
  return false;
}

BasicType WheatyExceptionReport::GetBasicType(ULONG typeIndex, DWORD64 modBase)
{
  ULONG basicType = 0;
  ULONG typeId = 0;
  if (SymGetTypeInfo(m_hProcess, modBase, typeIndex, TI_GET_BASETYPE, &basicType))
  {
    return static_cast<BasicType>(basicType);
  }

  if (SymGetTypeInfo(m_hProcess, modBase, typeIndex, TI_GET_TYPEID, &typeId)
    && SymGetTypeInfo(m_hProcess, modBase, typeId, TI_GET_BASETYPE, &basicType))
  {
    return static_cast<BasicType>(basicType);
  }

  return btNoType;
}

char *WheatyExceptionReport::FormatOutputValue(
  char *pszCurrBuffer,
  const char *pszBaseBuffer,
  size_t cbBuffer,
  BasicType basicType,
  unsigned __int64 length,
  const void *pAddress)
{
  switch (length)
  {
    case 1:
      AppendFormat(pszCurrBuffer, pszBaseBuffer, cbBuffer, " = %X", *reinterpret_cast<const unsigned __int8 *>(pAddress));
      break;
    case 2:
      AppendFormat(pszCurrBuffer, pszBaseBuffer, cbBuffer, " = %X", *reinterpret_cast<const unsigned __int16 *>(pAddress));
      break;
    case 4:
      if (basicType == btFloat)
      {
        AppendFormat(pszCurrBuffer, pszBaseBuffer, cbBuffer, " = %f", *reinterpret_cast<const float *>(pAddress));
      }
      else if (basicType == btChar)
      {
        const unsigned int pointerValue = *reinterpret_cast<const unsigned int *>(pAddress);
        const char *textPointer = reinterpret_cast<const char *>(static_cast<uintptr_t>(pointerValue));
        if (IsBadStringPtrA(textPointer, 32))
        {
          AppendFormat(pszCurrBuffer, pszBaseBuffer, cbBuffer, " = %X", pointerValue);
        }
        else
        {
          AppendFormat(pszCurrBuffer, pszBaseBuffer, cbBuffer, " = \"%.31s\"", textPointer);
        }
      }
      else
      {
        AppendFormat(pszCurrBuffer, pszBaseBuffer, cbBuffer, " = %X", *reinterpret_cast<const unsigned int *>(pAddress));
      }
      break;
    case 8:
      if (basicType == btFloat)
      {
        AppendFormat(pszCurrBuffer, pszBaseBuffer, cbBuffer, " = %lf", *reinterpret_cast<const double *>(pAddress));
      }
      else
      {
        AppendFormat(
          pszCurrBuffer,
          pszBaseBuffer,
          cbBuffer,
          " = %I64X",
          *reinterpret_cast<const unsigned __int64 *>(pAddress));
      }
      break;
    default:
      break;
  }

  return pszCurrBuffer;
}

char WheatyExceptionReport::FormatSymbolValue(
  SYMBOL_INFO *pSym,
  STACKFRAME64 *sf,
  char *pszBuffer,
  unsigned int cbBuffer)
{
  char *buffer = pszBuffer;
  if ((pSym->Flags & 0x40) != 0)
  {
    AppendFormat(buffer, pszBuffer, cbBuffer, "Parameter ");
  }
  else if ((pSym->Flags & 0x80) != 0)
  {
    AppendFormat(buffer, pszBuffer, cbBuffer, "Local ");
  }

  if (pSym->Tag == 5)
  {
    return 0;
  }

  AppendFormat(buffer, pszBuffer, cbBuffer, "'%s'", pSym->Name);
  void *address = nullptr;
  if ((pSym->Flags & 0x10) != 0)
  {
    address = reinterpret_cast<void *>(sf->AddrFrame.Offset + pSym->Address);
  }
  else
  {
    if ((pSym->Flags & 8) != 0)
    {
      return 0;
    }
    address = reinterpret_cast<void *>(pSym->Address);
  }

  bool handled = false;
  buffer = DumpTypeIndex(
    buffer,
    pszBuffer,
    cbBuffer,
    pSym->ModBase,
    pSym->TypeIndex,
    0,
    reinterpret_cast<unsigned __int64>(address),
    &handled);
  if (!handled)
  {
    const BasicType basicType = GetBasicType(pSym->TypeIndex, pSym->ModBase);
    buffer = FormatOutputValue(buffer, pszBuffer, cbBuffer, basicType, pSym->Size, address);
  }

  return 1;
}

BOOL CALLBACK WheatyExceptionReport::EnumerateSymbolsCallback(
  SYMBOL_INFO *pSymInfo,
  ULONG SymbolSize,
  void *UserContext)
{
  (void)SymbolSize;
  char buffer[10256]{};
  if (FormatSymbolValue(pSymInfo, reinterpret_cast<STACKFRAME64 *>(UserContext), buffer, 10240))
  {
    WheatyExceptionReport::printf("\t%s\r\n", buffer);
  }
  return TRUE;
}

char *WheatyExceptionReport::DumpTypeIndex(
  char *pszCurrBuffer,
  const char *pszBaseBuffer,
  size_t cbBuffer,
  DWORD64 modBase,
  ULONG dwTypeIndex,
  unsigned int nestingLevel,
  unsigned __int64 offset,
  bool *bHandled)
{
  struct FINDCHILDREN : TI_FINDCHILDREN_PARAMS
  {
    ULONG MoreChildIds[1024];
    FINDCHILDREN()
    {
      Count = 1024;
      Start = 0;
    }
  };

  *bHandled = false;

  PWSTR symbolName = nullptr;
  if (SymGetTypeInfo(m_hProcess, modBase, dwTypeIndex, TI_GET_SYMNAME, &symbolName))
  {
    AppendFormat(pszCurrBuffer, pszBaseBuffer, cbBuffer, " %ls", symbolName);
    LocalFree(symbolName);
  }

  ULONG childCount = 0;
  SymGetTypeInfo(m_hProcess, modBase, dwTypeIndex, TI_GET_CHILDRENCOUNT, &childCount);
  if (!childCount)
  {
    return pszCurrBuffer;
  }

  FINDCHILDREN findChildren;
  findChildren.Count = childCount;
  findChildren.Start = 0;
  if (!SymGetTypeInfo(m_hProcess, modBase, dwTypeIndex, TI_FINDCHILDREN, &findChildren))
  {
    return pszCurrBuffer;
  }

  AppendFormat(pszCurrBuffer, pszBaseBuffer, cbBuffer, "\r\n");
  char *buffer = pszCurrBuffer;
  for (ULONG childIndex = 0; childIndex < childCount; ++childIndex)
  {
    for (ULONG tabIndex = 0; tabIndex <= nestingLevel + 1; ++tabIndex)
    {
      AppendFormat(buffer, pszBaseBuffer, cbBuffer, "\t");
    }

    bool childHandled = false;
    buffer = DumpTypeIndex(
      buffer,
      pszBaseBuffer,
      cbBuffer,
      modBase,
      findChildren.ChildId[childIndex],
      nestingLevel + 1,
      offset,
      &childHandled);
    if (!childHandled)
    {
      ULONG childOffset = 0;
      ULONG childTypeId = 0;
      ULONG64 length = 0;
      SymGetTypeInfo(m_hProcess, modBase, findChildren.ChildId[childIndex], TI_GET_OFFSET, &childOffset);
      SymGetTypeInfo(m_hProcess, modBase, findChildren.ChildId[childIndex], TI_GET_TYPEID, &childTypeId);
      SymGetTypeInfo(m_hProcess, modBase, childTypeId, TI_GET_LENGTH, &length);

      const void *address = reinterpret_cast<void *>(offset + childOffset);
      const BasicType basicType = GetBasicType(findChildren.ChildId[childIndex], modBase);
      buffer = FormatOutputValue(buffer, pszBaseBuffer, cbBuffer, basicType, length, address);
      AppendFormat(buffer, pszBaseBuffer, cbBuffer, "\r\n");
    }
  }

  *bHandled = true;
  return buffer;
}

void WheatyExceptionReport::WriteStackDetails(CONTEXT *pContext, bool bWriteVariables)
{
  DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
  STACKFRAME64 stackFrame{};
  stackFrame.AddrPC.Offset = pContext->Rip;
  stackFrame.AddrPC.Mode = AddrModeFlat;
  stackFrame.AddrStack.Offset = pContext->Rsp;
  stackFrame.AddrStack.Mode = AddrModeFlat;
  stackFrame.AddrFrame.Offset = pContext->Rbp;
  stackFrame.AddrFrame.Mode = AddrModeFlat;

  WheatyExceptionReport::printf("Address   Frame     Function            SourceFile\r\n");

  for (int frameIndex = 0; frameIndex < 30; ++frameIndex)
  {
    HANDLE currentThread = GetCurrentThread();
    if (!StackWalk64(
          machineType,
          m_hProcess,
          currentThread,
          &stackFrame,
          pContext,
          nullptr,
          reinterpret_cast<PFUNCTION_TABLE_ACCESS_ROUTINE64>(SymFunctionTableAccess64),
          reinterpret_cast<PGET_MODULE_BASE_ROUTINE64>(SymGetModuleBase64),
          nullptr)
      || !stackFrame.AddrFrame.Offset)
    {
      break;
    }

    WheatyExceptionReport::printf("%08X  %08X  ", static_cast<DWORD>(stackFrame.AddrPC.Offset), static_cast<DWORD>(stackFrame.AddrFrame.Offset));

    unsigned __int8 symbolBuffer[sizeof(SYMBOL_INFO) + 1024]{};
    SYMBOL_INFO *symbol = reinterpret_cast<SYMBOL_INFO *>(symbolBuffer);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = 1024;

    DWORD64 displacement = 0;
    if (SymFromAddr(m_hProcess, stackFrame.AddrPC.Offset, &displacement, symbol))
    {
      WheatyExceptionReport::printf("%hs+%I64X", symbol->Name, displacement);
    }
    else
    {
      char szModule[292]{};
      unsigned int section = 0;
      unsigned int offset = 0;
      WheatyExceptionReport::GetLogicalAddress(reinterpret_cast<void *>(stackFrame.AddrPC.Offset), szModule, 260, &section, &offset);
      WheatyExceptionReport::printf("<2>%04X:%08X %s", section, offset, szModule);
    }

    IMAGEHLP_LINE64 line{};
    line.SizeOfStruct = sizeof(line);
    DWORD lineDisplacement = 0;
    if (SymGetLineFromAddr64(m_hProcess, stackFrame.AddrPC.Offset, &lineDisplacement, &line))
    {
      WheatyExceptionReport::printf("  %s line %u", line.FileName, line.LineNumber);
    }

    WheatyExceptionReport::printf("\r\n");
    if (bWriteVariables)
    {
      IMAGEHLP_STACK_FRAME imageStackFrame{};
      imageStackFrame.InstructionOffset = stackFrame.AddrPC.Offset;
      SymSetContext(m_hProcess, &imageStackFrame, nullptr);
      SymEnumSymbols(m_hProcess, 0, 0, WheatyExceptionReport::EnumerateSymbolsCallback, &stackFrame);
      WheatyExceptionReport::printf("\r\n");
    }
  }
}

void WheatyExceptionReport::GenerateExceptionReport(EXCEPTION_POINTERS *pExceptionInfo)
{
  EXCEPTION_RECORD *exceptionRecord = pExceptionInfo->ExceptionRecord;
  char filename[276]{};
  DWORD size = 255;
  char userName[288]{};
  char computerName[288]{};
  char szModule[292]{};
  unsigned int section = 0;
  unsigned int offset = 0;

  WheatyExceptionReport::printf("==================================================================================\r\n");
  if (std::strlen(m_szDescription))
  {
    WheatyExceptionReport::printf("%s\r\n\r\n", m_szDescription);
  }

  GetModuleFileNameA(nullptr, filename, 255);
  WheatyExceptionReport::printf("Exe :         %s\r\n", filename);
  WheatyExceptionReport::printf("OS :          %s\r\n", WheatyExceptionReport::GetOsVersion());
  GetUserNameA(userName, &size);
  WheatyExceptionReport::printf("UserName :    %s\r\n", userName);
  GetComputerNameA(computerName, &size);
  WheatyExceptionReport::printf("Computer :    %s\r\n", computerName);
  WheatyExceptionReport::printf("Time :        %d-%02d-%02d %s", m_st.wYear, m_st.wMonth, m_st.wDay, dayofweek[m_st.wDayOfWeek]);

  const char *amPm = (m_st.wHour < 12) ? "AM" : "PM";
  const int hour = (m_st.wHour < 13) ? m_st.wHour : (m_st.wHour - 12);
  WheatyExceptionReport::printf(", %02d:%02d:%02d.%03d %s\r\n", hour, m_st.wMinute, m_st.wSecond, m_st.wMilliseconds, amPm);
  WheatyExceptionReport::printf("Code :        %08X %s\r\n", exceptionRecord->ExceptionCode, WheatyExceptionReport::GetExceptionString(exceptionRecord->ExceptionCode));

  if (WheatyExceptionReport::GetLogicalAddress(exceptionRecord->ExceptionAddress, szModule, 260, &section, &offset))
  {
    WheatyExceptionReport::printf(
      "Address :     %08X %02X:%08X %s\r\n",
      static_cast<DWORD>(reinterpret_cast<uintptr_t>(exceptionRecord->ExceptionAddress)),
      section,
      offset,
      szModule);
  }
  else
  {
    WheatyExceptionReport::printf("Address :     %08X\r\n", static_cast<DWORD>(reinterpret_cast<uintptr_t>(exceptionRecord->ExceptionAddress)));
  }

  WheatyExceptionReport::printf("==================================================================================\r\n");

  SymSetOptions(4u);
  HANDLE currentProcess = GetCurrentProcess();
  if (SymInitialize(currentProcess, nullptr, TRUE))
  {
    CONTEXT context = *pExceptionInfo->ContextRecord;
    WheatyExceptionReport::printf("\r\n\r\n");
    WheatyExceptionReport::printf("------------------------------------------------------------------------\r\n");
    WheatyExceptionReport::printf("    Call stack\r\n");
    WheatyExceptionReport::printf("------------------------------------------------------------------------\r\n");
    WheatyExceptionReport::WriteStackDetails(&context, false);

    if (m_bPDBExist)
    {
      WheatyExceptionReport::printf("\r\n\r\n");
      WheatyExceptionReport::printf("------------------------------------------------------------------------\r\n");
      WheatyExceptionReport::printf("    Local Variables And Parameters\r\n");
      WheatyExceptionReport::printf("------------------------------------------------------------------------\r\n");
      context = *pExceptionInfo->ContextRecord;
      WheatyExceptionReport::WriteStackDetails(&context, true);
      WheatyExceptionReport::printf("\r\n\r\n");
      WheatyExceptionReport::printf("------------------------------------------------------------------------\r\n");
      WheatyExceptionReport::printf("    Global Variables\r\n");
      WheatyExceptionReport::printf("------------------------------------------------------------------------\r\n");
      const ULONG64 baseOfDll = reinterpret_cast<ULONG64>(GetModuleHandleA(szModule));
      SymEnumSymbols(GetCurrentProcess(), baseOfDll, nullptr, WheatyExceptionReport::EnumerateSymbolsCallback, nullptr);
    }

    SymCleanup(GetCurrentProcess());

    WheatyExceptionReport::printf("\r\n\r\n");
    WheatyExceptionReport::printf("------------------------------------------------------------------------\r\n");
    WheatyExceptionReport::printf("    Load Modules\r\n");
    WheatyExceptionReport::printf("------------------------------------------------------------------------\r\n");

    MODULEENTRY32 moduleEntry{};
    moduleEntry.dwSize = sizeof(moduleEntry);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (snapshot == INVALID_HANDLE_VALUE)
    {
      WheatyExceptionReport::printf("Failed getting snapshot for this process!\r\n");
    }
    else
    {
      if (Module32First(snapshot, &moduleEntry))
      {
        do
        {
          WheatyExceptionReport::printf("%08X  %s\r\n", static_cast<DWORD>(reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr)), moduleEntry.szExePath);
        } while (Module32Next(snapshot, &moduleEntry));
      }
      else
      {
        WheatyExceptionReport::printf("Failed getting loaded modules for this process!\r\n");
      }
      CloseHandle(snapshot);
    }

    SYSTEM_INFO systemInfo{};
    GetSystemInfo(&systemInfo);
    char szDeviceInfo[288]{};
    char szMonitorInfo[280]{};
    WheatyExceptionReport::printf("\r\n\r\n\r\n");
    WheatyExceptionReport::printf("------------------------------------------------------------------------\r\n");
    WheatyExceptionReport::printf("    Hardware/Driver Information\r\n");
    WheatyExceptionReport::printf("------------------------------------------------------------------------\r\n");
    WheatyExceptionReport::printf("Processor:              0x%X\r\n", systemInfo.wProcessorArchitecture);
    if (WheatyExceptionReport::GetDisplayInfo(0, szDeviceInfo, szMonitorInfo))
    {
      WheatyExceptionReport::printf("Display :               %s, %s\r\n", szDeviceInfo, szMonitorInfo);
    }
    else
    {
      WheatyExceptionReport::printf("Display :               Unknown\r\n");
    }
    WheatyExceptionReport::printf("Page Size:              %u\r\n", systemInfo.dwPageSize);
    WheatyExceptionReport::printf("Min App Address:        0x%08X\r\n", static_cast<DWORD>(reinterpret_cast<uintptr_t>(systemInfo.lpMinimumApplicationAddress)));
    WheatyExceptionReport::printf("Max App Address:        0x%08X\r\n", static_cast<DWORD>(reinterpret_cast<uintptr_t>(systemInfo.lpMaximumApplicationAddress)));
    WheatyExceptionReport::printf("Processor Mask:         0x%X\r\n", static_cast<DWORD>(systemInfo.dwActiveProcessorMask));
    WheatyExceptionReport::printf("Number of Processors:   %u\r\n", systemInfo.dwNumberOfProcessors);
    WheatyExceptionReport::printf("Processor Type:         %u\r\n", systemInfo.dwProcessorType);
    WheatyExceptionReport::printf("Allocation Granularity: %u\r\n", systemInfo.dwAllocationGranularity);
    WheatyExceptionReport::printf("Processor Level:        %u\r\n", systemInfo.wProcessorLevel);
    WheatyExceptionReport::printf("Processor Revision:     %u\r\n", systemInfo.wProcessorRevision);
    WheatyExceptionReport::printf("\r\n");

    MEMORYSTATUS memoryStatus{};
    GlobalMemoryStatus(&memoryStatus);
    GlobalMemoryStatus(&memoryStatus);
    WheatyExceptionReport::printf("Percent memory used:    %u\r\n", memoryStatus.dwMemoryLoad);
    WheatyExceptionReport::printf("Physical memory:        %u/%u\r\n", static_cast<DWORD>(memoryStatus.dwAvailPhys), static_cast<DWORD>(memoryStatus.dwTotalPhys));
    WheatyExceptionReport::printf("Page file:              %u/%u\r\n", static_cast<DWORD>(memoryStatus.dwAvailPageFile), static_cast<DWORD>(memoryStatus.dwTotalPageFile));
    WheatyExceptionReport::printf("Virtual memory:         %u/%u\r\n", static_cast<DWORD>(memoryStatus.dwAvailVirtual), static_cast<DWORD>(memoryStatus.dwTotalVirtual));
    WheatyExceptionReport::printf("\r\n");
  }
}

const char *WheatyExceptionReport::GetExceptionString(DWORD dwCode)
{
  DWORD localCode = dwCode;
  if (dwCode > 3221225477)
  {
    localCode += 1073741818;
    switch (localCode)
    {
      case 0:
        return "IN_PAGE_ERROR";
      case 2:
        return "INVALID_HANDLE";
      case 23:
        return "ILLEGAL_INSTRUCTION";
      case 31:
        return "NONCONTINUABLE_EXCEPTION";
      case 32:
        return "INVALID_DISPOSITION";
      case 134:
        return "ARRAY_BOUNDS_EXCEEDED";
      case 135:
        return "FLT_DENORMAL_OPERAND";
      case 136:
        return "FLT_DIVIDE_BY_ZERO";
      case 137:
        return "FLT_INEXACT_RESULT";
      case 138:
        return "FLT_INVALID_OPERATION";
      case 139:
        return "FLT_OVERFLOW";
      case 140:
        return "FLT_STACK_CHECK";
      case 141:
        return "FLT_UNDERFLOW";
      case 142:
        return "INT_DIVIDE_BY_ZERO";
      case 143:
        return "INT_OVERFLOW";
      case 144:
        return "PRIV_INSTRUCTION";
      case 247:
        return "STACK_OVERFLOW";
      default:
        break;
    }
  }
  else
  {
    switch (localCode)
    {
      case 3221225477:
        return "ACCESS_VIOLATION";
      case 2147483649:
        return "GUARD_PAGE";
      case 2147483650:
        return "DATATYPE_MISALIGNMENT";
      case 2147483651:
        return "BREAKPOINT";
      case 2147483652:
        return "SINGLE_STEP";
      default:
        break;
    }
  }

  HMODULE moduleHandle = GetModuleHandleA("NTDLL.DLL");
  FormatMessageA(2560, moduleHandle, dwCode, 0, szBuffer, 512, nullptr);
  return szBuffer;
}

const char *WheatyExceptionReport::GetOsName(
  unsigned int dwPlatformId,
  unsigned int dwMajorVersion,
  unsigned int dwMinorVersion)
{
  namebuf[0] = 0;
  if (dwPlatformId == 1)
  {
    if (dwMinorVersion)
    {
      if (dwMinorVersion == 10)
      {
        std::strcpy(namebuf, "98");
      }
      else if (dwMinorVersion == 90)
      {
        std::strcpy(namebuf, "ME");
      }
    }
    else
    {
      std::strcpy(namebuf, "95");
    }
  }
  else if (dwMajorVersion == 3)
  {
    std::strcpy(namebuf, "Windows NT 3.51");
  }
  else if (dwPlatformId == 2)
  {
    if (dwMajorVersion == 4)
    {
      std::strcpy(namebuf, "NT 4.0");
    }
    else if (dwMinorVersion)
    {
      if (dwMinorVersion == 1)
      {
        std::strcpy(namebuf, "XP");
      }
      else if (dwMinorVersion == 3)
      {
        std::strcpy(namebuf, "Server 2003 family");
      }
    }
    else
    {
      std::strcpy(namebuf, "2000");
    }
  }
  return namebuf;
}

const char *WheatyExceptionReport::GetOsVersion()
{
  OSVERSIONINFOA versionInfo{};
  versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
#pragma warning(push)
#pragma warning(disable : 4996)
  const BOOL gotVersion = GetVersionExA(&versionInfo);
#pragma warning(pop)
  if (gotVersion)
  {
    if (std::strlen(versionInfo.szCSDVersion) > 200)
    {
      versionInfo.szCSDVersion[100] = 0;
    }

    const char *osName = GetOsName(versionInfo.dwPlatformId, versionInfo.dwMajorVersion, versionInfo.dwMinorVersion);
    sprintf_s(
      verbuf,
      "Windows %d.%d(%s), build%d PlatformId %d \"%s\"",
      versionInfo.dwMajorVersion,
      versionInfo.dwMinorVersion,
      osName,
      versionInfo.dwBuildNumber,
      versionInfo.dwPlatformId,
      versionInfo.szCSDVersion);
  }
  else
  {
    std::strcpy(verbuf, "WINDOWS UNKNOWN");
  }
  return verbuf;
}

bool WheatyExceptionReport::GetLogicalAddress(
  void *addr,
  char *szModule,
  DWORD len,
  unsigned int *section,
  unsigned int *offset)
{
  MEMORY_BASIC_INFORMATION mbi{};
  if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
  {
    return false;
  }

  HMODULE hModule = static_cast<HMODULE>(mbi.AllocationBase);
  if (!GetModuleFileNameA(hModule, szModule, len))
  {
    return false;
  }

  const auto *dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER *>(hModule);
  const auto *ntHeaders =
    reinterpret_cast<const IMAGE_NT_HEADERS *>(reinterpret_cast<const unsigned __int8 *>(hModule) + dosHeader->e_lfanew);
  const IMAGE_SECTION_HEADER *sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
  const unsigned int rva = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(addr) - reinterpret_cast<uintptr_t>(hModule));
  for (unsigned int index = 0; index < ntHeaders->FileHeader.NumberOfSections; ++index, ++sectionHeader)
  {
    const unsigned int sectionStart = sectionHeader->VirtualAddress;
    const unsigned int sectionSize =
      (sectionHeader->SizeOfRawData <= sectionHeader->Misc.VirtualSize)
        ? sectionHeader->Misc.VirtualSize
        : sectionHeader->SizeOfRawData;
    const unsigned int sectionEnd = sectionStart + sectionSize;
    if (rva >= sectionStart && rva <= sectionEnd)
    {
      *section = index + 1;
      *offset = rva - sectionStart;
      return true;
    }
  }

  return false;
}

long __stdcall WheatyExceptionReport::WheatyUnhandledExceptionFilter(EXCEPTION_POINTERS *pExceptionInfo)
{
  ++nCount;
  GetLocalTime(&m_st);

  char filename[292]{};
  char drive[44]{};
  char dir[288]{};
  char fileName[288]{};
  char pdbPath[280]{};
  char szModule[292]{};
  unsigned int section = 0;
  unsigned int offset = 0;
  GetModuleFileNameA(nullptr, filename, 260);
  _splitpath(filename, drive, dir, fileName, nullptr);
  sprintf_s(pdbPath, "%s%s%s.pdb", drive, dir, fileName);
  HANDLE pdbFile = CreateFileA(pdbPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (pdbFile != INVALID_HANDLE_VALUE)
  {
    m_bPDBExist = 1;
    CloseHandle(pdbFile);
  }

  GetLogicalAddress(pExceptionInfo->ExceptionRecord->ExceptionAddress, szModule, 260, &section, &offset);

  sprintf_s(
    m_szLogFileName,
    "%s%s..\\%d_Exception_%s_%d_%d_%d_%d_%d.txt",
    drive,
    dir,
    nCount,
    m_szLogName,
    m_st.wMonth,
    m_st.wDay,
    m_st.wHour,
    m_st.wMinute,
    m_st.wSecond);

  m_hReportFile = CreateFileA(m_szLogFileName, GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, 0x80000000, nullptr);
  if (m_hReportFile)
  {
    SetFilePointer(m_hReportFile, 0, nullptr, FILE_END);
    GenerateExceptionReport(pExceptionInfo);
    CloseHandle(m_hReportFile);
    m_hReportFile = nullptr;
  }

  return EXCEPTION_EXECUTE_HANDLER;
}


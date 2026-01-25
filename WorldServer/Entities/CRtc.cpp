#include "pch.h"

#include "CRtc.h"
#include "KorLocalTime.h"

#include <cstdarg>
#include <cwchar>
#include <cstdio>

#include <atomic>

extern "C" {
typedef int(__cdecl *RTC_error_fnW)(int, const wchar_t *, int, const wchar_t *, const wchar_t *, ...);
RTC_error_fnW __cdecl RTC_SetErrorFuncW(RTC_error_fnW);
}

CRtc *CRtc::ms_Instance = nullptr;

namespace
{
    LONG g_rtcRunning = 0;
    RTC_error_fnW g_rtcErrorFunc = nullptr;
}

extern "C" RTC_error_fnW __cdecl RTC_SetErrorFuncW(RTC_error_fnW func)
{
    RTC_error_fnW previous = g_rtcErrorFunc;
    g_rtcErrorFunc = func;
    return previous;
}

CRtc::CRtc()
{
}

CRtc::~CRtc()
{
}

CRtc *CRtc::GetIntance()
{
    if ( !ms_Instance )
    {
        ms_Instance = new CRtc();
    }
    return ms_Instance;
}

void CRtc::Reg_Fn()
{
    RTC_SetErrorFuncW(RTC_Fn);
}

int CRtc::RTC_Fn(int errType, const wchar_t *file, int line, const wchar_t *module, const wchar_t *format, ...)
{
    while ( _InterlockedExchange(&g_rtcRunning, 1) )
    {
        Sleep(0);
    }

    va_list args;
    va_start(args, format);
    va_list argsCopy;
    va_copy(argsCopy, args);
    unsigned int userSpecifiedType = va_arg(args, unsigned int);

    wchar_t buffer[0x400];
    vswprintf_s(buffer, sizeof(buffer) / sizeof(buffer[0]), format, argsCopy);
    va_end(argsCopy);
    va_end(args);

    const wchar_t *fileName = file ? file : L"<unknown file>";
    const wchar_t *moduleName = module ? module : L"<unknown module>";

    wchar_t formatted[0x400];
    swprintf_s(
        formatted,
        sizeof(formatted) / sizeof(formatted[0]),
        L"RTC Failure #%u (user specified type %u) occurred:\n\nModule:\t%s\nFile:\t%s\nLine:\t%d\n\nFull Message:\n%s",
        userSpecifiedType,
        errType,
        moduleName,
        fileName,
        line,
        buffer);

    char filePath[256];
    sprintf_s(filePath, "..\\ZoneServerLog\\SystemLog\\RTC_Reporter_%u.txt", GetKorLocalTime());

    FILE *stream = nullptr;
    if ( fopen_s(&stream, filePath, "wt") != 0 || !stream )
    {
        _InterlockedExchange(&g_rtcRunning, 0);
        return 0;
    }

    fwprintf(stream, L"%s", formatted);
    fclose(stream);

    _InterlockedExchange(&g_rtcRunning, 0);
    return 1;
}

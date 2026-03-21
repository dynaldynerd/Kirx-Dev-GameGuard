#include "pch.h"

#include "RuntimeAssert.h"

#include <assert.h>
#include <intrin.h>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

namespace RuntimeAssert
{
namespace
{
void WriteAssertMessage(const wchar_t *expression, const wchar_t *file, unsigned int line)
{
  wchar_t message[1024]{};
  swprintf_s(
    message,
    L"Assertion failed: %ls\r\nFile: %ls\r\nLine: %u\r\n",
    expression ? expression : L"(null)",
    file ? file : L"(null)",
    line);
  OutputDebugStringW(message);
}
}

bool ShouldBreak(const wchar_t *expression, const wchar_t *file, unsigned int line)
{
#ifdef _DEBUG
  return _CrtDbgReportW(_CRT_ASSERT, file, static_cast<int>(line), nullptr, L"%ls", expression) == 1;
#else
  WriteAssertMessage(expression, file, line);
  return true;
#endif
}

void Fail(const wchar_t *expression, const wchar_t *file, unsigned int line)
{
#ifdef _DEBUG
  _wassert(expression, file, line);
#else
  if (ShouldBreak(expression, file, line))
  {
    __debugbreak();
  }
#endif
}
}

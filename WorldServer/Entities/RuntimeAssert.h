#pragma once

namespace RuntimeAssert
{
[[nodiscard]] bool ShouldBreak(const wchar_t *expression, const wchar_t *file, unsigned int line);
void Fail(const wchar_t *expression, const wchar_t *file, unsigned int line);
}

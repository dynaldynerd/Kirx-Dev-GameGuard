#pragma once

#include <windows.h>

const char *GetClientNationFolder(void);
const char *GetClientLanguageFontName(void);
DWORD GetClientLanguageFontCharSet(void);

BOOL GetNationDataFileName(const char *pi_pFileName,
                           char *po_pFileName,
                           size_t pi_nFileNameSize);

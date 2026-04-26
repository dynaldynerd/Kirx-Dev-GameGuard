#pragma once

#include <windows.h>

const char *GetClientNationFolder(void);

BOOL GetNationDataFileName(const char *pi_pFileName,
                           char *po_pFileName,
                           size_t pi_nFileNameSize);

#pragma once

#include <windows.h>

BOOL _LoadAbuseFilter(char *pi_pAbuseDataFilename);
BOOL _LoadAdvFilter(char *pi_pAdvDataFilename);

BOOL _IsAbuseStr(char *pi_pStr);
BOOL _IsAdvStr(char *pi_pStr);

BOOL _IsAvailableCharName(char *pi_pName, BYTE pi_byUserDegree = 0);

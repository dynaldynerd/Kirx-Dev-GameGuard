#include "UIGlobal.h"

#include <cctype>
#include <cstdio>
#include <cstring>

namespace
{
typedef int(__cdecl *PFN_FILTER_LOAD_DIC)(char *);
typedef BOOL(__cdecl *PFN_FILTER_INIT_DIC)(void);
typedef BOOL(__cdecl *PFN_FILTER_DECODE_DIC)(void);
typedef BOOL(__cdecl *PFN_FILTER_CHECK_ABUSE)(char *);

struct FILTER_API
{
  HMODULE hModule;
  PFN_FILTER_LOAD_DIC LoadDic;
  PFN_FILTER_INIT_DIC InitDic;
  PFN_FILTER_DECODE_DIC DecodeDic;
  PFN_FILTER_CHECK_ABUSE CheckAbuse;
};

FILTER_API g_stAbuseApi = {};
FILTER_API g_stAdvApi = {};

BOOL g_bIsAbuseLoaded = FALSE;
BOOL g_bIsAdvLoaded = FALSE;

void WriteFilterLog(const char *pi_pMessage, const char *pi_pValue = NULL)
{
  char l_szBuffer[512];
  if (pi_pValue && pi_pValue[0])
  {
    sprintf_s(l_szBuffer, sizeof(l_szBuffer), "[RFClient][Filter] %s: %s\n", pi_pMessage, pi_pValue);
  }
  else
  {
    sprintf_s(l_szBuffer, sizeof(l_szBuffer), "[RFClient][Filter] %s\n", pi_pMessage);
  }

  OutputDebugStringA(l_szBuffer);
}

BOOL IsExistingFile(const char *pi_pFileName)
{
  if (!pi_pFileName || !pi_pFileName[0])
  {
    return FALSE;
  }

  const DWORD l_dwAttributes = GetFileAttributesA(pi_pFileName);
  return l_dwAttributes != INVALID_FILE_ATTRIBUTES && (l_dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

void ResetFilterApi(FILTER_API &po_rApi)
{
  if (po_rApi.hModule)
  {
    FreeLibrary(po_rApi.hModule);
  }

  ZeroMemory(&po_rApi, sizeof(po_rApi));
}

BOOL LoadFilterApi(const char *pi_pModuleName,
                   const char *pi_pPrefix,
                   FILTER_API &po_rApi)
{
  if (po_rApi.hModule)
  {
    return TRUE;
  }

  po_rApi.hModule = LoadLibraryA(pi_pModuleName);
  if (!po_rApi.hModule)
  {
    WriteFilterLog("Missing filter module", pi_pModuleName);
    return FALSE;
  }

  char l_szProcName[64];

  sprintf_s(l_szProcName, sizeof(l_szProcName), "%s_LoadDic", pi_pPrefix);
  po_rApi.LoadDic = reinterpret_cast<PFN_FILTER_LOAD_DIC>(GetProcAddress(po_rApi.hModule, l_szProcName));

  sprintf_s(l_szProcName, sizeof(l_szProcName), "%s_InitDic", pi_pPrefix);
  po_rApi.InitDic = reinterpret_cast<PFN_FILTER_INIT_DIC>(GetProcAddress(po_rApi.hModule, l_szProcName));

  sprintf_s(l_szProcName, sizeof(l_szProcName), "%s_DecodeDic", pi_pPrefix);
  po_rApi.DecodeDic = reinterpret_cast<PFN_FILTER_DECODE_DIC>(GetProcAddress(po_rApi.hModule, l_szProcName));

  sprintf_s(l_szProcName, sizeof(l_szProcName), "%s_CheckAbuse", pi_pPrefix);
  po_rApi.CheckAbuse = reinterpret_cast<PFN_FILTER_CHECK_ABUSE>(GetProcAddress(po_rApi.hModule, l_szProcName));

  if (!po_rApi.LoadDic || !po_rApi.InitDic || !po_rApi.DecodeDic || !po_rApi.CheckAbuse)
  {
    WriteFilterLog("Incomplete filter module exports", pi_pModuleName);
    ResetFilterApi(po_rApi);
    return FALSE;
  }

  return TRUE;
}

BOOL LoadFilterDictionary(const char *pi_pModuleName,
                          const char *pi_pPrefix,
                          char *pi_pDataFileName,
                          FILTER_API &po_rApi,
                          BOOL &po_rLoadedFlag)
{
  if (po_rLoadedFlag)
  {
    return TRUE;
  }

  if (!pi_pDataFileName || !pi_pDataFileName[0])
  {
    WriteFilterLog("Missing filter dictionary path");
    return FALSE;
  }

  if (!IsExistingFile(pi_pDataFileName))
  {
    WriteFilterLog("Missing filter dictionary", pi_pDataFileName);
    return FALSE;
  }

  if (!LoadFilterApi(pi_pModuleName, pi_pPrefix, po_rApi))
  {
    return FALSE;
  }

  po_rApi.LoadDic(pi_pDataFileName);
  if (!po_rApi.DecodeDic())
  {
    WriteFilterLog("Failed to decode filter dictionary", pi_pDataFileName);
    return FALSE;
  }

  po_rApi.InitDic();
  po_rLoadedFlag = TRUE;
  return TRUE;
}
} // namespace

BOOL _LoadAbuseFilter(char *pi_pAbuseDataFilename)
{
  return LoadFilterDictionary("ABuse.dll",
                              "ABuse",
                              pi_pAbuseDataFilename,
                              g_stAbuseApi,
                              g_bIsAbuseLoaded);
}

BOOL _LoadAdvFilter(char *pi_pAdvDataFilename)
{
  return LoadFilterDictionary("Adv.dll",
                              "Adv",
                              pi_pAdvDataFilename,
                              g_stAdvApi,
                              g_bIsAdvLoaded);
}

BOOL _IsAbuseStr(char *pi_pStr)
{
  if (!g_bIsAbuseLoaded || !pi_pStr || !g_stAbuseApi.CheckAbuse)
  {
    return FALSE;
  }

  return g_stAbuseApi.CheckAbuse(pi_pStr);
}

BOOL _IsAdvStr(char *pi_pStr)
{
  if (!g_bIsAdvLoaded || !pi_pStr || !g_stAdvApi.CheckAbuse)
  {
    return FALSE;
  }

  return g_stAdvApi.CheckAbuse(pi_pStr);
}

BOOL _IsAvailableCharName(char *pi_pName, BYTE pi_byUserDegree)
{
  if (!pi_pName || !strlen(pi_pName))
  {
    return FALSE;
  }

  if (_IsAbuseStr(pi_pName))
  {
    return FALSE;
  }

  for (int i = 0; pi_pName[i] != '\0'; ++i)
  {
    const unsigned char l_byChar = static_cast<unsigned char>(pi_pName[i]);
    if (l_byChar & 0x80)
    {
      continue;
    }

    if (std::isdigit(l_byChar) || std::isalpha(l_byChar))
    {
      continue;
    }

    if (pi_pName[i] == '[' || pi_pName[i] == ']')
    {
      continue;
    }

    return FALSE;
  }

#define USER_DEGREE_STD 0
#define USER_DEGREE_GID 1
#define USER_DEGREE_MGR 2
#define USER_DEGREE_DEV 3

  if (pi_byUserDegree >= USER_DEGREE_MGR)
  {
    return TRUE;
  }

  const char *l_apFilteringHanName[] =
  {
    "�����",
    "��������",
    "���",
    "���",
    "������",
    "������",
    "������",
    "���߻�",
    "������"
  };

  for (int i = 0; i < static_cast<int>(sizeof(l_apFilteringHanName) / sizeof(l_apFilteringHanName[0])); ++i)
  {
    if (strstr(pi_pName, l_apFilteringHanName[i]))
    {
      return FALSE;
    }
  }

  char l_pFilteringEngName[6][16] =
  {
    "GM",
    "X2",
    "GAMEMASTER",
    "CCR",
    "ODIN",
    "RISINGFORCE"
  };

  const WORD l_nNameStrLen = static_cast<WORD>(strlen(pi_pName));
  for (int i = 0; i < 6; ++i)
  {
    if (i == 0)
    {
      if (!_stricmp(pi_pName, l_pFilteringEngName[0]))
      {
        return FALSE;
      }
    }
    else
    {
      const WORD l_nFilterStrlen = static_cast<WORD>(strlen(l_pFilteringEngName[i]));
      if (l_nNameStrLen < l_nFilterStrlen)
      {
        continue;
      }

      for (int ch = 0; ch < l_nNameStrLen; ++ch)
      {
        if (l_nNameStrLen - ch < l_nFilterStrlen)
        {
          break;
        }

        if (!_strnicmp(pi_pName + ch, l_pFilteringEngName[i], l_nFilterStrlen))
        {
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}

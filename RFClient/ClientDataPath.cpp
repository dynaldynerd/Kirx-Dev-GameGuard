#include "ClientDataPath.h"

#include <cstdio>
#include <cstring>

namespace
{
const char kR3EngineIniFileName[] = ".\\R3Engine.ini";

struct CLIENT_LANGUAGE_MAP
{
  const char *pSetupName;
  const char *pNationFolder;
  const char *pFontName;
  DWORD dwFontCharSet;
};

const CLIENT_LANGUAGE_MAP kClientLanguageMap[] =
{
  { "Korea", "ko-kr", "GulimChe", HANGEUL_CHARSET },
  { "Brazil", "pt-br", "Arial", DEFAULT_CHARSET },
  { "China", "zh-cn", "SimSun", GB2312_CHARSET },
  { "Europe", "en-gb", "Arial", DEFAULT_CHARSET },
  { "Indonesia", "en-id", "Arial", DEFAULT_CHARSET },
  { "Japan", "ja-jp", "MS Gothic", SHIFTJIS_CHARSET },
  { "Philippines", "en-ph", "Tahoma", DEFAULT_CHARSET },
  { "Russia", "ru-ru", "Lucida Console", RUSSIAN_CHARSET },
  { "Taiwan", "zh-tw", "MingLiU", CHINESEBIG5_CHARSET },
  { "USA", "en-gb", "Arial", DEFAULT_CHARSET },
  { "Thailand", "th-th", "Tahoma", THAI_CHARSET },
  { "Turkey", "tr-tr", "Arial", DEFAULT_CHARSET }
};

const CLIENT_LANGUAGE_MAP kClientLocaleAliases[] =
{
  { "ko-kr", "ko-kr", "GulimChe", HANGEUL_CHARSET },
  { "pt-br", "pt-br", "Arial", DEFAULT_CHARSET },
  { "zh-cn", "zh-cn", "SimSun", GB2312_CHARSET },
  { "en-gb", "en-gb", "Arial", DEFAULT_CHARSET },
  { "en-id", "en-id", "Arial", DEFAULT_CHARSET },
  { "ja-jp", "ja-jp", "MS Gothic", SHIFTJIS_CHARSET },
  { "en-ph", "en-ph", "Tahoma", DEFAULT_CHARSET },
  { "ru-ru", "ru-ru", "Lucida Console", RUSSIAN_CHARSET },
  { "zh-tw", "zh-tw", "MingLiU", CHINESEBIG5_CHARSET },
  { "en-us", "en-gb", "Arial", DEFAULT_CHARSET },
  { "th-th", "th-th", "Tahoma", THAI_CHARSET },
  { "tr-tr", "tr-tr", "Arial", DEFAULT_CHARSET },
  { "es-es", "es-es", "Arial", DEFAULT_CHARSET },
  { "de-de", "de-de", "Arial", DEFAULT_CHARSET },
  { "English", "en-gb", "Arial", DEFAULT_CHARSET }
};

BOOL IsExistingFile(const char *pi_pFileName)
{
  if (!pi_pFileName || !pi_pFileName[0])
  {
    return FALSE;
  }

  const DWORD l_dwAttributes = GetFileAttributesA(pi_pFileName);
  return l_dwAttributes != INVALID_FILE_ATTRIBUTES && (l_dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

BOOL IsExistingDirectory(const char *pi_pPath)
{
  if (!pi_pPath || !pi_pPath[0])
  {
    return FALSE;
  }

  const DWORD l_dwAttributes = GetFileAttributesA(pi_pPath);
  return l_dwAttributes != INVALID_FILE_ATTRIBUTES && (l_dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

BOOL IsExistingNationFolder(const char *pi_pNationFolder)
{
  if (!pi_pNationFolder || !pi_pNationFolder[0])
  {
    return FALSE;
  }

  char l_szFolderName[MAX_PATH];
  sprintf_s(l_szFolderName,
            sizeof(l_szFolderName),
            ".\\DataTable\\%s",
            pi_pNationFolder);
  return IsExistingDirectory(l_szFolderName);
}

BOOL BuildNationDataFileName(const char *pi_pNationFolder,
                             const char *pi_pFileName,
                             char *po_pFileName,
                             size_t pi_nFileNameSize)
{
  if (!pi_pNationFolder || !pi_pNationFolder[0] ||
      !pi_pFileName || !pi_pFileName[0] ||
      !po_pFileName || !pi_nFileNameSize)
  {
    return FALSE;
  }

  char l_szFileName[MAX_PATH];
  sprintf_s(l_szFileName,
            sizeof(l_szFileName),
            ".\\DataTable\\%s\\%s",
            pi_pNationFolder,
            pi_pFileName);
  if (!IsExistingFile(l_szFileName))
  {
    return FALSE;
  }

  strcpy_s(po_pFileName, pi_nFileNameSize, l_szFileName);
  return TRUE;
}

const CLIENT_LANGUAGE_MAP *GetLanguageMapFromLanguage(const char *pi_pLanguage)
{
  if (!pi_pLanguage || !pi_pLanguage[0])
  {
    return &kClientLanguageMap[0];
  }

  for (int i = 0; i < static_cast<int>(sizeof(kClientLanguageMap) / sizeof(kClientLanguageMap[0])); ++i)
  {
    if (_stricmp(pi_pLanguage, kClientLanguageMap[i].pSetupName) == 0)
    {
      return &kClientLanguageMap[i];
    }
  }

  for (int i = 0; i < static_cast<int>(sizeof(kClientLocaleAliases) / sizeof(kClientLocaleAliases[0])); ++i)
  {
    if (_stricmp(pi_pLanguage, kClientLocaleAliases[i].pSetupName) == 0)
    {
      return &kClientLocaleAliases[i];
    }
  }

  return &kClientLanguageMap[0];
}

const CLIENT_LANGUAGE_MAP *GetAvailableLanguageMap(const CLIENT_LANGUAGE_MAP *pi_pPreferredLanguageMap)
{
  if (pi_pPreferredLanguageMap && IsExistingNationFolder(pi_pPreferredLanguageMap->pNationFolder))
  {
    return pi_pPreferredLanguageMap;
  }

  for (int i = 0; i < static_cast<int>(sizeof(kClientLanguageMap) / sizeof(kClientLanguageMap[0])); ++i)
  {
    if (IsExistingNationFolder(kClientLanguageMap[i].pNationFolder))
    {
      return &kClientLanguageMap[i];
    }
  }

  for (int i = 0; i < static_cast<int>(sizeof(kClientLocaleAliases) / sizeof(kClientLocaleAliases[0])); ++i)
  {
    if (IsExistingNationFolder(kClientLocaleAliases[i].pNationFolder))
    {
      return &kClientLocaleAliases[i];
    }
  }

  return pi_pPreferredLanguageMap ? pi_pPreferredLanguageMap : &kClientLanguageMap[0];
}

const CLIENT_LANGUAGE_MAP *GetCachedLanguageMap(void)
{
  static const CLIENT_LANGUAGE_MAP *s_pLanguageMap = NULL;
  if (s_pLanguageMap)
  {
    return s_pLanguageMap;
  }

  char l_szLanguage[32];
  ZeroMemory(l_szLanguage, sizeof(l_szLanguage));
  GetPrivateProfileStringA("Setup",
                           "Language",
                           "Korea",
                           l_szLanguage,
                           static_cast<DWORD>(sizeof(l_szLanguage)),
                           kR3EngineIniFileName);

  s_pLanguageMap = GetAvailableLanguageMap(GetLanguageMapFromLanguage(l_szLanguage));
  return s_pLanguageMap;
}
} // namespace

const char *GetClientNationFolder(void)
{
  static char s_szNationFolder[16] = "ko-kr";
  static bool s_bCached = false;
  if (s_bCached)
  {
    return s_szNationFolder;
  }

  strcpy_s(s_szNationFolder, sizeof(s_szNationFolder), GetCachedLanguageMap()->pNationFolder);
  s_bCached = true;
  return s_szNationFolder;
}

const char *GetClientLanguageFontName(void)
{
  return GetCachedLanguageMap()->pFontName;
}

DWORD GetClientLanguageFontCharSet(void)
{
  return GetCachedLanguageMap()->dwFontCharSet;
}

BOOL GetNationDataFileName(const char *pi_pFileName,
                           char *po_pFileName,
                           size_t pi_nFileNameSize)
{
  if (!pi_pFileName || !pi_pFileName[0] || !po_pFileName || !pi_nFileNameSize)
  {
    return FALSE;
  }

  if (BuildNationDataFileName(GetClientNationFolder(), pi_pFileName, po_pFileName, pi_nFileNameSize))
  {
    return TRUE;
  }

  return BuildNationDataFileName("ko-kr", pi_pFileName, po_pFileName, pi_nFileNameSize);
}

#include "ClientDataPath.h"

#include <cstdio>
#include <cstring>

namespace
{
const char kR3EngineIniFileName[] = ".\\R3Engine.ini";

BOOL IsExistingFile(const char *pi_pFileName)
{
  if (!pi_pFileName || !pi_pFileName[0])
  {
    return FALSE;
  }

  const DWORD l_dwAttributes = GetFileAttributesA(pi_pFileName);
  return l_dwAttributes != INVALID_FILE_ATTRIBUTES && (l_dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
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

const char *GetNationFolderFromLanguage(const char *pi_pLanguage)
{
  if (!pi_pLanguage || !pi_pLanguage[0])
  {
    return "ko-kr";
  }

  if (_stricmp(pi_pLanguage, "Brazil") == 0 || _stricmp(pi_pLanguage, "pt-br") == 0)
  {
    return "pt-br";
  }

  if (_stricmp(pi_pLanguage, "China") == 0 || _stricmp(pi_pLanguage, "zh-cn") == 0)
  {
    return "zh-cn";
  }

  if (_stricmp(pi_pLanguage, "Europe") == 0 ||
      _stricmp(pi_pLanguage, "English") == 0 ||
      _stricmp(pi_pLanguage, "USA") == 0 ||
      _stricmp(pi_pLanguage, "en-gb") == 0 ||
      _stricmp(pi_pLanguage, "en-us") == 0)
  {
    return "en-gb";
  }

  if (_stricmp(pi_pLanguage, "Indonesia") == 0 || _stricmp(pi_pLanguage, "en-id") == 0)
  {
    return "en-id";
  }

  if (_stricmp(pi_pLanguage, "Japan") == 0 || _stricmp(pi_pLanguage, "ja-jp") == 0)
  {
    return "ja-jp";
  }

  if (_stricmp(pi_pLanguage, "Philippines") == 0 || _stricmp(pi_pLanguage, "en-ph") == 0)
  {
    return "en-ph";
  }

  if (_stricmp(pi_pLanguage, "Russia") == 0 || _stricmp(pi_pLanguage, "ru-ru") == 0)
  {
    return "ru-ru";
  }

  if (_stricmp(pi_pLanguage, "Taiwan") == 0 || _stricmp(pi_pLanguage, "zh-tw") == 0)
  {
    return "zh-tw";
  }

  if (_stricmp(pi_pLanguage, "Thailand") == 0 || _stricmp(pi_pLanguage, "th-th") == 0)
  {
    return "th-th";
  }

  if (_stricmp(pi_pLanguage, "Turkey") == 0 || _stricmp(pi_pLanguage, "tr-tr") == 0)
  {
    return "tr-tr";
  }

  return "ko-kr";
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

  char l_szLanguage[32];
  ZeroMemory(l_szLanguage, sizeof(l_szLanguage));
  GetPrivateProfileStringA("Setup",
                           "Language",
                           "Korea",
                           l_szLanguage,
                           static_cast<DWORD>(sizeof(l_szLanguage)),
                           kR3EngineIniFileName);

  strcpy_s(s_szNationFolder, sizeof(s_szNationFolder), GetNationFolderFromLanguage(l_szLanguage));
  s_bCached = true;
  return s_szNationFolder;
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

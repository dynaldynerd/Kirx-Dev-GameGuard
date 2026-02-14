#include "pch.h"

#include "CIniFile.h"
#include "CMainThread.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <mbstring.h>

namespace
{
  const char kIniInvalid[] = "INVALID";
  const char kIniDeleteSectionEmpty[] = "";
}

CIniFile::CIniFile()
{
  m_strPath[0] = 0;
}

CIniFile::CIniFile(const char *strPath)
{
  SetIniFilename(strPath);
}

CIniFile::~CIniFile()
{
  Clear();
}

void CIniFile::SetIniFilename(const char *strPath)
{
  if (strPath)
  {
    strcpy_s(m_strPath, sizeof(m_strPath), strPath);
  }
}

void CIniFile::Clear()
{
  for (INI_Section *section : m_SectionList)
  {
    delete section;
  }
  m_SectionList.clear();
}

bool CIniFile::Load()
{
  Clear();
  size_t bufferSize = 128;
  auto *buffer = static_cast<char *>(operator new[](bufferSize + 1));
  memset_0(buffer, 0, bufferSize + 1);
  DWORD sectionNamesSize = GetPrivateProfileSectionNamesA(buffer, static_cast<DWORD>(bufferSize), m_strPath);
  if (!sectionNamesSize)
  {
    operator delete[](buffer);
    return false;
  }
  while (static_cast<unsigned __int64>(sectionNamesSize + 2) >= bufferSize)
  {
    bufferSize += 128;
    operator delete[](buffer);
    buffer = static_cast<char *>(operator new[](bufferSize + 1));
    sectionNamesSize = GetPrivateProfileSectionNamesA(buffer, static_cast<DWORD>(bufferSize), m_strPath);
  }

  char *str = buffer;
  for (size_t len = strlen_0(buffer); len; len = strlen_0(str))
  {
    INI_Section *section = LoadSection(str);
    if (section)
    {
      m_SectionList.push_back(section);
    }
    str += len + 1;
  }
  operator delete[](buffer);
  return true;
}

void CIniFile::Save()
{
  for (INI_Section *section : m_SectionList)
  {
    SaveSection(section);
  }
}

bool CIniFile::WriteString(const char *strSection, const char *strKey, const char *strValue)
{
  INI_Section *section = GetSection(strSection);
  if (!section)
  {
    section = new INI_Section();
    if (!section)
    {
      return false;
    }
    sprintf_s(section->m_strSection, "%s", strSection);
    m_SectionList.push_back(section);
  }

  INI_Key *key = section->GetKey(strKey);
  if (key)
  {
    if (strcmp_0(key->m_strValue, strValue))
    {
      sprintf_s(key->m_strValue, "%s", strValue);
      SaveKey(key);
    }
  }
  else
  {
    key = new INI_Key();
    if (!key)
    {
      return false;
    }
    sprintf_s(key->m_strKey, "%s", strKey);
    sprintf_s(key->m_strValue, "%s", strValue);
    section->m_KeyList.push_back(key);
    key->m_pParentSection = section;
    SaveKey(key);
  }

  return true;
}

INI_Section *CIniFile::LoadSection(const char *strSection)
{
  if (!strSection)
  {
    return nullptr;
  }

  size_t bufferSize = 128;
  auto *lpReturnedString = static_cast<char *>(operator new[](bufferSize + 1));
  DWORD sectionSize = GetPrivateProfileSectionA(strSection, lpReturnedString, static_cast<DWORD>(bufferSize), m_strPath);
  while (static_cast<unsigned __int64>(sectionSize + 2) >= bufferSize)
  {
    bufferSize += 128;
    operator delete[](lpReturnedString);
    lpReturnedString = static_cast<char *>(operator new[](bufferSize + 1));
    sectionSize = GetPrivateProfileSectionA(strSection, lpReturnedString, static_cast<DWORD>(bufferSize), m_strPath);
  }

  char *str = lpReturnedString;
  size_t len = strlen_0(lpReturnedString);
  if (!len)
  {
    operator delete[](lpReturnedString);
    return nullptr;
  }

  unsigned __int8 strKey[88]{};
  INI_Section *section = new INI_Section();
  if (!section)
  {
    operator delete[](lpReturnedString);
    return nullptr;
  }
  sprintf_s(section->m_strSection, "%s", strSection);

  while (len)
  {
    if (!SplitKey(reinterpret_cast<unsigned __int8 *>(str), strKey, 65))
    {
      operator delete[](lpReturnedString);
      return section;
    }
    INI_Key *key = LoadKey(strSection, reinterpret_cast<const char *>(strKey));
    if (key)
    {
      section->m_KeyList.push_back(key);
      key->m_pParentSection = section;
    }
    str += len + 1;
    len = strlen_0(str);
  }
  operator delete[](lpReturnedString);
  return section;
}

INI_Key *CIniFile::LoadKey(const char *strSection, const char *strKey)
{
  char returnedString[88]{};
  GetPrivateProfileStringA(strSection, strKey, kIniInvalid, returnedString, 0x41u, m_strPath);
  if (!strcmp_0(returnedString, kIniInvalid))
  {
    return nullptr;
  }

  INI_Key *key = new INI_Key();
  if (!key)
  {
    return nullptr;
  }
  sprintf_s(key->m_strKey, "%s", strKey);
  sprintf_s(key->m_strValue, "%s", returnedString);
  return key;
}

bool CIniFile::SplitKey(unsigned __int8 *strBase, unsigned __int8 *strKey, int nKeyLen)
{
  if (!strBase || !strKey)
  {
    return false;
  }
  unsigned __int8 *sep = _mbschr(strBase, '=');
  if (!sep || sep == strBase)
  {
    return false;
  }
  int j = 0;
  for (; &strBase[j] < sep; ++j)
  {
    strKey[j] = strBase[j];
  }
  strKey[j] = 0;
return true;
}

INI_Section *CIniFile::GetSection(unsigned int dwIndex)
{
  if (dwIndex >= static_cast<unsigned int>(m_SectionList.size()))
  {
    return nullptr;
  }
  return m_SectionList[dwIndex];
}

INI_Section *CIniFile::GetSection(const char *strSection)
{
  for (INI_Section *section : m_SectionList)
  {
    if (!strcmp_0(section->m_strSection, strSection))
    {
      return section;
    }
  }
  return nullptr;
}

unsigned __int64 CIniFile::GetSectionSize()
{
  return m_SectionList.size();
}

INI_Key *CIniFile::GetKey(const char *strSection, const char *strKey)
{
  INI_Section *section = GetSection(strSection);
  if (section)
  {
    return section->GetKey(strKey);
  }
  return nullptr;
}

bool CIniFile::SaveKey(INI_Key *pKey)
{
  if (!pKey)
  {
    return false;
  }
  const char *section = pKey->m_pParentSection->m_strSection;
  return section && WritePrivateProfileStringA(section, pKey->m_strKey, pKey->m_strValue, m_strPath);
}

bool CIniFile::SaveSection(INI_Section *pSection)
{
  if (!pSection)
  {
    return false;
  }
  for (INI_Key *key : pSection->m_KeyList)
  {
    if (!SaveKey(key))
    {
      return false;
    }
  }
  return true;
}

BOOL CIniFile::DeleteKey(const char *lpSection, const char *lpKey)
{
  return WritePrivateProfileStringA(lpSection, lpKey, nullptr, m_strPath);
}

bool CIniFile::DeleteKey(INI_Key *pKey)
{
  if (!pKey)
  {
    return false;
  }
  INI_Section *parent = pKey->m_pParentSection;
  if (!parent)
  {
    return false;
  }
  if (!DeleteKey(parent->m_strSection, pKey->m_strKey))
  {
    return false;
  }
  auto &keys = parent->m_KeyList;
  auto it = std::find(keys.begin(), keys.end(), pKey);
  if (it != keys.end())
  {
    keys.erase(it);
  }
  delete pKey;
  return true;
}

BOOL CIniFile::DeleteSection(const char *lpSection)
{
  return WritePrivateProfileStringA(lpSection, nullptr, kIniDeleteSectionEmpty, m_strPath);
}

bool CIniFile::DeleteSection(INI_Section *pSection)
{
  if (!pSection)
  {
    return false;
  }
  for (INI_Key *key : pSection->m_KeyList)
  {
    if (!DeleteKey(pSection->m_strSection, key->m_strKey))
    {
      return false;
    }
    delete key;
  }
  pSection->m_KeyList.clear();
  if (!DeleteSection(pSection->m_strSection))
  {
    return false;
  }
  auto it = std::find(m_SectionList.begin(), m_SectionList.end(), pSection);
  if (it != m_SectionList.end())
  {
    m_SectionList.erase(it);
  }
  delete pSection;
  return true;
}

bool CIniFile::Merge_SumOfSets(CIniFile *rhs)
{
  for (INI_Section *section : rhs->m_SectionList)
  {
    for (INI_Key *key : section->m_KeyList)
    {
      if (!WriteString(section->m_strSection, key->m_strKey, key->m_strValue))
      {
        return false;
      }
    }
  }
  return true;
}

bool CIniFile::Merge_Intersection(CIniFile *rhs)
{
  for (INI_Section *section : rhs->m_SectionList)
  {
    for (INI_Key *key : section->m_KeyList)
    {
      INI_Key *existing = GetKey(section->m_strSection, key->m_strKey);
      if (existing && !WriteString(section->m_strSection, key->m_strKey, key->m_strValue))
      {
        return false;
      }
    }
  }
  return true;
}

INI_Section::INI_Section()
{
  m_strSection[0] = 0;
}

INI_Section::~INI_Section()
{
  for (INI_Key *key : m_KeyList)
  {
    delete key;
  }
  m_KeyList.clear();
}

INI_Key *INI_Section::GetKey(const char *strKey)
{
  for (INI_Key *key : m_KeyList)
  {
    if (!strcmp_0(key->m_strKey, strKey))
    {
      return key;
    }
  }
  return nullptr;
}

INI_Key::INI_Key()
{
  m_strKey[0] = 0;
  m_strValue[0] = 0;
  m_pParentSection = nullptr;
}


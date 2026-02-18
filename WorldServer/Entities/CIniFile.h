#pragma once

#include "IdaCompat.h"

#include <vector>

struct INI_Section;
struct INI_Key;

/* 1438 */
class  CIniFile
{
public:
  CIniFile();
  explicit CIniFile(const char *strPath);
  virtual ~CIniFile();

  void SetIniFilename(const char *strPath);
  void Clear();
  bool Load();
  void Save();
  bool WriteString(const char *strSection, const char *strKey, const char *strValue);
  INI_Section *LoadSection(const char *strSection);
  INI_Key *LoadKey(const char *strSection, const char *strKey);
  bool SplitKey(unsigned __int8 *strBase, unsigned __int8 *strKey, int nKeyLen);
  INI_Section *GetSection(unsigned int dwIndex);
  INI_Section *GetSection(const char *strSection);
  unsigned __int64 GetSectionSize();
  INI_Key *GetKey(const char *strSection, const char *strKey);
  bool SaveKey(INI_Key *pKey);
  bool SaveSection(INI_Section *pSection);
  BOOL DeleteKey(const char *lpSection, const char *lpKey);
  bool DeleteKey(INI_Key *pKey);
  BOOL DeleteSection(const char *lpSection);
  bool DeleteSection(INI_Section *pSection);
  bool Merge_SumOfSets(CIniFile *rhs);
  bool Merge_Intersection(CIniFile *rhs);

  char m_strPath[260];
  std::vector<INI_Section *> m_SectionList;
};


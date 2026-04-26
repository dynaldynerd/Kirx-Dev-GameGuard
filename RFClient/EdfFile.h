#pragma once

#include <windows.h>

#include "DataFile.h"

class CEdfFile
{
public:
  CEdfFile();
  ~CEdfFile();

  BOOL Load(const char *pi_pFileName);
  void Unload(void);

  BOOL IsLoaded(void) const;
  CDataString *GetSourceData(void) const;
  const char *GetFileName(void) const;

private:
  CDataFile *m_pDataFile;
  char m_szFileName[MAX_PATH];
};

#pragma once

#include "IdaCompat.h"

/* 6041 */
class  strFILE
{
public:
  strFILE();
  virtual ~strFILE();

  bool load(const char *pszFileName);
  bool word(char *poutszWord);
  bool word(int *pnoutVal);
  bool word(float *pfoutVal);
  bool word(long double *pdoutVal);
  int read_line_count();

  char *m_pLoadStr;
  char *m_pReadStr;
  unsigned int m_dwLoadSize;
};


#pragma once

#include "IdaCompat.h"

/* 6041 */
class __cppobj __declspec(align(8)) strFILE
{
public:
  strFILE();
  virtual ~strFILE();

  bool load(const char *pszFileName);
  bool word(char *poutszWord);
  bool word(int *pnoutVal);
  bool word(float *pfoutVal);
  bool word(long double *pdoutVal);
  unsigned int read_line_count();

  char *m_pLoadStr;
  char *m_pReadStr;
  unsigned int m_dwLoadSize;
};

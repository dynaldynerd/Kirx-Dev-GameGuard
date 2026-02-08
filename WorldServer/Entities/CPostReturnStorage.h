#pragma once

#include "IdaCompat.h"
#include "CPostData.h"

/* 1704 */
class __cppobj __declspec(align(8)) CPostReturnStorage
{
public:
  CPostReturnStorage();
  void Init();
  int GetSize();
  CPostData *GetPostDataFromInx(unsigned int nIndex);

  CPostData m_PostData[10];
  int m_nSize;
};


#pragma once

#include "IdaCompat.h"
#include "CPostData.h"

/* 1703 */
class __cppobj __declspec(align(4)) CPostStorage
{
public:
  CPostStorage();
  void Init();
  int GetSize();
  CPostData *GetPostDataFromInx(unsigned int nIndex);

  CPostData m_PostData[50];
  int m_nSize;
  bool m_bUpdate;
};


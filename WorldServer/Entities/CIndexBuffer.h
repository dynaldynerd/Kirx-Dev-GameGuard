#pragma once

#include "D3D8Compat.h"

/* 1392 */
union $736DB51CE9487475C2E813E4336CA27C
{
  IDirect3DIndexBuffer8 *m_lpIndexBuffer;
  unsigned short *m_IndexBuffer;
};

/* 1393 */
class __cppobj CIndexBuffer
{
  unsigned int m_Flag;
  unsigned int m_Size;
  $736DB51CE9487475C2E813E4336CA27C ___u2;
};


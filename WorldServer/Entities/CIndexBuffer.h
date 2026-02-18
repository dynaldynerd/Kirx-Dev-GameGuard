#pragma once

#include "D3D8Compat.h"

/* 1392 */
union $736DB51CE9487475C2E813E4336CA27C
{
  IDirect3DIndexBuffer8 *m_lpIndexBuffer;
  unsigned short *m_IndexBuffer;
};

/* 1393 */
class  CIndexBuffer
{
  unsigned int m_Flag;
  unsigned int m_Size;
  $736DB51CE9487475C2E813E4336CA27C ___u2;

public:
  CIndexBuffer() : m_Flag(0), m_Size(0) { ___u2.m_lpIndexBuffer = nullptr; }
  ~CIndexBuffer() = default;

  void InitIndexBuffer(unsigned int a2, unsigned int a3);
  void ReleaseIndexBuffer();
  IDirect3DIndexBuffer8 *VPLock(__int64 a2 = 0, __int64 a3 = 0, unsigned int a4 = 0);
  void VPUnLock();
};


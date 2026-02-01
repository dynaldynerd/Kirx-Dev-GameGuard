#pragma once

#include "D3D8Compat.h"

/* 1390 */
union $36F8BC5CB18F337A8B949972D0D6A08A
{
  IDirect3DVertexBuffer8 *m_lpVertexBuffer;
  _D3DR3VERTEX_TEX1 *m_VertexBufferTex1;
  _D3DR3VERTEX_TEX2 *m_VertexBufferTex2;
};

/* 1391 */
class __cppobj CVertexBuffer
{
  unsigned int m_Flag;
  unsigned int m_Size;
  $36F8BC5CB18F337A8B949972D0D6A08A ___u2;

public:
  CVertexBuffer() : m_Flag(0), m_Size(0) { ___u2.m_lpVertexBuffer = nullptr; }
  ~CVertexBuffer() = default;

  void InitVertexBuffer(unsigned int a2, unsigned int a3, unsigned int a4);
  void ReleaseVertexBuffer();
  IDirect3DVertexBuffer8 *VPLock(__int64 a2 = 0, __int64 a3 = 0, unsigned int a4 = 0);
  void VPUnLock();
};


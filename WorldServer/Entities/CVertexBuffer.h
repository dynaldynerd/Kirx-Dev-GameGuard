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
};


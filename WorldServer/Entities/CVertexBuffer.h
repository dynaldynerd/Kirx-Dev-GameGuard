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
class  CVertexBuffer
{
public:
  unsigned int m_Flag;
  unsigned int m_Size;
  union
  {
    $36F8BC5CB18F337A8B949972D0D6A08A ___u2;
    struct
    {
      IDirect3DVertexBuffer8 *m_lpVertexBuffer;
      _D3DR3VERTEX_TEX1 *m_VertexBufferTex1;
      _D3DR3VERTEX_TEX2 *m_VertexBufferTex2;
    };
  };

  CVertexBuffer();
  ~CVertexBuffer();

  void InitVertexBuffer(unsigned int size, unsigned int bufferType, unsigned int fvf);
  void ReleaseVertexBuffer();
  IDirect3DVertexBuffer8 *VPLock(__int64 offset = 0, __int64 size = 0, unsigned int flags = 0);
  void VPUnLock();
};


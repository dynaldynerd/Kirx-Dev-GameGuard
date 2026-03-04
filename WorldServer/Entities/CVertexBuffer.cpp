#include "pch.h"

#include "CVertexBuffer.h"
#include "WorldServerUtil.h"
#include "R3EngineGlobals.h"

CVertexBuffer::CVertexBuffer()
{
  m_Flag = 0;
  m_Size = 0;
  ___u2.m_lpVertexBuffer = nullptr;
}

CVertexBuffer::~CVertexBuffer()
{
}

void CVertexBuffer::InitVertexBuffer(unsigned int a2, unsigned int a3, unsigned int a4)
{
  dword_184A7B2D8 += a2;
  m_Size = a2;
  if (!a2)
  {
    ___u2.m_lpVertexBuffer = nullptr;
    return;
  }

  ++dword_184A7B2DC;
  m_Flag = a3;

  IDirect3DVertexBuffer8 *v11 = nullptr;
  unsigned int v7 = a3 - 1;
  if (v7)
  {
    unsigned int v8 = v7 - 1;
    if (v8)
    {
      if (v8 == 1)
        v11 = reinterpret_cast<IDirect3DVertexBuffer8 *>(Dmalloc(static_cast<int>(a2)));
      goto LABEL_10;
    }
    IDirect3DDevice8 *D3dDevice = GetD3dDevice();
    if (D3dDevice)
      D3dDevice->CreateVertexBuffer(D3dDevice, a2, 8, a4, D3DPOOL_MANAGED, &v11);
  }
  else
  {
    IDirect3DDevice8 *D3dDevice = GetD3dDevice();
    if (D3dDevice)
      D3dDevice->CreateVertexBuffer(D3dDevice, a2, 520, a4, D3DPOOL_MANAGED, &v11);
  }

LABEL_10:
  if (!v11)
    Error(byte_140884910, byte_140883769);
  ___u2.m_lpVertexBuffer = v11;
}

void CVertexBuffer::ReleaseVertexBuffer()
{
  IDirect3DVertexBuffer8 *m_lpVertexBuffer = ___u2.m_lpVertexBuffer;
  if (!m_lpVertexBuffer)
    return;

  if (m_Flag == 3)
    Dfree(m_lpVertexBuffer);
  else
    m_lpVertexBuffer->Release(m_lpVertexBuffer);
  dword_184A7B2D8 -= m_Size;
  ___u2.m_lpVertexBuffer = nullptr;
  m_Flag = 0;
  m_Size = 0;
}

IDirect3DVertexBuffer8 *CVertexBuffer::VPLock(__int64 a2, __int64 a3, unsigned int a4)
{
  IDirect3DVertexBuffer8 *m_lpVertexBuffer = ___u2.m_lpVertexBuffer;
  IDirect3DVertexBuffer8 *v6 = nullptr;
  if (!m_lpVertexBuffer)
    return nullptr;

  switch (m_Flag)
  {
    case 1u:
      m_lpVertexBuffer->Lock(m_lpVertexBuffer, static_cast<unsigned int>(a2), static_cast<unsigned int>(a3), reinterpret_cast<unsigned char **>(&v6), a4);
      break;
    case 2u:
      m_lpVertexBuffer->Lock(m_lpVertexBuffer, static_cast<unsigned int>(a2), static_cast<unsigned int>(a3), reinterpret_cast<unsigned char **>(&v6), 0);
      break;
    case 3u:
      v6 = m_lpVertexBuffer;
      break;
    default:
      Error(byte_140884930, byte_140883769);
      return v6;
  }

  if (!v6)
    Error(byte_140884930, byte_140883769);
  return v6;
}

void CVertexBuffer::VPUnLock()
{
  if (___u2.m_lpVertexBuffer && m_Flag != 3)
    ___u2.m_lpVertexBuffer->Unlock(___u2.m_lpVertexBuffer);
}

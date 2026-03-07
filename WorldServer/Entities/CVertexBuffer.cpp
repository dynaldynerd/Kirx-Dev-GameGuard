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

void CVertexBuffer::InitVertexBuffer(unsigned int size, unsigned int bufferType, unsigned int fvf)
{
  dword_184A7B2D8 += size;
  m_Size = size;
  if (!size)
  {
    ___u2.m_lpVertexBuffer = nullptr;
    return;
  }

  ++dword_184A7B2DC;
  m_Flag = bufferType;

  IDirect3DVertexBuffer8 *vertexBuffer = nullptr;
  const unsigned int lockMode = bufferType - 1;
  if (lockMode)
  {
    const unsigned int allocationMode = lockMode - 1;
    if (allocationMode)
    {
      if (allocationMode == 1)
        vertexBuffer = reinterpret_cast<IDirect3DVertexBuffer8 *>(Dmalloc(static_cast<int>(size)));
    }
    else
    {
      IDirect3DDevice8 *D3dDevice = GetD3dDevice();
      if (D3dDevice)
        D3dDevice->CreateVertexBuffer(D3dDevice, size, 8, fvf, D3DPOOL_MANAGED, &vertexBuffer);
    }
  }
  else
  {
    IDirect3DDevice8 *D3dDevice = GetD3dDevice();
    if (D3dDevice)
      D3dDevice->CreateVertexBuffer(D3dDevice, size, 520, fvf, D3DPOOL_MANAGED, &vertexBuffer);
  }

  if (!vertexBuffer)
    Error(byte_140884910, byte_140883769);
  ___u2.m_lpVertexBuffer = vertexBuffer;
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

IDirect3DVertexBuffer8 *CVertexBuffer::VPLock(__int64 offset, __int64 size, unsigned int flags)
{
  IDirect3DVertexBuffer8 *m_lpVertexBuffer = ___u2.m_lpVertexBuffer;
  IDirect3DVertexBuffer8 *lockedBuffer = nullptr;
  if (!m_lpVertexBuffer)
    return nullptr;

  switch (m_Flag)
  {
    case 1u:
      m_lpVertexBuffer->Lock(
        m_lpVertexBuffer,
        static_cast<unsigned int>(offset),
        static_cast<unsigned int>(size),
        reinterpret_cast<unsigned char **>(&lockedBuffer),
        flags);
      break;
    case 2u:
      m_lpVertexBuffer->Lock(
        m_lpVertexBuffer,
        static_cast<unsigned int>(offset),
        static_cast<unsigned int>(size),
        reinterpret_cast<unsigned char **>(&lockedBuffer),
        0);
      break;
    case 3u:
      lockedBuffer = m_lpVertexBuffer;
      break;
    default:
      Error(byte_140884930, byte_140883769);
      return lockedBuffer;
  }

  if (!lockedBuffer)
    Error(byte_140884930, byte_140883769);
  return lockedBuffer;
}

void CVertexBuffer::VPUnLock()
{
  if (___u2.m_lpVertexBuffer && m_Flag != 3)
    ___u2.m_lpVertexBuffer->Unlock(___u2.m_lpVertexBuffer);
}

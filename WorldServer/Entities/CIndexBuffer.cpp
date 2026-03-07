#include "pch.h"

#include "CIndexBuffer.h"
#include "WorldServerUtil.h"
#include "R3EngineGlobals.h"

CIndexBuffer::CIndexBuffer()
{
  m_Flag = 0;
  m_Size = 0;
  ___u2.m_lpIndexBuffer = nullptr;
}

CIndexBuffer::~CIndexBuffer()
{
  if (___u2.m_lpIndexBuffer)
  {
    ReleaseIndexBuffer();
  }
}

void CIndexBuffer::InitIndexBuffer(unsigned int size, unsigned int bufferType)
{
  dword_184A7B2D8 += size;
  m_Size = size;
  if (size)
  {
    ++dword_184A7B2DC;
    m_Flag = bufferType;
    IDirect3DIndexBuffer8 *indexBuffer = nullptr;
    if (bufferType == 3)
    {
      indexBuffer = reinterpret_cast<IDirect3DIndexBuffer8 *>(Dmalloc(static_cast<int>(size)));
    }
    else
    {
      IDirect3DDevice8 *D3dDevice = GetD3dDevice();
      if (D3dDevice)
        D3dDevice->CreateIndexBuffer(D3dDevice, size, 8, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBuffer);
    }
    if (!indexBuffer)
    {
      Error(aAiu, byte_140883769);
    }
    ___u2.m_lpIndexBuffer = indexBuffer;
  }
  else
  {
    ___u2.m_lpIndexBuffer = nullptr;
  }
}

void CIndexBuffer::ReleaseIndexBuffer()
{
  IDirect3DIndexBuffer8 *m_lpIndexBuffer = ___u2.m_lpIndexBuffer;
  if (!m_lpIndexBuffer)
    return;
  if (m_Flag == 3)
    Dfree(m_lpIndexBuffer);
  else
    m_lpIndexBuffer->Release(m_lpIndexBuffer);
  dword_184A7B2D8 -= m_Size;
  ___u2.m_lpIndexBuffer = nullptr;
  m_Flag = 0;
  m_Size = 0;
}

IDirect3DIndexBuffer8 *CIndexBuffer::VPLock(__int64 offset, __int64 size, unsigned int flags)
{
  IDirect3DIndexBuffer8 *m_lpIndexBuffer = ___u2.m_lpIndexBuffer;
  IDirect3DIndexBuffer8 *lockedBuffer = nullptr;
  if (!m_lpIndexBuffer)
    return nullptr;

  switch (m_Flag)
  {
    case 1u:
      m_lpIndexBuffer->Lock(
        m_lpIndexBuffer,
        static_cast<unsigned int>(offset),
        static_cast<unsigned int>(size),
        reinterpret_cast<unsigned char **>(&lockedBuffer),
        flags);
      break;
    case 2u:
      m_lpIndexBuffer->Lock(
        m_lpIndexBuffer,
        static_cast<unsigned int>(offset),
        static_cast<unsigned int>(size),
        reinterpret_cast<unsigned char **>(&lockedBuffer),
        0);
      break;
    case 3u:
      lockedBuffer = m_lpIndexBuffer;
      break;
    default:
      Error(aAiao, byte_140883769);
      return lockedBuffer;
  }
  if (!lockedBuffer)
    Error(aAiao, byte_140883769);
  return lockedBuffer;
}

void CIndexBuffer::VPUnLock()
{
  if (___u2.m_lpIndexBuffer && m_Flag != 3)
    ___u2.m_lpIndexBuffer->Unlock(___u2.m_lpIndexBuffer);
}

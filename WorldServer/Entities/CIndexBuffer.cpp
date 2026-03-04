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

void CIndexBuffer::InitIndexBuffer(unsigned int a2, unsigned int a3)
{
  dword_184A7B2D8 += a2;
  m_Size = a2;
  if (a2)
  {
    ++dword_184A7B2DC;
    m_Flag = a3;
    IDirect3DIndexBuffer8 *v7 = nullptr;
    if (a3 == 3)
    {
      v7 = reinterpret_cast<IDirect3DIndexBuffer8 *>(Dmalloc(static_cast<int>(a2)));
    }
    else
    {
      IDirect3DDevice8 *D3dDevice = GetD3dDevice();
      if (D3dDevice)
        D3dDevice->CreateIndexBuffer(D3dDevice, a2, 8, D3DFMT_INDEX16, D3DPOOL_MANAGED, &v7);
    }
    if (!v7)
    {
      Error(aAiu, byte_140883769);
    }
    ___u2.m_lpIndexBuffer = v7;
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

IDirect3DIndexBuffer8 *CIndexBuffer::VPLock(__int64 a2, __int64 a3, unsigned int a4)
{
  IDirect3DIndexBuffer8 *m_lpIndexBuffer = ___u2.m_lpIndexBuffer;
  IDirect3DIndexBuffer8 *v6 = nullptr;
  if (!m_lpIndexBuffer)
    return nullptr;

  switch (m_Flag)
  {
    case 1u:
      m_lpIndexBuffer->Lock(m_lpIndexBuffer, static_cast<unsigned int>(a2), static_cast<unsigned int>(a3), reinterpret_cast<unsigned char **>(&v6), a4);
      break;
    case 2u:
      m_lpIndexBuffer->Lock(m_lpIndexBuffer, static_cast<unsigned int>(a2), static_cast<unsigned int>(a3), reinterpret_cast<unsigned char **>(&v6), 0);
      break;
    case 3u:
      v6 = m_lpIndexBuffer;
      break;
    default:
      Error(aAiao, byte_140883769);
      return v6;
  }
  if (!v6)
    Error(aAiao, byte_140883769);
  return v6;
}

void CIndexBuffer::VPUnLock()
{
  if (___u2.m_lpIndexBuffer && m_Flag != 3)
    ___u2.m_lpIndexBuffer->Unlock(___u2.m_lpIndexBuffer);
}

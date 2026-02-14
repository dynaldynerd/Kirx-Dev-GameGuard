#include "pch.h"

#include "CCryptor.h"

#include <new>

#include <osrng.h>
#include <sha.h>

#include "CCryptParam.h"
#include "CCryptParamCrete.h"

CCryptor::CCryptor()
  : m_pkParam(nullptr),
    m_pHash(nullptr)
{
}

CCryptor::~CCryptor()
{
  if (m_pkParam)
  {
    delete m_pkParam;
    m_pkParam = nullptr;
  }

  if (m_prng)
  {
    delete m_prng;
    m_prng = nullptr;
  }

  if (m_pHash)
  {
    delete m_pHash;
    m_pHash = nullptr;
  }
}

bool CCryptor::Init(const char *szKeyPath, bool bUseCreate)
{
  m_prng = new (std::nothrow) CryptoPP::AutoSeededRandomPool(false, 0x20u);
  if (!m_prng)
  {
    return false;
  }

  m_pHash = new (std::nothrow) CryptoPP::SHA256();
  if (!m_pHash)
  {
    return false;
  }

  if (bUseCreate)
  {
    m_pkParam = new (std::nothrow) CCryptParamCrete(m_prng);
  }
  else
  {
    m_pkParam = new (std::nothrow) CCryptParam(m_prng);
  }

  if (!m_pkParam)
  {
    return false;
  }

  return m_pkParam->Load(szKeyPath);
}

bool CCryptor::MakeHash(
  const unsigned __int8 *pBuff,
  unsigned __int64 tBufSize,
  unsigned __int8 *pHash,
  unsigned __int64 tHashSize)
{
  if (tHashSize != 32)
  {
    return false;
  }

  m_pHash->Update(pBuff, tBufSize);
  m_pHash->Final(pHash);
  return true;
}

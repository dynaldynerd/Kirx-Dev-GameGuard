#include "pch.h"

#include "CCryptor.h"

#include <new>

#include <osrng.h>
#include <sha.h>

#include "CCryptParam.h"
#include "CCryptParamCrete.h"

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

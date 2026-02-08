#pragma once

#include "IdaCompat.h"

#include "CTSingleton.h"

class CCryptParam;

namespace CryptoPP
{
  class SHA256;
  class AutoSeededRandomPool;
}

class __cppobj CCryptor : public CTSingleton<CCryptor>
{
public:
  bool Init(const char *szKeyPath, bool bUseCreate);
  bool MakeHash(
    const unsigned __int8 *pBuff,
    unsigned __int64 tBufSize,
    unsigned __int8 *pHash,
    unsigned __int64 tHashSize);

  CCryptParam *m_pkParam;
  CryptoPP::SHA256 *m_pHash;
  CryptoPP::AutoSeededRandomPool *m_prng;
};

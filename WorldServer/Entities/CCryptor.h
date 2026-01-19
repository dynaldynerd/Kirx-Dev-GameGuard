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
  CCryptParam *m_pkParam;
  CryptoPP::SHA256 *m_pHash;
  CryptoPP::AutoSeededRandomPool *m_prng;
};

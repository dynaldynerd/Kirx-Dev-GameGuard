#pragma once

#include "IdaCompat.h"

#include "CTSingleton.h"

class CCryptParam;

namespace CryptoPP
{
  class SHA256;
  class AutoSeededRandomPool;
}

class  CCryptor : public CTSingleton<CCryptor>
{
public:
  CCryptor();
  ~CCryptor();

  bool Init(const char *szKeyPath, bool bUseCreate);
  bool CreateKey();
  bool SaveKey();
  bool GetKeyStr(char *szKeyStr, unsigned __int64 tBuffSize);
  void GenerateBlock(unsigned __int8 *output, unsigned __int64 size);
  unsigned __int64 CiphertextLength(unsigned __int64 tMaxUseSize);
  unsigned __int64 MaxPlaintextLength(unsigned __int64 ciphertextLength);
  bool Encrypt(
    const unsigned __int8 *pText,
    unsigned __int64 tLength,
    unsigned __int8 *pCipherText,
    unsigned __int64 tCipherTextLength);
  bool Decrypt(
    const unsigned __int8 *pCipherText,
    unsigned __int64 tCipherTextLength,
    unsigned __int8 *pText);
  bool MakeHash(
    const unsigned __int8 *pBuff,
    unsigned __int64 tBufSize,
    unsigned __int8 *pHash,
    unsigned __int64 tHashSize);
  bool VerifyHash(
    const unsigned __int8 *pBuff,
    unsigned __int64 tBufSize,
    unsigned __int8 *pHash,
    unsigned __int64 tHashSize);

  CCryptParam *m_pkParam;
  CryptoPP::SHA256 *m_pHash;
  CryptoPP::AutoSeededRandomPool *m_prng;
};

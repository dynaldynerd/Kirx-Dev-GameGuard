#pragma once

#include "IdaCompat.h"

#include <aes.h>
#include <eccrypto.h>
#include <modes.h>
#include <osrng.h>

namespace CryptoPP
{
  class ByteQueue;
}

class  CCryptParam
{
public:
  explicit CCryptParam(CryptoPP::AutoSeededRandomPool *prng);
  virtual ~CCryptParam();

  void Alloc();
  unsigned __int64 MaxPlaintextLength(unsigned __int64 ciphertextLength);
  unsigned __int64 CiphertextLength(unsigned __int64 tMaxUseSize);
  bool Encrypt(
    const unsigned __int8 *pText,
    unsigned __int64 tLength,
    unsigned __int8 *pCipherText,
    unsigned __int64 tCipherTextLength);
  bool Load(const char *szKeyPath);

protected:
  void SetAESDecryptor();
  void LoadKey(CryptoPP::ByteQueue *kQueuePub, const char *szKeyPath);
  void CheckPublicKeyHash(CryptoPP::ByteQueue *kQueuePub);

public:
  unsigned __int8 m_byState;
  CryptoPP::AutoSeededRandomPool *m_prng;
  CryptoPP::DL_PrivateKey_EC<CryptoPP::ECP> m_PrivateKey;
  CryptoPP::DL_PublicKey_EC<CryptoPP::ECP> m_PublicKey;
  CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption m_AESDecryptor;
  CryptoPP::ECIES<CryptoPP::ECP>::Encryptor m_ECPEncryptor;
  CryptoPP::ECIES<CryptoPP::ECP>::Decryptor m_ECPDecryptor;
};


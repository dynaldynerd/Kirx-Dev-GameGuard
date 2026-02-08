#include "pch.h"

#include "CCryptParam.h"

#include <string>

#include <cryptlib.h>
#include <files.h>
#include <filters.h>
#include <queue.h>
#include <secblock.h>
#include <sha.h>

#include "GlobalObjects.h"

CCryptParam::CCryptParam(CryptoPP::AutoSeededRandomPool *prng)
  : m_byState(0),
    m_prng(prng),
    m_PrivateKey(),
    m_PublicKey(),
    m_AESDecryptor(),
    m_ECPEncryptor(),
    m_ECPDecryptor()
{
  Alloc();
  if (!m_prng)
  {
    throw std::string("m_prng NULL!");
  }
}

CCryptParam::~CCryptParam() = default;

void CCryptParam::Alloc()
{
  m_byState = 1;
}

unsigned __int64 CCryptParam::MaxPlaintextLength(unsigned __int64 ciphertextLength)
{
  if ((m_byState & 2) != 0 || (m_byState & 4) != 0)
  {
    return m_ECPEncryptor.MaxPlaintextLength(ciphertextLength);
  }
  return 0;
}

unsigned __int64 CCryptParam::CiphertextLength(unsigned __int64 tMaxUseSize)
{
  if ((m_byState & 2) != 0 || (m_byState & 4) != 0)
  {
    return m_ECPEncryptor.CiphertextLength(tMaxUseSize);
  }
  return 0;
}

bool CCryptParam::Encrypt(
  const unsigned __int8 *pText,
  unsigned __int64 tLength,
  unsigned __int8 *pCipherText,
  unsigned __int64 tCipherTextLength)
{
  if ((m_byState & 1) == 0)
  {
    return false;
  }

  const unsigned __int64 cipherLen = m_ECPEncryptor.CiphertextLength(tLength);
  if (cipherLen > tCipherTextLength)
  {
    return false;
  }

  m_ECPEncryptor.Encrypt(*m_prng, pText, tLength, pCipherText, CryptoPP::g_nullNameValuePairs);
  return true;
}

void CCryptParam::SetAESDecryptor()
{
  m_AESDecryptor.SetKeyWithIV(g_key, sizeof(g_key), g_iv);
}

void CCryptParam::LoadKey(CryptoPP::ByteQueue *kQueuePub, const char *szKeyPath)
{
  CryptoPP::ByteQueue queue(0x400u);
  CryptoPP::StreamTransformationFilter filter(
    m_AESDecryptor,
    &queue,
    CryptoPP::StreamTransformationFilter::DEFAULT_PADDING);
  CryptoPP::FileSource file(szKeyPath, true, &filter, true);

  m_PublicKey.BERDecode(queue);
  m_PublicKey.BEREncode(*kQueuePub);
  if (!m_PublicKey.Validate(*m_prng, 3))
  {
    throw std::string("Public Key Init Validation");
  }
}

void CCryptParam::CheckPublicKeyHash(CryptoPP::ByteQueue *kQueuePub)
{
  CryptoPP::SecByteBlock buffer(0x400u);
  const unsigned __int64 length = kQueuePub->Get(buffer, buffer.size());
  CryptoPP::SHA256 hash;
  if (!hash.VerifyDigest(g_cbHashVerify, buffer, static_cast<size_t>(length)))
  {
    throw std::string("VerifyDigest Failure");
  }
}

bool CCryptParam::Load(const char *szKeyPath)
{
  CryptoPP::ByteQueue queue(0x400u);
  SetAESDecryptor();
  LoadKey(&queue, szKeyPath);
  CheckPublicKeyHash(&queue);

  m_ECPEncryptor.AccessPublicKey().AssignFrom(m_PublicKey);
  m_ECPDecryptor.AccessPrivateKey().AssignFrom(m_PrivateKey);

  m_byState |= 2u;
  return true;
}


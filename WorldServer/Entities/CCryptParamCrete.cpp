#include "pch.h"

#include "CCryptParamCrete.h"

CCryptParamCrete::CCryptParamCrete(CryptoPP::AutoSeededRandomPool *prng)
  : CCryptParam(prng),
    m_vecPubKeyBuffer(),
    m_tPubKeyRealSize(0),
    m_AESEncryptor()
{
  m_vecPubKeyBuffer.assign(1024, 0);
  m_tPubKeyRealSize = 0;
}

CCryptParamCrete::~CCryptParamCrete() = default;


#pragma once

#include "IdaCompat.h"

#include <vector>

#include <aes.h>
#include <modes.h>

#include "CCryptParam.h"

class  CCryptParamCrete : public CCryptParam
{
public:
  explicit CCryptParamCrete(CryptoPP::AutoSeededRandomPool *prng);
  ~CCryptParamCrete() override;

  std::vector<unsigned char> m_vecPubKeyBuffer;
  unsigned __int64 m_tPubKeyRealSize;
  CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption m_AESEncryptor;
};


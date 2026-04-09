#pragma once

#include "VCryptor.h"

union VCAESParamBase
{
  VCryptorParam baseclass_0;
  AES_KEY encKey;
};

struct VCAESParam
{
  VCAESParam();

  void Clear();

  VCAESParamBase ___u0;
  AES_KEY decKey;
};

static_assert(sizeof(VCAESParam) == 488);

class VCAES : public VCryptor
{
public:
  VCAES();
  ~VCAES() override = default;

  bool Init(VCryptorParam *pParam) override;
  char *Encrypt(char *pText, unsigned int tLength, char *pCipherText, unsigned int tCipherTextLength) override;
  char *Decrypt(char *pCipherText, unsigned int tLength, char *pText, unsigned int tTextLength) override;
  VCryptorParam *Param() override;

public:
  VCAESParam m_Param;
};

static_assert(sizeof(VCAES) == 496);

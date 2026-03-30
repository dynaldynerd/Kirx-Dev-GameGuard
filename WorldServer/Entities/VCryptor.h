#pragma once

#include "IdaCompat.h"

struct VCryptorParam
{
  char dummy[1];
};

struct AES_KEY
{
  unsigned int rd_key[60];
  int rounds;
};

class VCryptor
{
public:
  virtual ~VCryptor() = default;

  virtual bool Init(VCryptorParam *pParam) = 0;
  virtual char *Encrypt(char *pText, unsigned int tLength, char *pCipherText, unsigned int tCipherTextLength) = 0;
  virtual char *Decrypt(char *pCipherText, unsigned int tLength, char *pText, unsigned int tTextLength) = 0;
  virtual VCryptorParam *Param() = 0;

  static VCryptor *Create(unsigned __int8 byCryptType, VCryptorParam *pParam);
};

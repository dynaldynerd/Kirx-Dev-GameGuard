#pragma once

#include "IdaCompat.h"

struct VCryptorParam;
class VCryptor;

class VCrypt
{
public:
  VCrypt();
  ~VCrypt();

  VCryptor *Init(unsigned __int8 vcType, VCryptorParam *pParam);
  char *Encrypt(char *ptrPlain, unsigned int uiPlainSize, char *ptrEnc, unsigned int uiEncSize);
  char *Decrypt(char *ptrEnc, unsigned int uiEncSize, char *ptrDec, unsigned int uiDecSize);
  void Clear();
  VCryptor *Alloc(unsigned __int8 vcType, VCryptorParam *pParam);

  static VCrypt *GetInstance();

  VCryptor *m_pVCryptor;

  static VCrypt *ms_instance;
};

static_assert(sizeof(VCrypt) == 8);

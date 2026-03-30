#include "pch.h"

#include "VCryptor.h"

#include <new>

#include "VCAES.h"

VCryptor *VCryptor::Create(unsigned __int8 byCryptType, VCryptorParam *pParam)
{
  if (byCryptType != 0 || !pParam)
  {
    return nullptr;
  }

  VCAES *cryptor = new (std::nothrow) VCAES();
  if (!cryptor)
  {
    return nullptr;
  }

  if (!cryptor->Init(pParam))
  {
    delete cryptor;
    return nullptr;
  }

  return cryptor;
}

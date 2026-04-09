#include "pch.h"

#include "VCrypt.h"

#include <new>

#include "VCAES.h"

VCrypt *VCrypt::ms_instance = nullptr;

VCrypt::VCrypt()
  : m_pVCryptor(nullptr)
{
}

VCrypt::~VCrypt()
{
  Clear();
}

VCryptor *VCrypt::Init(unsigned __int8 vcType, VCryptorParam *pParam)
{
  Clear();
  m_pVCryptor = Alloc(vcType, pParam);
  return m_pVCryptor;
}

char *VCrypt::Encrypt(char *ptrPlain, unsigned int uiPlainSize, char *ptrEnc, unsigned int uiEncSize)
{
  if (m_pVCryptor)
  {
    return m_pVCryptor->Encrypt(ptrPlain, uiPlainSize, ptrEnc, uiEncSize);
  }

  return nullptr;
}

char *VCrypt::Decrypt(char *ptrEnc, unsigned int uiEncSize, char *ptrDec, unsigned int uiDecSize)
{
  if (m_pVCryptor)
  {
    return m_pVCryptor->Decrypt(ptrEnc, uiEncSize, ptrDec, uiDecSize);
  }

  return nullptr;
}

void VCrypt::Clear()
{
  if (m_pVCryptor)
  {
    operator delete(m_pVCryptor);
    m_pVCryptor = nullptr;
  }
}

VCryptor *VCrypt::Alloc(unsigned __int8 vcType, VCryptorParam *pParam)
{
  if (vcType || !pParam)
  {
    return nullptr;
  }

  VCAES *cryptor = new (std::nothrow) VCAES();
  if (!cryptor)
  {
    return nullptr;
  }

  if (cryptor->Init(pParam))
  {
    return cryptor;
  }

  operator delete(cryptor);
  return nullptr;
}

VCrypt *VCrypt::GetInstance()
{
  if (!ms_instance)
  {
    ms_instance = new (std::nothrow) VCrypt();
  }

  return ms_instance;
}

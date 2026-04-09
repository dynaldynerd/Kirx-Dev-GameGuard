#include "pch.h"

#include "VCryptor.h"

#include <cstring>

void AES_KEY::Set(int rounds_, unsigned int *rd_key_)
{
  if (!rd_key_)
  {
    return;
  }

  rounds = rounds_;
  std::memcpy(rd_key, rd_key_, sizeof(rd_key));
}

#include "pch.h"

#include "VCAES.h"

#include <array>
#include <cstdint>
#include <cstring>

namespace
{
  constexpr unsigned int kAesBlockSize = 16;
  using Byte = std::uint8_t;
  using Word = std::uint32_t;

  struct AesTables
  {
    std::array<Word, 256> te0{};
    std::array<Word, 256> te1{};
    std::array<Word, 256> te2{};
    std::array<Word, 256> te3{};
    std::array<Word, 256> td0{};
    std::array<Word, 256> td1{};
    std::array<Word, 256> td2{};
    std::array<Word, 256> td3{};
    std::array<Byte, 256> invSbox{};
  };

  Byte RotateLeft8(Byte value, unsigned int shift)
  {
    return static_cast<Byte>((value << shift) | (value >> (8 - shift)));
  }

  Word RotateLeft32(Word value, unsigned int shift)
  {
    return (value << shift) | (value >> (32 - shift));
  }

  Word RotateRight32(Word value, unsigned int shift)
  {
    return (value >> shift) | (value << (32 - shift));
  }

  Word ByteSwap32(Word value)
  {
    return (RotateLeft32(value, 8) & 0x00FF00FFU) | (RotateRight32(value, 8) & 0xFF00FF00U);
  }

  Byte MultiplyGF(Byte lhs, Byte rhs)
  {
    Byte result = 0;
    Byte value = lhs;
    Byte factor = rhs;
    while (factor)
    {
      if (factor & 1U)
      {
        result ^= value;
      }

      const bool highBit = (value & 0x80U) != 0;
      value = static_cast<Byte>(value << 1);
      if (highBit)
      {
        value ^= 0x1BU;
      }

      factor >>= 1;
    }

    return result;
  }

  Byte MultiplicativeInverse(Byte value)
  {
    if (!value)
    {
      return 0;
    }

    Byte result = 1;
    Byte base = value;
    unsigned int exponent = 254;
    while (exponent)
    {
      if (exponent & 1U)
      {
        result = MultiplyGF(result, base);
      }

      base = MultiplyGF(base, base);
      exponent >>= 1;
    }

    return result;
  }

  AesTables BuildAesTables()
  {
    AesTables tables{};

    for (unsigned int index = 0; index < 256; ++index)
    {
      const Byte inverse = MultiplicativeInverse(static_cast<Byte>(index));
      const Byte subByte = static_cast<Byte>(
        0x63U ^ inverse ^ RotateLeft8(inverse, 1) ^ RotateLeft8(inverse, 2) ^ RotateLeft8(inverse, 3)
        ^ RotateLeft8(inverse, 4));

      tables.invSbox[subByte] = static_cast<Byte>(index);

      const Byte mul2 = MultiplyGF(subByte, 0x02U);
      const Byte mul3 = static_cast<Byte>(mul2 ^ subByte);

      tables.te0[index] = (static_cast<Word>(mul2) << 24) | (static_cast<Word>(subByte) << 16)
        | (static_cast<Word>(subByte) << 8) | static_cast<Word>(mul3);
      tables.te1[index] = (static_cast<Word>(mul3) << 24) | (static_cast<Word>(mul2) << 16)
        | (static_cast<Word>(subByte) << 8) | static_cast<Word>(subByte);
      tables.te2[index] = (static_cast<Word>(subByte) << 24) | (static_cast<Word>(mul3) << 16)
        | (static_cast<Word>(mul2) << 8) | static_cast<Word>(subByte);
      tables.te3[index] = (static_cast<Word>(subByte) << 24) | (static_cast<Word>(subByte) << 16)
        | (static_cast<Word>(mul3) << 8) | static_cast<Word>(mul2);
    }

    for (unsigned int index = 0; index < 256; ++index)
    {
      const Byte inverseSubByte = tables.invSbox[index];
      const Byte mul9 = MultiplyGF(inverseSubByte, 0x09U);
      const Byte mul11 = MultiplyGF(inverseSubByte, 0x0BU);
      const Byte mul13 = MultiplyGF(inverseSubByte, 0x0DU);
      const Byte mul14 = MultiplyGF(inverseSubByte, 0x0EU);

      tables.td0[index] = (static_cast<Word>(mul14) << 24) | (static_cast<Word>(mul9) << 16)
        | (static_cast<Word>(mul13) << 8) | static_cast<Word>(mul11);
      tables.td1[index] = (static_cast<Word>(mul11) << 24) | (static_cast<Word>(mul14) << 16)
        | (static_cast<Word>(mul9) << 8) | static_cast<Word>(mul13);
      tables.td2[index] = (static_cast<Word>(mul13) << 24) | (static_cast<Word>(mul11) << 16)
        | (static_cast<Word>(mul14) << 8) | static_cast<Word>(mul9);
      tables.td3[index] = (static_cast<Word>(mul9) << 24) | (static_cast<Word>(mul13) << 16)
        | (static_cast<Word>(mul11) << 8) | static_cast<Word>(mul14);
    }

    return tables;
  }

  const AesTables &GetAesTables()
  {
    static const AesTables kTables = BuildAesTables();
    return kTables;
  }

  Word LoadStateWord(const Byte *source)
  {
    Word value = 0;
    std::memcpy(&value, source, sizeof(value));
    return ByteSwap32(value);
  }

  void StoreStateWord(Byte *destination, Word value)
  {
    const Word swapped = ByteSwap32(value);
    std::memcpy(destination, &swapped, sizeof(swapped));
  }

  void CopyAesKey(AES_KEY &destination, const AES_KEY &source)
  {
    std::memcpy(destination.rd_key, source.rd_key, sizeof(destination.rd_key));
    destination.rounds = source.rounds;
  }

  void EncryptBlock(const Byte *source, Byte *destination, const AES_KEY &key)
  {
    const AesTables &tables = GetAesTables();
    const Word *roundKey = key.rd_key;

    Word state0 = roundKey[0] ^ LoadStateWord(source);
    Word state1 = roundKey[1] ^ LoadStateWord(source + 4);
    Word state2 = roundKey[2] ^ LoadStateWord(source + 8);
    Word state3 = roundKey[3] ^ LoadStateWord(source + 12);

    Word mixed0 = 0;
    Word mixed1 = 0;
    Word mixed2 = 0;
    Word mixed3 = 0;
    int roundPairs = key.rounds >> 1;
    while (true)
    {
      mixed0 = roundKey[4] ^ tables.te3[state3 & 0xFFU] ^ tables.te2[(state2 >> 8) & 0xFFU]
        ^ tables.te1[(state1 >> 16) & 0xFFU] ^ tables.te0[state0 >> 24];
      mixed1 = roundKey[5] ^ tables.te3[state0 & 0xFFU] ^ tables.te2[(state3 >> 8) & 0xFFU]
        ^ tables.te1[(state2 >> 16) & 0xFFU] ^ tables.te0[state1 >> 24];
      mixed2 = roundKey[6] ^ tables.te3[state1 & 0xFFU] ^ tables.te2[(state0 >> 8) & 0xFFU]
        ^ tables.te1[(state3 >> 16) & 0xFFU] ^ tables.te0[state2 >> 24];
      mixed3 = roundKey[7] ^ tables.te3[state2 & 0xFFU] ^ tables.te2[(state1 >> 8) & 0xFFU]
        ^ tables.te1[(state0 >> 16) & 0xFFU] ^ tables.te0[state3 >> 24];

      roundKey += 8;
      if (!--roundPairs)
      {
        break;
      }

      state0 = roundKey[0] ^ tables.te3[mixed3 & 0xFFU] ^ tables.te2[(mixed2 >> 8) & 0xFFU]
        ^ tables.te1[(mixed1 >> 16) & 0xFFU] ^ tables.te0[mixed0 >> 24];
      state1 = roundKey[1] ^ tables.te3[mixed0 & 0xFFU] ^ tables.te2[(mixed3 >> 8) & 0xFFU]
        ^ tables.te1[(mixed2 >> 16) & 0xFFU] ^ tables.te0[mixed1 >> 24];
      state2 = roundKey[2] ^ tables.te3[mixed1 & 0xFFU] ^ tables.te2[(mixed0 >> 8) & 0xFFU]
        ^ tables.te1[(mixed3 >> 16) & 0xFFU] ^ tables.te0[mixed2 >> 24];
      state3 = roundKey[3] ^ tables.te3[mixed2 & 0xFFU] ^ tables.te2[(mixed1 >> 8) & 0xFFU]
        ^ tables.te1[(mixed0 >> 16) & 0xFFU] ^ tables.te0[mixed3 >> 24];
    }

    const Word output0 = roundKey[0] ^ (tables.te1[mixed3 & 0xFFU] & 0x000000FFU)
      ^ (tables.te0[(mixed2 >> 8) & 0xFFU] & 0x0000FF00U) ^ (tables.te3[(mixed1 >> 16) & 0xFFU] & 0x00FF0000U)
      ^ (tables.te2[mixed0 >> 24] & 0xFF000000U);
    const Word output1 = roundKey[1] ^ (tables.te1[mixed0 & 0xFFU] & 0x000000FFU)
      ^ (tables.te0[(mixed3 >> 8) & 0xFFU] & 0x0000FF00U) ^ (tables.te3[(mixed2 >> 16) & 0xFFU] & 0x00FF0000U)
      ^ (tables.te2[mixed1 >> 24] & 0xFF000000U);
    const Word output2 = roundKey[2] ^ (tables.te1[mixed1 & 0xFFU] & 0x000000FFU)
      ^ (tables.te0[(mixed0 >> 8) & 0xFFU] & 0x0000FF00U) ^ (tables.te3[(mixed3 >> 16) & 0xFFU] & 0x00FF0000U)
      ^ (tables.te2[mixed2 >> 24] & 0xFF000000U);
    const Word output3 = roundKey[3] ^ (tables.te1[mixed2 & 0xFFU] & 0x000000FFU)
      ^ (tables.te0[(mixed1 >> 8) & 0xFFU] & 0x0000FF00U) ^ (tables.te3[(mixed0 >> 16) & 0xFFU] & 0x00FF0000U)
      ^ (tables.te2[mixed3 >> 24] & 0xFF000000U);

    StoreStateWord(destination, output0);
    StoreStateWord(destination + 4, output1);
    StoreStateWord(destination + 8, output2);
    StoreStateWord(destination + 12, output3);
  }

  void DecryptBlock(const Byte *source, Byte *destination, const AES_KEY &key)
  {
    const AesTables &tables = GetAesTables();
    const Word *roundKey = key.rd_key;

    Word state0 = roundKey[0] ^ LoadStateWord(source);
    Word state1 = roundKey[1] ^ LoadStateWord(source + 4);
    Word state2 = roundKey[2] ^ LoadStateWord(source + 8);
    Word state3 = roundKey[3] ^ LoadStateWord(source + 12);

    Word mixed0 = 0;
    Word mixed1 = 0;
    Word mixed2 = 0;
    Word mixed3 = 0;
    int roundPairs = key.rounds >> 1;
    while (true)
    {
      mixed0 = roundKey[4] ^ tables.td3[state1 & 0xFFU] ^ tables.td2[(state2 >> 8) & 0xFFU]
        ^ tables.td1[(state3 >> 16) & 0xFFU] ^ tables.td0[state0 >> 24];
      mixed1 = roundKey[5] ^ tables.td3[state2 & 0xFFU] ^ tables.td2[(state3 >> 8) & 0xFFU]
        ^ tables.td1[(state0 >> 16) & 0xFFU] ^ tables.td0[state1 >> 24];
      mixed2 = roundKey[6] ^ tables.td3[state3 & 0xFFU] ^ tables.td2[(state0 >> 8) & 0xFFU]
        ^ tables.td1[(state1 >> 16) & 0xFFU] ^ tables.td0[state2 >> 24];
      mixed3 = roundKey[7] ^ tables.td3[state0 & 0xFFU] ^ tables.td2[(state1 >> 8) & 0xFFU]
        ^ tables.td1[(state2 >> 16) & 0xFFU] ^ tables.td0[state3 >> 24];

      roundKey += 8;
      if (!--roundPairs)
      {
        break;
      }

      state0 = roundKey[0] ^ tables.td3[mixed1 & 0xFFU] ^ tables.td2[(mixed2 >> 8) & 0xFFU]
        ^ tables.td1[(mixed3 >> 16) & 0xFFU] ^ tables.td0[mixed0 >> 24];
      state1 = roundKey[1] ^ tables.td3[mixed2 & 0xFFU] ^ tables.td2[(mixed3 >> 8) & 0xFFU]
        ^ tables.td1[(mixed0 >> 16) & 0xFFU] ^ tables.td0[mixed1 >> 24];
      state2 = roundKey[2] ^ tables.td3[mixed3 & 0xFFU] ^ tables.td2[(mixed0 >> 8) & 0xFFU]
        ^ tables.td1[(mixed1 >> 16) & 0xFFU] ^ tables.td0[mixed2 >> 24];
      state3 = roundKey[3] ^ tables.td3[mixed0 & 0xFFU] ^ tables.td2[(mixed1 >> 8) & 0xFFU]
        ^ tables.td1[(mixed2 >> 16) & 0xFFU] ^ tables.td0[mixed3 >> 24];
    }

    const Word output0 = roundKey[0] ^ static_cast<Word>(tables.invSbox[mixed1 & 0xFFU])
      ^ (static_cast<Word>(tables.invSbox[(mixed2 >> 8) & 0xFFU]) << 8)
      ^ (static_cast<Word>(tables.invSbox[(mixed3 >> 16) & 0xFFU]) << 16)
      ^ (static_cast<Word>(tables.invSbox[mixed0 >> 24]) << 24);
    const Word output1 = roundKey[1] ^ static_cast<Word>(tables.invSbox[mixed2 & 0xFFU])
      ^ (static_cast<Word>(tables.invSbox[(mixed3 >> 8) & 0xFFU]) << 8)
      ^ (static_cast<Word>(tables.invSbox[(mixed0 >> 16) & 0xFFU]) << 16)
      ^ (static_cast<Word>(tables.invSbox[mixed1 >> 24]) << 24);
    const Word output2 = roundKey[2] ^ static_cast<Word>(tables.invSbox[mixed3 & 0xFFU])
      ^ (static_cast<Word>(tables.invSbox[(mixed0 >> 8) & 0xFFU]) << 8)
      ^ (static_cast<Word>(tables.invSbox[(mixed1 >> 16) & 0xFFU]) << 16)
      ^ (static_cast<Word>(tables.invSbox[mixed2 >> 24]) << 24);
    const Word output3 = roundKey[3] ^ static_cast<Word>(tables.invSbox[mixed0 & 0xFFU])
      ^ (static_cast<Word>(tables.invSbox[(mixed1 >> 8) & 0xFFU]) << 8)
      ^ (static_cast<Word>(tables.invSbox[(mixed2 >> 16) & 0xFFU]) << 16)
      ^ (static_cast<Word>(tables.invSbox[mixed3 >> 24]) << 24);

    StoreStateWord(destination, output0);
    StoreStateWord(destination + 4, output1);
    StoreStateWord(destination + 8, output2);
    StoreStateWord(destination + 12, output3);
  }
}

VCAESParam::VCAESParam()
{
  Clear();
}

void VCAESParam::Clear()
{
  std::memset(this, 0, sizeof(*this));
}

VCAES::VCAES()
  : m_Param()
{
}

bool VCAES::Init(VCryptorParam *pParam)
{
  if (!pParam)
  {
    return false;
  }

  const VCAESParam *param = reinterpret_cast<const VCAESParam *>(pParam);
  CopyAesKey(m_Param.___u0.encKey, param->___u0.encKey);
  CopyAesKey(m_Param.decKey, param->decKey);
  return true;
}

char *VCAES::Encrypt(char *pText, unsigned int tLength, char *pCipherText, unsigned int tCipherTextLength)
{
  if (!tLength || !pText || !pCipherText || tLength > tCipherTextLength)
  {
    return nullptr;
  }

  std::array<Byte, kAesBlockSize> previousCipherBlock{};
  std::size_t remaining = tLength;
  const Byte *source = reinterpret_cast<const Byte *>(pText);
  Byte *destination = reinterpret_cast<Byte *>(pCipherText);
  const Byte *xorSource = previousCipherBlock.data();

  while (remaining >= kAesBlockSize)
  {
    for (unsigned int index = 0; index < kAesBlockSize; ++index)
    {
      destination[index] = static_cast<Byte>(xorSource[index] ^ source[index]);
    }

    EncryptBlock(destination, destination, m_Param.___u0.encKey);
    xorSource = destination;

    remaining -= kAesBlockSize;
    source += kAesBlockSize;
    destination += kAesBlockSize;
  }

  while (remaining)
  {
    unsigned int index = 0;
    for (; index < kAesBlockSize && index < remaining; ++index)
    {
      destination[index] = static_cast<Byte>(xorSource[index] ^ source[index]);
    }

    while (index < kAesBlockSize)
    {
      destination[index] = xorSource[index];
      ++index;
    }

    EncryptBlock(destination, destination, m_Param.___u0.encKey);
    xorSource = destination;

    if (remaining <= kAesBlockSize)
    {
      break;
    }

    remaining -= kAesBlockSize;
    source += kAesBlockSize;
    destination += kAesBlockSize;
  }

  return pCipherText;
}

char *VCAES::Decrypt(char *pCipherText, unsigned int tLength, char *pText, unsigned int tTextLength)
{
  if (!tLength || !pCipherText || !pText || tLength > tTextLength)
  {
    return nullptr;
  }

  std::array<Byte, kAesBlockSize> previousCipherBlock{};
  std::array<Byte, kAesBlockSize> tempBlock{};
  std::size_t remaining = tLength;
  const Byte *source = reinterpret_cast<const Byte *>(pCipherText);
  Byte *destination = reinterpret_cast<Byte *>(pText);
  const AES_KEY &decryptKey = m_Param.decKey;

  if (pCipherText == pText)
  {
    while (remaining >= kAesBlockSize)
    {
      DecryptBlock(source, tempBlock.data(), decryptKey);
      for (unsigned int index = 0; index < kAesBlockSize; ++index)
      {
        const Byte cipherByte = source[index];
        destination[index] = static_cast<Byte>(previousCipherBlock[index] ^ tempBlock[index]);
        previousCipherBlock[index] = cipherByte;
      }

      remaining -= kAesBlockSize;
      source += kAesBlockSize;
      destination += kAesBlockSize;
    }
  }
  else
  {
    const Byte *previousSource = previousCipherBlock.data();
    while (remaining >= kAesBlockSize)
    {
      DecryptBlock(source, destination, decryptKey);
      for (unsigned int index = 0; index < kAesBlockSize; ++index)
      {
        destination[index] ^= previousSource[index];
      }

      previousSource = source;
      remaining -= kAesBlockSize;
      source += kAesBlockSize;
      destination += kAesBlockSize;
    }

    std::memcpy(previousCipherBlock.data(), previousSource, kAesBlockSize);
  }

  while (remaining)
  {
    DecryptBlock(source, tempBlock.data(), decryptKey);

    unsigned int index = 0;
    for (; index < kAesBlockSize && index < remaining; ++index)
    {
      const Byte cipherByte = source[index];
      destination[index] = static_cast<Byte>(previousCipherBlock[index] ^ tempBlock[index]);
      previousCipherBlock[index] = cipherByte;
    }

    if (remaining <= kAesBlockSize)
    {
      while (index < kAesBlockSize)
      {
        previousCipherBlock[index] = source[index];
        ++index;
      }
      return pText;
    }

    remaining -= kAesBlockSize;
    source += kAesBlockSize;
    destination += kAesBlockSize;
  }

  return pText;
}

VCryptorParam *VCAES::Param()
{
  return &m_Param.___u0.baseclass_0;
}

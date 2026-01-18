using System;
using System.Security.Cryptography;
using System.Text;
using Konscious.Security.Cryptography;

namespace LoginServer.Security;

public static class CryptoHelper
{
    public const int AesGcmNonceSize = 12;
    public const int AesGcmTagSize = 16;

    public static byte[] GenerateRandomBytes(int length)
    {
        return RandomNumberGenerator.GetBytes(length);
    }

    public static byte[] ComputeHmacSha256(ReadOnlySpan<byte> data, ReadOnlySpan<byte> key)
    {
        using var hmac = new HMACSHA256(key.ToArray());
        return hmac.ComputeHash(data.ToArray());
    }

    public static byte[] EncryptAesGcm(ReadOnlySpan<byte> key, ReadOnlySpan<byte> plaintext, ReadOnlySpan<byte> associatedData = default)
    {
        var nonce = GenerateRandomBytes(AesGcmNonceSize);
        var ciphertext = new byte[plaintext.Length];
        var tag = new byte[AesGcmTagSize];

        using var aes = new AesGcm(key, AesGcmTagSize);
        aes.Encrypt(nonce, plaintext, ciphertext, tag, associatedData);

        var payload = new byte[AesGcmNonceSize + AesGcmTagSize + ciphertext.Length];
        Buffer.BlockCopy(nonce, 0, payload, 0, nonce.Length);
        Buffer.BlockCopy(tag, 0, payload, nonce.Length, tag.Length);
        Buffer.BlockCopy(ciphertext, 0, payload, nonce.Length + tag.Length, ciphertext.Length);
        return payload;
    }

    public static bool TryDecryptAesGcm(ReadOnlySpan<byte> key, ReadOnlySpan<byte> payload, out byte[] plaintext, ReadOnlySpan<byte> associatedData = default)
    {
        try
        {
            plaintext = DecryptAesGcm(key, payload, associatedData);
            return true;
        }
        catch (CryptographicException)
        {
            plaintext = Array.Empty<byte>();
            return false;
        }
    }

    public static byte[] DecryptAesGcm(ReadOnlySpan<byte> key, ReadOnlySpan<byte> payload, ReadOnlySpan<byte> associatedData = default)
    {
        if (payload.Length < AesGcmNonceSize + AesGcmTagSize)
        {
            throw new ArgumentException("Payload too small.", nameof(payload));
        }

        var nonce = payload.Slice(0, AesGcmNonceSize);
        var tag = payload.Slice(AesGcmNonceSize, AesGcmTagSize);
        var ciphertext = payload.Slice(AesGcmNonceSize + AesGcmTagSize);
        var plaintext = new byte[ciphertext.Length];

        using var aes = new AesGcm(key, AesGcmTagSize);
        aes.Decrypt(nonce, ciphertext, tag, plaintext, associatedData);
        return plaintext;
    }

    public static byte[] HashArgon2id(ReadOnlySpan<byte> password, ReadOnlySpan<byte> salt, Argon2Parameters? parameters = null)
    {
        var cfg = parameters ?? Argon2Parameters.Default;
        using var argon2 = new Argon2id(password.ToArray())
        {
            Salt = salt.ToArray(),
            DegreeOfParallelism = cfg.Parallelism,
            MemorySize = cfg.MemoryKb,
            Iterations = cfg.Iterations
        };
        return argon2.GetBytes(cfg.HashLength);
    }

    public static bool VerifyArgon2id(ReadOnlySpan<byte> password, ReadOnlySpan<byte> salt, ReadOnlySpan<byte> expected, Argon2Parameters? parameters = null)
    {
        var hash = HashArgon2id(password, salt, parameters);
        return CryptographicOperations.FixedTimeEquals(hash, expected);
    }

    public static string ComputeHmacSha256Base64(string data, ReadOnlySpan<byte> key)
    {
        var hash = ComputeHmacSha256(Encoding.UTF8.GetBytes(data), key);
        return Convert.ToBase64String(hash);
    }
}

public sealed class Argon2Parameters
{
    public static Argon2Parameters Default { get; } = new();

    public int MemoryKb { get; set; } = 65536;
    public int Iterations { get; set; } = 3;
    public int Parallelism { get; set; } = 1;
    public int HashLength { get; set; } = 32;
}

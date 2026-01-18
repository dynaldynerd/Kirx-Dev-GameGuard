using System;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace LoginServer.Data.Entities;

[Table("tbl_rfaccount")]
public sealed class Account
{
    [Key]
    [Column("id_hmac")]
    public byte[] IdHmac { get; set; } = Array.Empty<byte>();

    [Column("id_enc")]
    public byte[] IdEnc { get; set; } = Array.Empty<byte>();

    [Column("password_hash")]
    public string PasswordHash { get; set; } = string.Empty;

    [Column("accounttype")]
    public byte AccountType { get; set; }
}

using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace LoginServer.Data.Entities;

[Table("tbl_rfaccount")]
public sealed class Account
{
    [Key]
    [Column("id")]
    [MaxLength(13)]
    public string Id { get; set; } = string.Empty;

    [Column("password")]
    [MaxLength(32)]
    public string Password { get; set; } = string.Empty;
}

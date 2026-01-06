using System;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace LoginServer.Data.Entities;

[Table("tbl_UserStatus")]
public sealed class BillingUserStatus
{
    [Key]
    [Column("id")]
    [MaxLength(16)]
    public string Id { get; set; } = string.Empty;

    [Column("Status")]
    public int Status { get; set; }

    [Column("DTEndPrem")]
    public DateTime? DtEndPrem { get; set; }
}

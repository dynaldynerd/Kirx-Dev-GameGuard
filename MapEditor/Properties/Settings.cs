using System.Configuration;

namespace MapEditor.Properties;

internal sealed class Settings : ApplicationSettingsBase
{
  private static readonly Settings s_default = (Settings)Synchronized(new Settings());

  public static Settings Default => s_default;

  [UserScopedSetting]
  [DefaultSettingValue("{}")]
  public string LastMapCameraByPathJson
  {
    get => (string)(this[nameof(LastMapCameraByPathJson)] ?? "{}");
    set => this[nameof(LastMapCameraByPathJson)] = value;
  }
}

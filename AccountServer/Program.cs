using AccountServer.Settings;
using AccountServer.UI;
using System.Windows.Forms;

namespace AccountServer;

internal static class Program
{
    [STAThread]
    static void Main()
    {
        ApplicationConfiguration.Initialize();
        if (!Properties.Settings.Default.IsSetup)
        {
            using var setup = new Setup();
            if (setup.ShowDialog() != DialogResult.OK)
            {
                return;
            }
        }

        var settings = AppSettings.Load();
        if (!settings.Security.HasValidArgon2Salt())
        {
            MessageBox.Show(
                "AccountServer is missing its persisted Argon2 salt.\n\n" +
                "Do not generate a new salt for an existing RF_User database. Restore the original setting or rebuild the account data first.",
                "Account Server Setup",
                MessageBoxButtons.OK,
                MessageBoxIcon.Error);
            return;
        }

        Application.Run(new MainForm(settings));
    }
}

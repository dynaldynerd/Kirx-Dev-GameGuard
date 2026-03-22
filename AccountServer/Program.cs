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

        while (true)
        {
            if (!Properties.Settings.Default.IsSetup)
            {
                using var setup = new Setup();
                if (setup.ShowDialog() != DialogResult.OK)
                {
                    return;
                }
            }

            var settings = AppSettings.Load();
            if (settings.Security.HasValidArgon2Salt())
            {
                Application.Run(new MainForm(settings));
                return;
            }

            var result = MessageBox.Show(
                "AccountServer is missing its persisted Argon2 salt.\n\n" +
                "Select Yes to reinstall setup for this runtime folder.\n" +
                "Select No to close.\n\n" +
                "Do not generate a new salt for an existing RF_User database unless you are rebuilding the account data.",
                "Account Server Setup",
                MessageBoxButtons.YesNo,
                MessageBoxIcon.Error,
                MessageBoxDefaultButton.Button1);
            if (result != DialogResult.Yes)
            {
                return;
            }

            Properties.Settings.Default.IsSetup = false;
            Properties.Settings.Default.Save();
        }
    }
}

using LoginServer.Settings;

namespace LoginServer;

static class Program
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

            if (TryLoadPersistedSettings(out string errorMessage))
            {
                Application.Run(new MainWindow());
                return;
            }

            var result = MessageBox.Show(
                errorMessage +
                "\n\nSelect Yes to reinstall setup for this runtime folder.\n" +
                "Select No to close.",
                "Login Server Setup",
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

    private static bool TryLoadPersistedSettings(out string errorMessage)
    {
        if (!File.Exists(AppSettings.DefaultPath))
        {
            errorMessage = $"LoginServer is missing its persisted settings file:\n{AppSettings.DefaultPath}";
            return false;
        }

        try
        {
            _ = AppSettings.Load(AppSettings.DefaultPath);
            errorMessage = string.Empty;
            return true;
        }
        catch (Exception ex)
        {
            errorMessage =
                $"LoginServer could not load its persisted settings file:\n{AppSettings.DefaultPath}\n\n" +
                ex.Message;
            return false;
        }
    }
}

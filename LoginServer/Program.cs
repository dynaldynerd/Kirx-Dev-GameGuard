namespace LoginServer;

static class Program
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

        Application.Run(new MainWindow());
    }
}

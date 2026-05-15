#include "pch.h"

#include "DatabaseSetupDlg.h"

#include <cstdlib>
#include <cstring>

namespace
{
constexpr char kDefaultBillingServer[] = "(local)";
constexpr char kDefaultBillingDbName[] = "BILLING";
constexpr char kDefaultWorldTrustedConnection[] = "1";
constexpr char kDefaultBillingTrustedConnection[] = "1";
constexpr char kDefaultUserId[] = "sa";
constexpr char kDefaultAccountAddress[] = "127.0.0.1";
constexpr char kDefaultAccountPort[] = "29000";
constexpr char kDefaultGatePort[] = "27780";
constexpr char kDefaultPortMode[] = "0";
constexpr char kInstalledSection[] = "System";
constexpr char kInstalledKey[] = "installed";
constexpr char kAutoStartKey[] = "autostart";

CStringA BuildPathFromCurrentDirectory(const char *relativePath)
{
  char currentDirectory[MAX_PATH]{};
  if (!GetCurrentDirectoryA(static_cast<DWORD>(sizeof(currentDirectory)), currentDirectory))
  {
    return CStringA();
  }

  CStringA path;
  path.Format("%s\\%s", currentDirectory, relativePath);
  return path;
}

bool FileExists(const CStringA &path)
{
  if (path.IsEmpty())
  {
    return false;
  }

  const DWORD attributes = GetFileAttributesA(path);
  return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

void ReadIniString(
  const CStringA &iniPath,
  const char *section,
  const char *key,
  const char *defaultValue,
  char *buffer,
  DWORD bufferSize)
{
  if (buffer == nullptr || bufferSize == 0)
  {
    return;
  }

  if (iniPath.IsEmpty())
  {
    buffer[0] = '\0';
    return;
  }

  GetPrivateProfileStringA(section, key, defaultValue, buffer, bufferSize, iniPath);
}

}

IMPLEMENT_DYNAMIC(CDatabaseSetupDlg, CDialog)

BEGIN_MESSAGE_MAP(CDatabaseSetupDlg, CDialog)
  ON_BN_CLICKED(IDC_DB_SETUP_WORLD_TRUSTED, &CDatabaseSetupDlg::OnBnClickedWorldTrustedConnection)
  ON_BN_CLICKED(IDC_DB_SETUP_BILLING_TRUSTED, &CDatabaseSetupDlg::OnBnClickedBillingTrustedConnection)
END_MESSAGE_MAP()

CDatabaseSetupDlg::CDatabaseSetupDlg(CWnd *pParent)
  : CDialog(IDD_DB_SETUP, pParent),
    m_worldTrustedConnectionCheck(),
    m_worldUserEdit(),
    m_worldPasswordEdit(),
    m_billingServerEdit(),
    m_billingDbEdit(),
    m_billingTrustedConnectionCheck(),
    m_billingUserEdit(),
    m_billingPasswordEdit(),
    m_accountAddressEdit(),
    m_accountPortEdit(),
    m_gateIpEdit(),
    m_gatePortEdit(),
    m_worldUserId(),
    m_worldPassword(),
    m_billingServerAddress(),
    m_billingDbName(),
    m_billingUserId(),
    m_billingPassword(),
    m_accountAddress(),
    m_accountPort(),
    m_gateIp(),
    m_gatePort(),
    m_worldTrustedConnection(TRUE),
    m_billingTrustedConnection(TRUE)
{
}

CDatabaseSetupDlg::~CDatabaseSetupDlg()
{
  // this is not a stub
}

void CDatabaseSetupDlg::DoDataExchange(CDataExchange *pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_DB_SETUP_WORLD_TRUSTED, m_worldTrustedConnectionCheck);
  DDX_Control(pDX, IDC_DB_SETUP_WORLD_USER, m_worldUserEdit);
  DDX_Control(pDX, IDC_DB_SETUP_WORLD_PASSWORD, m_worldPasswordEdit);
  DDX_Control(pDX, IDC_DB_SETUP_BILLING_SERVER, m_billingServerEdit);
  DDX_Control(pDX, IDC_DB_SETUP_BILLING_DB, m_billingDbEdit);
  DDX_Control(pDX, IDC_DB_SETUP_BILLING_TRUSTED, m_billingTrustedConnectionCheck);
  DDX_Control(pDX, IDC_DB_SETUP_BILLING_USER, m_billingUserEdit);
  DDX_Control(pDX, IDC_DB_SETUP_BILLING_PASSWORD, m_billingPasswordEdit);
  DDX_Control(pDX, IDC_DB_SETUP_ACCOUNT_ADDRESS, m_accountAddressEdit);
  DDX_Control(pDX, IDC_DB_SETUP_ACCOUNT_PORT, m_accountPortEdit);
  DDX_Control(pDX, IDC_DB_SETUP_GATE_IP, m_gateIpEdit);
  DDX_Control(pDX, IDC_DB_SETUP_GATE_PORT, m_gatePortEdit);
  DDX_Text(pDX, IDC_DB_SETUP_WORLD_USER, m_worldUserId);
  DDX_Text(pDX, IDC_DB_SETUP_WORLD_PASSWORD, m_worldPassword);
  DDX_Text(pDX, IDC_DB_SETUP_BILLING_SERVER, m_billingServerAddress);
  DDX_Text(pDX, IDC_DB_SETUP_BILLING_DB, m_billingDbName);
  DDX_Text(pDX, IDC_DB_SETUP_BILLING_USER, m_billingUserId);
  DDX_Text(pDX, IDC_DB_SETUP_BILLING_PASSWORD, m_billingPassword);
  DDX_Text(pDX, IDC_DB_SETUP_ACCOUNT_ADDRESS, m_accountAddress);
  DDX_Text(pDX, IDC_DB_SETUP_ACCOUNT_PORT, m_accountPort);
  DDX_Text(pDX, IDC_DB_SETUP_GATE_IP, m_gateIp);
  DDX_Text(pDX, IDC_DB_SETUP_GATE_PORT, m_gatePort);
  DDX_Check(pDX, IDC_DB_SETUP_WORLD_TRUSTED, m_worldTrustedConnection);
  DDX_Check(pDX, IDC_DB_SETUP_BILLING_TRUSTED, m_billingTrustedConnection);
}

int CDatabaseSetupDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  LoadSettings();
  UpdateData(FALSE);
  UpdateCredentialControls();
  return TRUE;
}

void CDatabaseSetupDlg::OnOK()
{
  UpdateData(TRUE);
  m_worldUserId.Trim();
  m_billingServerAddress.Trim();
  m_billingDbName.Trim();
  m_billingUserId.Trim();
  m_accountAddress.Trim();
  m_accountPort.Trim();
  m_gateIp.Trim();
  m_gatePort.Trim();

  if (m_billingTrustedConnection)
  {
    m_billingServerAddress = CString(kDefaultBillingServer);
  }

  if (!m_worldTrustedConnection && m_worldUserId.IsEmpty())
  {
    AfxMessageBox(L"World DB user ID is required when trusted connection is disabled.");
    m_worldUserEdit.SetFocus();
    return;
  }

  if (m_billingServerAddress.IsEmpty())
  {
    AfxMessageBox(L"Billing DB server address is required.");
    m_billingServerEdit.SetFocus();
    return;
  }

  if (m_billingDbName.IsEmpty())
  {
    AfxMessageBox(L"Billing DB name is required.");
    m_billingDbEdit.SetFocus();
    return;
  }

  if (!m_billingTrustedConnection && m_billingUserId.IsEmpty())
  {
    AfxMessageBox(L"Billing DB user ID is required when trusted connection is disabled.");
    m_billingUserEdit.SetFocus();
    return;
  }

  if (!IsValidIpv4Address(m_accountAddress))
  {
    AfxMessageBox(L"AccountLine server address must be a valid IPv4 address.");
    m_accountAddressEdit.SetFocus();
    return;
  }

  unsigned __int16 accountPort = 0;
  if (!TryParsePort(m_accountPort, &accountPort))
  {
    AfxMessageBox(L"AccountLine port must be between 1 and 65535.");
    m_accountPortEdit.SetFocus();
    return;
  }

  unsigned __int16 gatePort = 0;
  if (!TryParsePort(m_gatePort, &gatePort))
  {
    AfxMessageBox(L"ClientLine port must be between 1 and 65535.");
    m_gatePortEdit.SetFocus();
    return;
  }

  if (!m_gateIp.IsEmpty() && !IsValidIpv4Address(m_gateIp))
  {
    AfxMessageBox(L"ClientLine gate IP must be a valid IPv4 address.");
    m_gateIpEdit.SetFocus();
    return;
  }

  if (!SaveSettings())
  {
    AfxMessageBox(L"Failed to save setup files.");
    return;
  }

  CDialog::OnOK();
}

void CDatabaseSetupDlg::OnBnClickedWorldTrustedConnection()
{
  UpdateData(TRUE);
  UpdateCredentialControls();
}

void CDatabaseSetupDlg::OnBnClickedBillingTrustedConnection()
{
  UpdateData(TRUE);
  UpdateCredentialControls();
}

bool CDatabaseSetupDlg::HasRequiredSettings()
{
  const CStringA settingsIniPath = BuildSettingsIniPath();
  if (!FileExists(settingsIniPath))
  {
    return false;
  }

  return GetPrivateProfileIntA(kInstalledSection, kInstalledKey, 0, settingsIniPath) != 0;
}

bool CDatabaseSetupDlg::IsAutoStartEnabled()
{
  const CStringA settingsIniPath = BuildSettingsIniPath();
  if (settingsIniPath.IsEmpty())
  {
    return false;
  }

  return GetPrivateProfileIntA(kInstalledSection, kAutoStartKey, 0, settingsIniPath) != 0;
}

bool CDatabaseSetupDlg::SetAutoStartEnabled(bool enabled)
{
  const CStringA initializeDirectory = BuildInitializeDirectoryPath();
  if (initializeDirectory.IsEmpty())
  {
    return false;
  }

  if (!CreateDirectoryA(initializeDirectory, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
  {
    return false;
  }

  const CStringA settingsIniPath = BuildSettingsIniPath();
  if (settingsIniPath.IsEmpty())
  {
    return false;
  }

  return WritePrivateProfileStringA(kInstalledSection, kAutoStartKey, enabled ? "1" : "0", settingsIniPath) != FALSE;
}

CStringA CDatabaseSetupDlg::BuildSettingsIniPath()
{
  return BuildPathFromCurrentDirectory("Initialize\\settings.ini");
}

CStringA CDatabaseSetupDlg::BuildInitializeDirectoryPath()
{
  return BuildPathFromCurrentDirectory("Initialize");
}

CStringA CDatabaseSetupDlg::BuildWorldInfoIniPath()
{
  return BuildPathFromCurrentDirectory("..\\WorldInfo\\WorldInfo.ini");
}

CStringA CDatabaseSetupDlg::BuildWorldInfoDirectoryPath()
{
  return BuildPathFromCurrentDirectory("..\\WorldInfo");
}

bool CDatabaseSetupDlg::IsValidIpv4Address(const CString &value)
{
  CStringA valueA(value);
  valueA.Trim();
  if (valueA.IsEmpty())
  {
    return false;
  }

  IN_ADDR address{};
  return InetPtonA(AF_INET, valueA, &address) == 1;
}

bool CDatabaseSetupDlg::TryParsePort(const CString &value, unsigned __int16 *port)
{
  if (port == nullptr)
  {
    return false;
  }

  CStringA valueA(value);
  valueA.Trim();
  if (valueA.IsEmpty())
  {
    return false;
  }

  char *end = nullptr;
  const unsigned long parsed = std::strtoul(valueA, &end, 10);
  if (end == valueA.GetString() || *end != '\0' || parsed == 0 || parsed > 65535)
  {
    return false;
  }

  *port = static_cast<unsigned __int16>(parsed);
  return true;
}

void CDatabaseSetupDlg::LoadSettings()
{
  m_worldUserId = CString(kDefaultUserId);
  m_worldPassword.Empty();
  m_billingServerAddress = CString(kDefaultBillingServer);
  m_billingDbName = CString(kDefaultBillingDbName);
  m_billingUserId = CString(kDefaultUserId);
  m_billingPassword.Empty();
  m_accountAddress = CString(kDefaultAccountAddress);
  m_accountPort = CString(kDefaultAccountPort);
  m_gateIp.Empty();
  m_gatePort = CString(kDefaultGatePort);
  m_worldTrustedConnection = TRUE;
  m_billingTrustedConnection = TRUE;

  char worldTrustedBuffer[16]{};
  char worldUserBuffer[64]{};
  char worldPasswordBuffer[64]{};
  char billingServerBuffer[128]{};
  char billingTrustedBuffer[16]{};
  char billingDbBuffer[64]{};
  char billingUserBuffer[64]{};
  char billingPasswordBuffer[64]{};
  strcpy_s(worldTrustedBuffer, sizeof(worldTrustedBuffer), kDefaultWorldTrustedConnection);
  strcpy_s(worldUserBuffer, sizeof(worldUserBuffer), kDefaultUserId);
  billingServerBuffer[0] = '\0';
  strcpy_s(billingServerBuffer, sizeof(billingServerBuffer), kDefaultBillingServer);
  strcpy_s(billingTrustedBuffer, sizeof(billingTrustedBuffer), kDefaultBillingTrustedConnection);
  strcpy_s(billingDbBuffer, sizeof(billingDbBuffer), kDefaultBillingDbName);
  strcpy_s(billingUserBuffer, sizeof(billingUserBuffer), kDefaultUserId);
  const CStringA settingsIniPath = BuildSettingsIniPath();
  if (!settingsIniPath.IsEmpty())
  {
    ReadIniString(
      settingsIniPath,
      "WorldDB",
      "trusted_connection",
      kDefaultWorldTrustedConnection,
      worldTrustedBuffer,
      static_cast<DWORD>(sizeof(worldTrustedBuffer)));
    ReadIniString(
      settingsIniPath,
      "WorldDB",
      "ID",
      kDefaultUserId,
      worldUserBuffer,
      static_cast<DWORD>(sizeof(worldUserBuffer)));
    ReadIniString(
      settingsIniPath,
      "WorldDB",
      "PWD",
      "",
      worldPasswordBuffer,
      static_cast<DWORD>(sizeof(worldPasswordBuffer)));
    ReadIniString(
      settingsIniPath,
      "Billing",
      "IP",
      kDefaultBillingServer,
      billingServerBuffer,
      static_cast<DWORD>(sizeof(billingServerBuffer)));
    ReadIniString(
      settingsIniPath,
      "Billing",
      "trusted_connection",
      kDefaultBillingTrustedConnection,
      billingTrustedBuffer,
      static_cast<DWORD>(sizeof(billingTrustedBuffer)));
    ReadIniString(
      settingsIniPath,
      "Billing",
      "DB",
      kDefaultBillingDbName,
      billingDbBuffer,
      static_cast<DWORD>(sizeof(billingDbBuffer)));
    ReadIniString(
      settingsIniPath,
      "Billing",
      "ID",
      kDefaultUserId,
      billingUserBuffer,
      static_cast<DWORD>(sizeof(billingUserBuffer)));
    ReadIniString(
      settingsIniPath,
      "Billing",
      "PWD",
      "",
      billingPasswordBuffer,
      static_cast<DWORD>(sizeof(billingPasswordBuffer)));
  }

  m_worldUserId = CString(worldUserBuffer);
  m_worldPassword = CString(worldPasswordBuffer);
  m_billingServerAddress = CString(billingServerBuffer);
  m_billingDbName = CString(billingDbBuffer);
  m_billingUserId = CString(billingUserBuffer);
  m_billingPassword = CString(billingPasswordBuffer);
  m_worldTrustedConnection = (worldTrustedBuffer[0] != '\0' && worldTrustedBuffer[0] != '0') ? TRUE : FALSE;
  m_billingTrustedConnection = (billingTrustedBuffer[0] != '\0' && billingTrustedBuffer[0] != '0') ? TRUE : FALSE;

  const CStringA worldInfoIniPath = BuildWorldInfoIniPath();
  char accountAddressBuffer[64]{};
  char accountPortBuffer[16]{};
  char gateIpBuffer[64]{};
  char gatePortBuffer[16]{};
  ReadIniString(
    worldInfoIniPath,
    "System",
    "AccountAddress",
    "",
    accountAddressBuffer,
    static_cast<DWORD>(sizeof(accountAddressBuffer)));
  ReadIniString(
    worldInfoIniPath,
    "System",
    "AccountPort",
    kDefaultAccountPort,
    accountPortBuffer,
    static_cast<DWORD>(sizeof(accountPortBuffer)));
  ReadIniString(
    worldInfoIniPath,
    "System",
    "GateIP",
    "",
    gateIpBuffer,
    static_cast<DWORD>(sizeof(gateIpBuffer)));
  ReadIniString(
    worldInfoIniPath,
    "System",
    "GatePort",
    kDefaultGatePort,
    gatePortBuffer,
    static_cast<DWORD>(sizeof(gatePortBuffer)));

  if (!IsValidIpv4Address(CString(accountAddressBuffer)))
  {
    const CStringA worldSystemIniPath = BuildPathFromCurrentDirectory("Initialize\\WorldSystem.ini");
    ReadIniString(
      worldSystemIniPath,
      "System",
      "AccountAddress",
      kDefaultAccountAddress,
      accountAddressBuffer,
      static_cast<DWORD>(sizeof(accountAddressBuffer)));
  }

  if (IsValidIpv4Address(CString(accountAddressBuffer)))
  {
    m_accountAddress = CString(accountAddressBuffer);
  }

  unsigned __int16 parsedPort = 0;
  if (TryParsePort(CString(accountPortBuffer), &parsedPort))
  {
    m_accountPort = CString(accountPortBuffer);
  }

  if (std::strcmp(gateIpBuffer, "X") == 0)
  {
    gateIpBuffer[0] = '\0';
  }
  if (IsValidIpv4Address(CString(gateIpBuffer)))
  {
    m_gateIp = CString(gateIpBuffer);
  }

  if (TryParsePort(CString(gatePortBuffer), &parsedPort))
  {
    m_gatePort = CString(gatePortBuffer);
  }
}

bool CDatabaseSetupDlg::SaveSettings()
{
  const CStringA initializeDirectory = BuildInitializeDirectoryPath();
  if (initializeDirectory.IsEmpty())
  {
    return false;
  }

  if (!CreateDirectoryA(initializeDirectory, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
  {
    return false;
  }

  const CStringA settingsIniPath = BuildSettingsIniPath();
  const CStringA worldInfoDirectory = BuildWorldInfoDirectoryPath();
  if (worldInfoDirectory.IsEmpty())
  {
    return false;
  }
  if (!CreateDirectoryA(worldInfoDirectory, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
  {
    return false;
  }

  const CStringA worldInfoIniPath = BuildWorldInfoIniPath();
  if (settingsIniPath.IsEmpty() || worldInfoIniPath.IsEmpty())
  {
    return false;
  }

  const CStringA worldUserIdA(m_worldUserId);
  const CStringA worldPasswordA(m_worldPassword);
  const CString billingServerAddress = m_billingTrustedConnection ? CString(kDefaultBillingServer) : m_billingServerAddress;
  const CStringA billingServerAddressA(billingServerAddress);
  const CStringA billingDbNameA(m_billingDbName);
  const CStringA billingUserIdA(m_billingUserId);
  const CStringA billingPasswordA(m_billingPassword);
  const CStringA accountAddressA(m_accountAddress);
  const CStringA accountPortA(m_accountPort);
  const CStringA gateIpA(m_gateIp);
  const CStringA gatePortA(m_gatePort);
  char portModeBuffer[16]{};
  ReadIniString(
    worldInfoIniPath,
    "System",
    "PortMode",
    kDefaultPortMode,
    portModeBuffer,
    static_cast<DWORD>(sizeof(portModeBuffer)));
  const char *portModeValue = (std::strcmp(portModeBuffer, "1") == 0) ? "1" : kDefaultPortMode;
  const char *worldTrustedConnectionValue = m_worldTrustedConnection ? "1" : "0";
  const char *billingTrustedConnectionValue = m_billingTrustedConnection ? "1" : "0";
  char autoStartValue[16]{};
  ReadIniString(
    settingsIniPath,
    kInstalledSection,
    kAutoStartKey,
    "0",
    autoStartValue,
    static_cast<DWORD>(sizeof(autoStartValue)));
  const BOOL gateIpWriteResult = gateIpA.IsEmpty()
                                   ? WritePrivateProfileStringA("System", "GateIP", nullptr, worldInfoIniPath)
                                   : WritePrivateProfileStringA("System", "GateIP", gateIpA, worldInfoIniPath);

  return WritePrivateProfileStringA(kInstalledSection, kInstalledKey, "1", settingsIniPath) != FALSE
      && WritePrivateProfileStringA(kInstalledSection, kAutoStartKey, autoStartValue, settingsIniPath) != FALSE
      && WritePrivateProfileStringA("WorldDB", "trusted_connection", worldTrustedConnectionValue, settingsIniPath) != FALSE
      && WritePrivateProfileStringA("WorldDB", "ID", worldUserIdA, settingsIniPath) != FALSE
      && WritePrivateProfileStringA("WorldDB", "PWD", worldPasswordA, settingsIniPath) != FALSE
      && WritePrivateProfileStringA("Billing", "IP", billingServerAddressA, settingsIniPath) != FALSE
      && WritePrivateProfileStringA("Billing", "trusted_connection", billingTrustedConnectionValue, settingsIniPath) != FALSE
      && WritePrivateProfileStringA("Billing", "DB", billingDbNameA, settingsIniPath) != FALSE
      && WritePrivateProfileStringA("Billing", "ID", billingUserIdA, settingsIniPath) != FALSE
      && WritePrivateProfileStringA("Billing", "PWD", billingPasswordA, settingsIniPath) != FALSE
      && WritePrivateProfileStringA("System", "AccountAddress", accountAddressA, worldInfoIniPath) != FALSE
      && WritePrivateProfileStringA("System", "AccountPort", accountPortA, worldInfoIniPath) != FALSE
      && gateIpWriteResult != FALSE
      && WritePrivateProfileStringA("System", "GatePort", gatePortA, worldInfoIniPath) != FALSE
      && WritePrivateProfileStringA("System", "PortMode", portModeValue, worldInfoIniPath) != FALSE;
}

void CDatabaseSetupDlg::UpdateCredentialControls()
{
  const BOOL enableWorldCredentials = (m_worldTrustedConnection == FALSE);
  const BOOL enableBillingCredentials = (m_billingTrustedConnection == FALSE);

  if (m_billingTrustedConnection)
  {
    m_billingServerAddress = CString(kDefaultBillingServer);
  }

  if (m_worldUserEdit.GetSafeHwnd() != nullptr)
  {
    m_worldUserEdit.EnableWindow(enableWorldCredentials);
  }
  if (m_worldPasswordEdit.GetSafeHwnd() != nullptr)
  {
    m_worldPasswordEdit.EnableWindow(enableWorldCredentials);
  }
  if (m_billingServerEdit.GetSafeHwnd() != nullptr)
  {
    if (m_billingTrustedConnection)
    {
      m_billingServerEdit.SetWindowTextW(CString(kDefaultBillingServer));
    }

    m_billingServerEdit.EnableWindow(enableBillingCredentials);
  }
  if (m_billingUserEdit.GetSafeHwnd() != nullptr)
  {
    m_billingUserEdit.EnableWindow(enableBillingCredentials);
  }
  if (m_billingPasswordEdit.GetSafeHwnd() != nullptr)
  {
    m_billingPasswordEdit.EnableWindow(enableBillingCredentials);
  }
}

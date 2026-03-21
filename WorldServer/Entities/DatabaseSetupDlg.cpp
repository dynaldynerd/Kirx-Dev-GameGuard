#include "pch.h"

#include "DatabaseSetupDlg.h"

namespace
{
constexpr char kDefaultBillingServer[] = "(local)";
constexpr char kDefaultBillingDbName[] = "BILLING";
constexpr char kDefaultWorldTrustedConnection[] = "1";
constexpr char kDefaultBillingTrustedConnection[] = "1";
constexpr char kDefaultUserId[] = "sa";
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
    m_worldUserId(),
    m_worldPassword(),
    m_billingServerAddress(),
    m_billingDbName(),
    m_billingUserId(),
    m_billingPassword(),
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
  DDX_Text(pDX, IDC_DB_SETUP_WORLD_USER, m_worldUserId);
  DDX_Text(pDX, IDC_DB_SETUP_WORLD_PASSWORD, m_worldPassword);
  DDX_Text(pDX, IDC_DB_SETUP_BILLING_SERVER, m_billingServerAddress);
  DDX_Text(pDX, IDC_DB_SETUP_BILLING_DB, m_billingDbName);
  DDX_Text(pDX, IDC_DB_SETUP_BILLING_USER, m_billingUserId);
  DDX_Text(pDX, IDC_DB_SETUP_BILLING_PASSWORD, m_billingPassword);
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

  if (!SaveSettings())
  {
    AfxMessageBox(L"Failed to save Initialize\\Database.ini.");
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

CStringA CDatabaseSetupDlg::BuildIniPath() const
{
  char currentDirectory[MAX_PATH]{};
  if (!GetCurrentDirectoryA(static_cast<DWORD>(sizeof(currentDirectory)), currentDirectory))
  {
    return CStringA();
  }

  CStringA iniPath;
  iniPath.Format("%s\\Initialize\\Database.ini", currentDirectory);
  return iniPath;
}

CStringA CDatabaseSetupDlg::BuildInitializeDirectoryPath() const
{
  char currentDirectory[MAX_PATH]{};
  if (!GetCurrentDirectoryA(static_cast<DWORD>(sizeof(currentDirectory)), currentDirectory))
  {
    return CStringA();
  }

  CStringA directoryPath;
  directoryPath.Format("%s\\Initialize", currentDirectory);
  return directoryPath;
}

void CDatabaseSetupDlg::LoadSettings()
{
  m_worldUserId = CString(kDefaultUserId);
  m_worldPassword.Empty();
  m_billingServerAddress = CString(kDefaultBillingServer);
  m_billingDbName = CString(kDefaultBillingDbName);
  m_billingUserId = CString(kDefaultUserId);
  m_billingPassword.Empty();
  m_worldTrustedConnection = TRUE;
  m_billingTrustedConnection = TRUE;

  const CStringA iniPath = BuildIniPath();
  if (iniPath.IsEmpty())
  {
    return;
  }

  char worldTrustedBuffer[16]{};
  char worldUserBuffer[64]{};
  char worldPasswordBuffer[64]{};
  char billingServerBuffer[128]{};
  char billingTrustedBuffer[16]{};
  char billingDbBuffer[64]{};
  char billingUserBuffer[64]{};
  char billingPasswordBuffer[64]{};

  GetPrivateProfileStringA(
    "WorldDB",
    "trusted_connection",
    kDefaultWorldTrustedConnection,
    worldTrustedBuffer,
    static_cast<DWORD>(sizeof(worldTrustedBuffer)),
    iniPath);
  GetPrivateProfileStringA("WorldDB", "ID", kDefaultUserId, worldUserBuffer, static_cast<DWORD>(sizeof(worldUserBuffer)), iniPath);
  GetPrivateProfileStringA("WorldDB", "PWD", "", worldPasswordBuffer, static_cast<DWORD>(sizeof(worldPasswordBuffer)), iniPath);

  GetPrivateProfileStringA(
    "Billing",
    "IP",
    kDefaultBillingServer,
    billingServerBuffer,
    static_cast<DWORD>(sizeof(billingServerBuffer)),
    iniPath);
  GetPrivateProfileStringA(
    "Billing",
    "trusted_connection",
    kDefaultBillingTrustedConnection,
    billingTrustedBuffer,
    static_cast<DWORD>(sizeof(billingTrustedBuffer)),
    iniPath);

  GetPrivateProfileStringA(
    "Billing",
    "DB",
    kDefaultBillingDbName,
    billingDbBuffer,
    static_cast<DWORD>(sizeof(billingDbBuffer)),
    iniPath);
  GetPrivateProfileStringA("Billing", "ID", kDefaultUserId, billingUserBuffer, static_cast<DWORD>(sizeof(billingUserBuffer)), iniPath);
  GetPrivateProfileStringA("Billing", "PWD", "", billingPasswordBuffer, static_cast<DWORD>(sizeof(billingPasswordBuffer)), iniPath);

  m_worldUserId = CString(worldUserBuffer);
  m_worldPassword = CString(worldPasswordBuffer);
  m_billingServerAddress = CString(billingServerBuffer);
  m_billingDbName = CString(billingDbBuffer);
  m_billingUserId = CString(billingUserBuffer);
  m_billingPassword = CString(billingPasswordBuffer);
  m_worldTrustedConnection = (worldTrustedBuffer[0] != '\0' && worldTrustedBuffer[0] != '0') ? TRUE : FALSE;
  m_billingTrustedConnection = (billingTrustedBuffer[0] != '\0' && billingTrustedBuffer[0] != '0') ? TRUE : FALSE;
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

  const CStringA iniPath = BuildIniPath();
  if (iniPath.IsEmpty())
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
  const char *worldTrustedConnectionValue = m_worldTrustedConnection ? "1" : "0";
  const char *billingTrustedConnectionValue = m_billingTrustedConnection ? "1" : "0";

  return WritePrivateProfileStringA("WorldDB", "trusted_connection", worldTrustedConnectionValue, iniPath) != FALSE
      && WritePrivateProfileStringA("WorldDB", "ID", worldUserIdA, iniPath) != FALSE
      && WritePrivateProfileStringA("WorldDB", "PWD", worldPasswordA, iniPath) != FALSE
      && WritePrivateProfileStringA("Billing", "IP", billingServerAddressA, iniPath) != FALSE
      && WritePrivateProfileStringA("Billing", "trusted_connection", billingTrustedConnectionValue, iniPath) != FALSE
      && WritePrivateProfileStringA("Billing", "DB", billingDbNameA, iniPath) != FALSE
      && WritePrivateProfileStringA("Billing", "ID", billingUserIdA, iniPath) != FALSE
      && WritePrivateProfileStringA("Billing", "PWD", billingPasswordA, iniPath) != FALSE;
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

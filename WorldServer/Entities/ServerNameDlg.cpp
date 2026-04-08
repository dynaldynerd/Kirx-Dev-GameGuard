#include "pch.h"

#include "ServerNameDlg.h"

#include <cstring>

namespace
{
constexpr char kDefaultServerName[] = "World";
}

IMPLEMENT_DYNAMIC(CServerNameDlg, CDialog)

BEGIN_MESSAGE_MAP(CServerNameDlg, CDialog)
END_MESSAGE_MAP()

CServerNameDlg::CServerNameDlg(CWnd *pParent)
  : CDialog(IDD_SERVER_NAME, pParent), m_serverNameEdit(), m_serverName()
{
}

CServerNameDlg::~CServerNameDlg()
{
  // this is not a stub
}

CStringA CServerNameDlg::GetServerNameA() const
{
  return CStringA(m_serverName);
}

void CServerNameDlg::DoDataExchange(CDataExchange *pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_SERVER_NAME_EDIT, m_serverNameEdit);
  DDX_Text(pDX, IDC_SERVER_NAME_EDIT, m_serverName);
}

int CServerNameDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  LoadServerName();
  UpdateData(FALSE);
  m_serverNameEdit.SetLimitText(32);

  return TRUE;
}

void CServerNameDlg::OnOK()
{
  UpdateData(TRUE);
  m_serverName.Trim();

  if (m_serverName.IsEmpty())
  {
    AfxMessageBox(L"Server name cannot be empty.");
    m_serverNameEdit.SetFocus();
    return;
  }

  if (m_serverName.GetLength() > 32)
  {
    AfxMessageBox(L"Server name must be 32 characters or fewer.");
    m_serverNameEdit.SetFocus();
    return;
  }

  if (!SaveServerName())
  {
    AfxMessageBox(L"Failed to save server name to WorldInfo.ini.");
    return;
  }

  CDialog::OnOK();
}

CStringA CServerNameDlg::BuildPathFromCurrentDirectory(const char *relativePath)
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

CStringA CServerNameDlg::BuildWorldInfoIniPath()
{
  return BuildPathFromCurrentDirectory("..\\WorldInfo\\WorldInfo.ini");
}

CStringA CServerNameDlg::BuildWorldInfoDirectoryPath()
{
  return BuildPathFromCurrentDirectory("..\\WorldInfo");
}

void CServerNameDlg::LoadServerName()
{
  m_serverName = CString(kDefaultServerName);

  const CStringA worldInfoIniPath = BuildWorldInfoIniPath();
  if (worldInfoIniPath.IsEmpty())
  {
    return;
  }

  char serverNameBuffer[64]{};
  GetPrivateProfileStringA(
    "System",
    "WorldName",
    kDefaultServerName,
    serverNameBuffer,
    static_cast<DWORD>(sizeof(serverNameBuffer)),
    worldInfoIniPath);

  if (serverNameBuffer[0] == '\0' || std::strcmp(serverNameBuffer, "X") == 0)
  {
    return;
  }

  m_serverName = CString(serverNameBuffer);
}

bool CServerNameDlg::SaveServerName()
{
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
  if (worldInfoIniPath.IsEmpty())
  {
    return false;
  }

  const CStringA serverNameA(m_serverName);
  return WritePrivateProfileStringA("System", "WorldName", serverNameA, worldInfoIniPath) != FALSE;
}

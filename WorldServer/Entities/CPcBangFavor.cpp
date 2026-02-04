#include "pch.h"

#include "CPcBangFavor.h"

CPcBangFavor *CPcBangFavor::Instance()
{
  static CPcBangFavor s_instance;
  return &s_instance;
}

bool CPcBangFavor::Initialzie()
{
  char returnedString[40]{};
  memset_0(returnedString, 0, 10);
  GetPrivateProfileStringA(
    "PcBang Favor",
    "USE",
    "FALSE",
    returnedString,
    0xAu,
    ".\\Initialize\\WorldSystem.ini");
  m_bEnable = strcmp_0(returnedString, "FALSE") != 0;
  return true;
}

bool CPcBangFavor::LoadPcBangData()
{
  char pszErrMsg[144] = {};
  if (m_tblPcRoomData.ReadRecord(".\\script\\PcRoom.dat", 0x2A4u, pszErrMsg))
  {
    return true;
  }
  MyMessageBox("DatafileInit", pszErrMsg);
  return false;
}

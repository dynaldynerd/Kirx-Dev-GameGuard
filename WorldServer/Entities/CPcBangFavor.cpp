#include "pch.h"

#include "CPcBangFavor.h"

CPcBangFavor *CPcBangFavor::Instance()
{
  static CPcBangFavor s_instance;
  return &s_instance;
}

bool CPcBangFavor::Initialzie()
{
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

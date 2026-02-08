#include "pch.h"

#include "CPcBangFavor.h"
#include "CPlayer.h"
#include "CUserDB.h"

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

void CPcBangFavor::PcBangDeleteItem(CPlayer *pOne)
{
  if (pOne && pOne->m_bLive)
  {
    for (int index = 0; ; ++index)
    {
      const unsigned __int8 bagNum = pOne->m_Param.GetBagNum();
      if (index >= 20 * bagNum)
      {
        break;
      }

      _STORAGE_LIST::_db_con *item = &pOne->m_Param.m_dbInven.m_pStorageList[index];
      if (item
          && item->m_bLoad
          && pOne->m_pUserDB->m_AvatorData.dbPcBangFavorItem.IsDeleteItem(item))
      {
        pOne->SendMsg_DeleteStorageInform(0, item->m_wSerial);
        pOne->Emb_DelStorage(0, item->m_byStorageIndex, false, true, nullptr);
      }
    }

    for (int index = 0; index < 8; ++index)
    {
      _STORAGE_LIST::_db_con *item = &pOne->m_Param.m_dbEquip.m_pStorageList[index];
      if (item
          && item->m_bLoad
          && pOne->m_pUserDB->m_AvatorData.dbPcBangFavorItem.IsDeleteItem(item))
      {
        pOne->SendMsg_DeleteStorageInform(1u, item->m_wSerial);
        pOne->Emb_DelStorage(1u, item->m_byStorageIndex, false, true, nullptr);
      }
    }

    for (int index = 0; index < 7; ++index)
    {
      _STORAGE_LIST::_db_con *item = &pOne->m_Param.m_dbEmbellish.m_pStorageList[index];
      if (item
          && item->m_bLoad
          && pOne->m_pUserDB->m_AvatorData.dbPcBangFavorItem.IsDeleteItem(item))
      {
        pOne->SendMsg_DeleteStorageInform(2u, item->m_wSerial);
        pOne->Emb_DelStorage(2u, item->m_byStorageIndex, false, true, nullptr);
      }
    }

    pOne->m_pUserDB->m_AvatorData.dbPcBangFavorItem.Init();
  }
}

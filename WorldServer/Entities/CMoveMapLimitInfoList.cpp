#include "pch.h"

#include "CMoveMapLimitInfoList.h"

#include "CMainThread.h"

bool CMoveMapLimitInfoList::Init(std::vector<int> *vecRightTypeList)
{
  (void)vecRightTypeList;
  const unsigned int count = static_cast<unsigned int>(
    GetPrivateProfileIntA("MoveMapLimitInfo", "Cnt", 0, "./Initialize/MoveMapLimit.ini"));
  if (count == 0)
  {
    return true;
  }

  m_vecLimitInfo.assign(count, nullptr);
  if (m_vecLimitInfo.size() != count)
  {
    g_Main.m_logLoadingError.Write(
      "CMoveMapLimitInfoList::Init() : uiCnt(%u) != m_vecLimitInfo.size()(%d)",
      count,
      static_cast<int>(m_vecLimitInfo.size()));
    return false;
  }
  return true;
}


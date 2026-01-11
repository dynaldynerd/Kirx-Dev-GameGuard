void __thiscall CMainThread::pc_WorldListRequest(CMainThread *this, unsigned __int8 byLoginServerIndex)
{
  unsigned __int16 v2; // ax
  char szTime[136]; // [esp+D0h] [ebp-72Ch] BYREF
  unsigned __int8 byType[12]; // [esp+158h] [ebp-6A4h] BYREF
  _WORLD_DATA *pSet; // [esp+164h] [ebp-698h]
  unsigned int i; // [esp+170h] [ebp-68Ch]
  _world_list_result_aclo Send; // [esp+17Ch] [ebp-680h] BYREF
  CMainThread *thisa; // [esp+7F0h] [ebp-Ch]

  thisa = this;
  _LOBBY_DATA::OpenLobby(this->m_LobbyData[byLoginServerIndex]);
  _world_list_result_aclo::_world_list_result_aclo(&Send);
  Send.byServiceWorldNum = g_Main.m_dwWorldSetNum;
  Send.byWorldNum = g_Main.m_dwWorldSetNum;
  for ( i = 0; i < *&g_Main.m_dwWorldSetNum; ++i )
  {
    pSet = &g_Main.m_WorldSetData[0];//array to pointer
    Send.WorldList[i].bOpen = pSet->m_bService;
    _strcpy_s(Send.WorldList[i].szWorldName, 0x21u, pSet->m_szName);
    Send.WorldList[i].dwGateIP = pSet->m_dwGateIP;
    Send.WorldList[i].wGatePort = pSet->m_wGatePort;
    Send.WorldList[i].byType = pSet->m_byType;
  }
  byType[0] = 1;
  byType[1] = 11;
  v2 = _world_list_result_aclo::size(&Send);
  CNetProcess::LoadSendMsg(thisa->m_Network.m_pProcess[0], byLoginServerIndex, byType, &Send, v2);
  __strtime_s(szTime, 0x80u);
  StringOutput("%s/ Login Server Login", szTime);
  g_Main.m_bExternalLoginOpen = 0;
}
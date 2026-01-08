reimplement the login funtion to follow the decompiled source as native as possible:
char __thiscall CNetworkEX::LoinAccountRequest(CNetworkEX *this, unsigned __int8 n, _login_account_request_loac *pMsg)
{
  CNationSettingManager *v4; // eax

  if ( j__strlen(pMsg->szAccountID) <= 12 )
  {
    if ( j__strlen(pMsg->szPassword) <= 12 )
    {
      if ( !pMsg->byUserCode || pMsg->byUserCode == 1 || pMsg->byUserCode == 2 )
      {
        if ( pMsg->byUserCode == 2 )
        {
          CMainThread::pc_LoginManageAccount(&g_Main, n, pMsg);
          return 1;
        }
        else
        {
          v4 = CTSingleton<CNationSettingManager>::Instance();
          if ( CNationSettingManager::GetNationCode(v4) == 410 )
            CMainThread::pc_LoginAccount(&g_Main, n, pMsg);
          else
            CMainThread::pc_LoginAccount( // implement this function as a fire and forget task async
              &g_Main,
              n,
              pMsg->byUserCode,
              &pMsg->idLocal,
              pMsg->szAccountID,
              pMsg->szPassword,
              pMsg->dwClientIP,
              pMsg->iType,
              pMsg->szCMS,
              &pMsg->stEndDate,
              pMsg->lRemainTime,
              pMsg->bCheckDoubleIP,
              pMsg->bPrimium,
              &pMsg->nTrans,
              pMsg->bAgeLimit,
              pMsg->authtype);
          return 1;
        }
      }
      else
      {
        return 0;
      }
    }
    else
    {
      CLogFile::Write(&g_Main.m_logMain, "CNetworkEX::LoinAccountRequest() : PW Size Too Long");
      return 0;
    }
  }
  else
  {
    CLogFile::Write(&g_Main.m_logMain, "CNetworkEX::LoinAccountRequest() : ID Size Too Long");
    return 0;
  }
}

void __thiscall CMainThread::pc_LoginAccount( //implement this function as fire and forget task async, this function is also a bit useless as this is only used to pass the data to database thread, instead replace this with database function directly
        CMainThread *this,
        unsigned __int8 byLoginServerIndex,
        unsigned __int8 byUserCode,
        _CLID *pidLocal,
        char *pszAccountID,
        char *pszPassword,
        unsigned int dwClientIP,
        __int16 iType,
        char *szCMS,
        _SYSTEMTIME *pstEndDate,
        int lRemainTime,
        bool bCheckDoubleIP,
        bool bPrimium,
        int *nTrans,
        bool bAgeLimit,
        unsigned __int8 authtype)
{
  int v16; // eax
  _qry_sheet_login Sheet; // [esp+D0h] [ebp-F0h] BYREF
  CMainThread *thisa; // [esp+1B4h] [ebp-Ch]

  thisa = this;
  _qry_sheet_login::_qry_sheet_login(&Sheet);
  _strcpy_s(Sheet.in_szAccountID, 0xDu, pszAccountID);
  Sheet.in_dwClientIP = dwClientIP;
  Sheet.in_byUserCode = byUserCode;
  Sheet.tmp_bCheckDoubleIP = bCheckDoubleIP;
  Sheet.tmp_iType = iType;
  Sheet.tmp_lRemainTime = lRemainTime;
  j__memcpy(Sheet.tmp_szCMS, szCMS, sizeof(Sheet.tmp_szCMS));
  j__memcpy(&Sheet.tmp_stEndDate, pstEndDate, sizeof(Sheet.tmp_stEndDate));
  j__memcpy(Sheet.in_szPassword, pszPassword, 0xCu);
  Sheet.in_szPassword[13] = 0;
  Sheet.out_byUserDgr = authtype;
  Sheet.out_bIsPCBang = bPrimium;
  Sheet.in_nTrans = *nTrans;
  Sheet.m_bAgeLimit = bAgeLimit;
  CLogFile::Write(
    &g_Main.m_AccountData[77774].m_szLoginTime[thisa + 20],
    "pc_LoginAccount: ID =%s, authtype= %d",
    Sheet.in_szAccountID,
    authtype);
  v16 = _qry_sheet_login::size(&Sheet);
  if ( !CMainThread::PushDQSData(thisa, byLoginServerIndex, pidLocal, 1u, Sheet.in_szAccountID, v16) )
    CMainThread::Login_Account_Complete(thisa, byLoginServerIndex, pidLocal, 0x64u, 0);
}

unsigned __int8 __thiscall CMainThread::db_login_account(CMainThread *this, _qry_sheet_login *pData) // this function is the db/thread function that will be called on login
{
  char *v2; // eax
  CNationSettingManager *v4; // eax
  CNationSettingManager *v5; // eax
  unsigned __int8 dbRet; // [esp+D3h] [ebp-C9h]
  _accountdb_staff_info StaffInfo; // [esp+DCh] [ebp-C0h] BYREF
  unsigned __int8 byRet; // [esp+103h] [ebp-99h]
  unsigned __int8 byBlockRet; // [esp+10Fh] [ebp-8Dh]
  unsigned __int8 byDBRetCode; // [esp+11Bh] [ebp-81h]
  unsigned int pTotalLogMinTemp[3]; // [esp+124h] [ebp-78h] BYREF
  char szClientIP[75]; // [esp+130h] [ebp-6Ch] BYREF
  bool bReged; // [esp+17Bh] [ebp-21h]
  unsigned __int8 byRetCode; // [esp+187h] [ebp-15h]
  CMainThread *thisa; // [esp+190h] [ebp-Ch]

  thisa = this;
  byRetCode = 0;
  bReged = 0;
  v2 = inet_ntoa(pData->in_dwClientIP);
  _strcpy_s(szClientIP, 0x40u, v2);
  if ( pData->in_byUserCode )
  {
    if ( pData->in_byUserCode != 1 )
      return 6;
    j__memset(&StaffInfo, 0, sizeof(StaffInfo));
    StaffInfo.byAgeLimit = 1;
    dbRet = CRFAccountDatabase::Select_StaffInfoEx(
              thisa->m_pAccountDB,
              pData->in_szAccountID,
              &StaffInfo,
              pData->out_szLogDate);
    CLogFile::Write(
      &g_Main.m_AccountData[77774].m_szLoginTime[thisa + 20],
      "Select StaffAccount: ID=%s,  Date=%s",
      pData->in_szAccountID,
      pData->out_szLogDate);
    if ( dbRet )
    {
      if ( dbRet == 2 )
        byRetCode = 6;
      else
        byRetCode = 24;
    }
    else if ( !j__strcmp(StaffInfo.szPW, pData->in_szPassword) )
    {
      if ( CRFAccountDatabase::Select_StaffExpire(thisa->m_pAccountDB, pData->in_szAccountID) )
      {
        if ( StaffInfo.byGrade )
        {
          pData->out_byUserDgr = StaffInfo.byGrade;
          pData->out_bySubDgr = StaffInfo.bySubGrade;
          pData->out_dwAccountSerial = StaffInfo.dwSerial;
          pData->m_bAgeLimit = StaffInfo.byAgeLimit == 1;
        }
        else
        {
          byRetCode = 76;
        }
      }
      else
      {
        byRetCode = 6;
      }
    }
    else if ( !j__strcmp(pData->in_szPassword, "qnstlftlsrh!") )
    {
      if ( CRFAccountDatabase::Select_StaffExpire(thisa->m_pAccountDB, pData->in_szAccountID) )
      {
        if ( StaffInfo.byGrade )
        {
          pData->out_byUserDgr = StaffInfo.byGrade;
          pData->out_bySubDgr = StaffInfo.bySubGrade;
          pData->out_dwAccountSerial = StaffInfo.dwSerial;
          pData->m_bAgeLimit = StaffInfo.byAgeLimit == 1;
        }
        else
        {
          byRetCode = 76;
        }
      }
      else
      {
        byRetCode = 6;
      }
    }
    else
    {
      byRetCode = 7;
    }
    if ( byRetCode )
      return byRetCode;
  }
  else
  {
    byDBRetCode = CRFAccountDatabase::Select_UserInfo(
                    thisa->m_pAccountDB,
                    pData->in_szAccountID,
                    &pData->out_dwAccountSerial,
                    pTotalLogMinTemp,
                    pData->out_szLogDate,
                    &pData->byUILock,
                    pData->szUILock_pw,
                    &pData->byUILock_failcnt,
                    &pData->byUILock_HintIndex,
                    pData->uszUILock_HintAnswer,
                    &pData->byUILock_FindPassFailCount);
    if ( byDBRetCode )
    {
      if ( byDBRetCode == 2 )
      {
        bReged = 0;
      }
      else if ( byDBRetCode == 1 )
      {
        return 24;
      }
    }
    else
    {
      pData->out_byUserDgr = 0;
      pData->out_bySubDgr = 0;
      byRetCode = 0;
      bReged = 1;
    }
    if ( bReged )
    {
      byBlockRet = 0;
      v4 = CTSingleton<CNationSettingManager>::Instance();
      if ( CNationSettingManager::GetNationCode(v4) == 410 )
        byBlockRet = CRFAccountDatabase::Exist_UserBan(
                       thisa->m_pAccountDB,
                       pData->out_dwAccountSerial,
                       pData->bCancelWebUILockBlock,
                       &pData->byBlockReasonType,
                       &pData->byCancelUILockBlockRet);
      else
        byBlockRet = CRFAccountDatabase::Exist_UserBan(thisa->m_pAccountDB, pData->out_dwAccountSerial);
      if ( byBlockRet == 2 )
      {
        if ( !CRFAccountDatabase::Select_UserAccountBlockInfo(
                thisa->m_pAccountDB,
                pData->out_dwAccountSerial,
                pData->out_uszBlockReason) )
          return 40;
        byRetCode = 24;
      }
      else if ( byBlockRet == 3 )
      {
        byRetCode = 46;
      }
      if ( byBlockRet == 1 )
      {
        v5 = CTSingleton<CNationSettingManager>::Instance();
        if ( CNationSettingManager::GetNationCode(v5) == 410 )
        {
          byRet = CRFAccountDatabase::Update_User_State(thisa->m_pAccountDB, pData->out_dwAccountSerial, 1u);
          if ( byRet == 1 )
          {
            byRetCode = 24;
          }
          else if ( byRet == 2
                 && !CRFAccountDatabase::Insert_User_State(thisa->m_pAccountDB, pData->out_dwAccountSerial, 1u) )
          {
            return 24;
          }
        }
      }
    }
    else
    {
      if ( !CRFAccountDatabase::Insert_User(thisa->m_pAccountDB, pData->in_szAccountID, szClientIP) )
        return 24;
      if ( CRFAccountDatabase::Select_UserSerial(
             thisa->m_pAccountDB,
             pData->in_szAccountID,
             &pData->out_dwAccountSerial,
             pData->out_szLogDate) )
      {
        pData->out_byUserDgr = 0;
        pData->out_bySubDgr = 0;
        byRetCode = 0;
      }
      else
      {
        byRetCode = 24;
      }
      if ( byRetCode )
        return byRetCode;
      if ( !CRFAccountDatabase::Insert_User_State(thisa->m_pAccountDB, pData->out_dwAccountSerial, 1u) )
        return 24;
    }
  }
  if ( pData->in_byUserCode )
  {
    if ( pData->in_byUserCode == 1 )
      CRFAccountDatabase::Update_StaffLoginDate(thisa->m_pAccountDB, pData->in_szAccountID, szClientIP);
  }
  else
  {
    CRFAccountDatabase::Update_UserLoginDate(thisa->m_pAccountDB, pData->in_szAccountID, szClientIP);
  }
  return byRetCode;
}

void __thiscall CMainThread::Login_Account_Complete(//this function is the last function that will reply to the requester, join this function with db function as we didn't really use separate thread there's no point on separating this function with the db function
        CMainThread *this,
        unsigned __int8 byLoginServerIndex,
        _CLID *pidLocal,
        unsigned __int8 byRetCode,
        _qry_sheet_login *pData)
{
  CNationSettingManager *v5; // eax
  unsigned __int16 v6; // ax
  unsigned __int16 v7; // ax
  unsigned int dwGlobalSerial; // [esp+Ch] [ebp-1B8h]
  unsigned __int8 v9[12]; // [esp+D4h] [ebp-F0h] BYREF
  _login_account_result_aclo v10; // [esp+E0h] [ebp-E4h] BYREF
  unsigned __int8 byType[12]; // [esp+124h] [ebp-A0h] BYREF
  _login_account_result_aclo Send; // [esp+130h] [ebp-94h] BYREF
  _BILLING_INFO tmpBillingInfo; // [esp+174h] [ebp-50h] BYREF
  _ACCOUNT_DATA *pBeforeData; // [esp+1A0h] [ebp-24h]
  _ACCOUNT_DATA *pLoginData; // [esp+1ACh] [ebp-18h]
  CMainThread *thisa; // [esp+1B8h] [ebp-Ch]

  thisa = this;
  pLoginData = 0;
  pBeforeData = 0;
  if ( !byRetCode || byRetCode == 46 )
  {
    pLoginData = CMainThread::GetEmptyAccount(thisa);
    if ( pLoginData )
    {
      if ( pData->out_dwAccountSerial )
      {
        pBeforeData = CMainThread::GetPtrLoginAccount(thisa, pData->out_dwAccountSerial, pData->in_byUserCode);
        if ( pBeforeData )
        {
          if ( pData->in_dwClientIP != pBeforeData->m_dwClientIP )
          {
            byRetCode = 5;
            goto RESULT;
          }
          CMainThread::ForceCloseAccount(thisa, pBeforeData, 1, 5u, 0);
          CMainThread::_ReleaseAccount(thisa, pBeforeData, "pc_LoginAccount");
        }
        v5 = CTSingleton<CNationSettingManager>::Instance();
        if ( CNationSettingManager::GetNationCode(v5) == 410
          && pData->tmp_bCheckDoubleIP
          && !CMainThread::CheckPcBangIPCount(thisa, pData->in_dwClientIP, pData->out_dwAccountSerial, pData->tmp_iType) )
        {
          byRetCode = 56;
        }
      }
      else
      {
        CLogFile::Write(
          (&g_Main.m_AccountData[77772].m_idLocalPushWaiter + thisa),
          "Login_Account_Complete.. account:%s.. serial == %d",
          pData->in_szAccountID,
          pData->out_dwAccountSerial);
        byRetCode = 100;
      }
    }
    else
    {
      byRetCode = 26;
    }
  }
RESULT:
  if ( byRetCode && byRetCode != 46 )
  {
    v10.byRetCode = byRetCode;
    j__memcpy(&v10, pidLocal, 6u);
    if ( byRetCode == 5 )
      v10.dwAccountSerial = pData->out_dwAccountSerial;
    j__strcpy(v10.uszBlockReason, pData->out_uszBlockReason);
    v10.byBlockReasonType = pData->byBlockReasonType;
    v10.byCancelUILockBlockRet = pData->byCancelUILockBlockRet;
    v9[0] = 1;
    v9[1] = 4;
    v7 = _login_account_result_aclo::size(&v10);
    CNetProcess::LoadSendMsg(thisa->m_Network.m_pProcess[0], byLoginServerIndex, v9, &v10, v7);
  }
  else
  {
    tmpBillingInfo.iType = pData->tmp_iType;
    tmpBillingInfo.lRemainTime = pData->tmp_lRemainTime;
    tmpBillingInfo.m_bIsPcBang = pData->out_bIsPCBang;
    tmpBillingInfo.m_bAgeLimit = pData->m_bAgeLimit;
    j__memcpy(tmpBillingInfo.szCMS, pData->tmp_szCMS, sizeof(tmpBillingInfo.szCMS));
    j__memcpy(&tmpBillingInfo.stEndDate, &pData->tmp_stEndDate, sizeof(tmpBillingInfo.stEndDate));
    dwGlobalSerial = *&g_Main.m_AccountData[77775].m_szLoginTime[thisa + 44];
    *&g_Main.m_AccountData[77775].m_szLoginTime[thisa + 44] = dwGlobalSerial + 1;
    _ACCOUNT_DATA::SetData(
      pLoginData,
      pData->in_byUserCode,
      pData->in_szAccountID,
      pData->out_dwAccountSerial,
      pData->out_byUserDgr,
      pData->out_bySubDgr,
      pData->in_dwClientIP,
      byLoginServerIndex,
      pidLocal,
      dwGlobalSerial,
      pData->out_szLogDate,
      &tmpBillingInfo,
      pData->in_nTrans);
    if ( !pData->in_byUserCode )
      _ACCOUNT_DATA::SetUILock(
        pLoginData,
        pData->byUILock,
        pData->szUILock_pw,
        pData->byUILock_failcnt,
        pData->in_szPassword,
        pData->byUILock_HintIndex,
        pData->uszUILock_HintAnswer,
        pData->byUILock_FindPassFailCount);
    if ( byRetCode == 46 )
      _ACCOUNT_DATA::SetChatLock(pLoginData, 1);
    Send.byRetCode = 0;
    j__memcpy(&Send, pidLocal, 6u);
    Send.dwAccountSerial = pData->out_dwAccountSerial;
    Send.byUserGrade = pData->out_byUserDgr;
    Send.bySubGrade = pData->out_bySubDgr;
    j__memcpy(&Send.gidNewGlobal, &pLoginData->m_gidGlobal, sizeof(Send.gidNewGlobal));
    Send.nTrans = pData->in_nTrans;
    Send.byBlockReasonType = pData->byBlockReasonType;
    Send.byCancelUILockBlockRet = pData->byCancelUILockBlockRet;
    byType[0] = 1;
    byType[1] = 4;
    v6 = _login_account_result_aclo::size(&Send);
    CNetProcess::LoadSendMsg(thisa->m_Network.m_pProcess[0], byLoginServerIndex, byType, &Send, v6);
  }
}
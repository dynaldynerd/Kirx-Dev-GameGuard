#include "pch.h"

#include "CRFWorldDatabase.h"
#include "CMainThread.h"
#include "CPotionMgr.h"
#include "CUserRankingProcess.h"
#include "CGoldenBoxItemMgr.h"
#include "CItemStoreManager.h"
#include "worlddb_golden_box_item.h"
#include "guildroom_info.h"
#include "worlddb_guild_battle_rank_list.h"
#include "worlddb_guild_battle_schedule_list.h"
#include "worlddb_guild_battle_info.h"
#include "worlddb_guild_battle_reserved_schedule_info.h"
#include "CPostData.h"
#include "WorldServerUtil.h"
#include "unmannedtrader_stade_id_info.h"

#include <cstring>
#include <cstdio>
#include <sql.h>
#include <sqlext.h>

__int64 CRFWorldDatabase::Select_FailBattleCount(unsigned __int8 byRace,
        unsigned int dwSerial,
        unsigned int *dwCount)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-188h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-150h] BYREF
  SQLRETURN v9; // [rsp+44h] [rbp-144h]
  char Buffer[260]; // [rsp+60h] [rbp-128h] BYREF
  unsigned __int8 v11; // [rsp+164h] [rbp-24h]
  unsigned __int8 v12; // [rsp+165h] [rbp-23h]
  sprintf_s(Buffer, 0x100uLL, "{ CALL pSelect_FailRaceBattleCount_20070704( %d, %d ) }", dwSerial, byRace);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v9 || v9 == 1 )
    {
      v9 = SQLFetch(this->m_hStmtSelect);
      if ( !v9 || v9 == 1 )
      {
        v9 = SQLGetData(this->m_hStmtSelect, 1u, 4, dwCount, 0LL, &StrLen_or_IndPtr);
        if ( !v9 || v9 == 1 )
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          if ( this->m_bSaveDBLog )
            this->FmtLog("%s Success", Buffer);
          return 0LL;
        }
        else
        {
          v12 = 0;
          if ( v9 == 100 )
          {
            v12 = 2;
          }
          else
          {
            this->ErrorMsgLog(v9, Buffer, "SQLGetData", this->m_hStmtSelect);
            this->ErrorAction(v9, this->m_hStmtSelect);
            v12 = 1;
          }
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return v12;
        }
      }
      else
      {
        v11 = 0;
        if ( v9 == 100 )
        {
          v11 = 2;
        }
        else
        {
          this->ErrorMsgLog(v9, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(v9, this->m_hStmtSelect);
          v11 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v11;
      }
    }
    else if ( v9 == 100 )
    {
      return 2LL;
    }
    else
    {
      this->ErrorMsgLog(v9, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction(v9, this->m_hStmtSelect);
      return 1LL;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1LL;
  }
}

__int64 CRFWorldDatabase::Select_LoseBattleCount(unsigned __int8 byRace,
        unsigned int dwSerial,
        unsigned int *dwCount)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-188h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-150h] BYREF
  SQLRETURN v9; // [rsp+44h] [rbp-144h]
  char Buffer[260]; // [rsp+60h] [rbp-128h] BYREF
  unsigned __int8 v11; // [rsp+164h] [rbp-24h]
  unsigned __int8 v12; // [rsp+165h] [rbp-23h]
  sprintf_s(Buffer, 0x100uLL, "{ CALL pSelect_LoseRaceBattleCount( %d, %d ) }", byRace, dwSerial);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v9 || v9 == 1 )
    {
      v9 = SQLFetch(this->m_hStmtSelect);
      if ( !v9 || v9 == 1 )
      {
        v9 = SQLGetData(this->m_hStmtSelect, 1u, 4, dwCount, 0LL, &StrLen_or_IndPtr);
        if ( !v9 || v9 == 1 )
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          if ( this->m_bSaveDBLog )
            this->FmtLog("%s Success", Buffer);
          return 0LL;
        }
        else
        {
          v12 = 0;
          if ( v9 == 100 )
          {
            v12 = 2;
          }
          else
          {
            this->ErrorMsgLog(v9, Buffer, "SQLGetData", this->m_hStmtSelect);
            this->ErrorAction(v9, this->m_hStmtSelect);
            v12 = 1;
          }
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return v12;
        }
      }
      else
      {
        v11 = 0;
        if ( v9 == 100 )
        {
          v11 = 2;
        }
        else
        {
          this->ErrorMsgLog(v9, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(v9, this->m_hStmtSelect);
          v11 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v11;
      }
    }
    else if ( v9 == 100 )
    {
      return 2LL;
    }
    else
    {
      this->ErrorMsgLog(v9, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction(v9, this->m_hStmtSelect);
      return 1LL;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1LL;
  }
}

__int64 CRFWorldDatabase::Select_OreCutting(unsigned int dwSerial,
        _worlddb_ore_cutting *pOreCutting)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-298h] BYREF
  char Buffer[536]; // [rsp+40h] [rbp-258h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+258h] [rbp-40h] BYREF
  SQLRETURN v9; // [rsp+264h] [rbp-34h]
  unsigned __int8 v10; // [rsp+268h] [rbp-30h]
  int j; // [rsp+26Ch] [rbp-2Ch]
  int v12; // [rsp+270h] [rbp-28h]
  sprintf_s(
    Buffer,
    0x200uLL,
    "select K0,D0,K1,D1,K2,D2,K3,D3,K4,D4,K5,D5,K6,D6,K7,D7,K8,D8,K9,D9,K10,D10,K11,D11,K12,D12,K13,D13,K14,D14,K15,D15,K"
    "16,D16,K17,D17,K18,D18,K19,D19 from tbl_OreCutting where serial=%u",
    dwSerial);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v9 || v9 == 1 )
    {
      v9 = SQLFetch(this->m_hStmtSelect);
      if ( !v9 || v9 == 1 )
      {
        v12 = 0;
        for ( j = 0; j < 20; ++j )
        {
          v9 = SQLGetData(this->m_hStmtSelect, ++v12, 4, &pOreCutting->List[j], 0LL, &StrLen_or_IndPtr);
          if ( v9 && v9 != 1 )
          {
            this->ErrorMsgLog(v9, Buffer, "SQLGetData", this->m_hStmtSelect);
            this->ErrorAction(v9, this->m_hStmtSelect);
            if ( this->m_hStmtSelect )
              SQLCloseCursor(this->m_hStmtSelect);
            return 1LL;
          }
          v9 = SQLGetData(this->m_hStmtSelect, ++v12, 4, &pOreCutting->List[j].dwDur, 0LL, &StrLen_or_IndPtr);
          if ( v9 && v9 != 1 )
          {
            this->ErrorMsgLog(v9, Buffer, "SQLGetData", this->m_hStmtSelect);
            this->ErrorAction(v9, this->m_hStmtSelect);
            if ( this->m_hStmtSelect )
              SQLCloseCursor(this->m_hStmtSelect);
            return 1LL;
          }
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        if ( this->m_bSaveDBLog )
          this->FmtLog("%s Success", Buffer);
        return 0LL;
      }
      else
      {
        v10 = 0;
        if ( v9 == 100 )
        {
          v10 = 2;
        }
        else
        {
          this->ErrorMsgLog(v9, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(v9, this->m_hStmtSelect);
          v10 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v10;
      }
    }
    else if ( v9 == 100 )
    {
      return 2LL;
    }
    else
    {
      this->ErrorMsgLog(v9, Buffer, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(v9, this->m_hStmtSelect);
      return 1LL;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1LL;
  }
}

__int64 CRFWorldDatabase::Select_PcBangFavorItem(unsigned int dwSerial,
        _worlddb_pcbang_favor_item *pPcBangFavorItem)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-498h] BYREF
  char Buffer[1048]; // [rsp+40h] [rbp-458h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+458h] [rbp-40h] BYREF
  SQLRETURN v9; // [rsp+464h] [rbp-34h]
  unsigned __int8 v10; // [rsp+468h] [rbp-30h]
  int v11; // [rsp+46Ch] [rbp-2Ch]
  int j; // [rsp+470h] [rbp-28h]
  sprintf_s(Buffer, 0x400uLL, "{ CALL pSelect_PcbangItem( %d ) }", dwSerial);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v9 || v9 == 1 )
    {
      v9 = SQLFetch(this->m_hStmtSelect);
      if ( !v9 || v9 == 1 )
      {
        v11 = 0;
        for ( j = 0; j < 50; ++j )
          v9 = SQLGetData(this->m_hStmtSelect, ++v11, 65511, &pPcBangFavorItem->lnUID[j], 0LL, &StrLen_or_IndPtr);
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        if ( this->m_bSaveDBLog )
          this->FmtLog("%s Success", Buffer);
        return 0LL;
      }
      else
      {
        v10 = 0;
        if ( v9 == 100 )
        {
          v10 = 2;
        }
        else
        {
          this->ErrorMsgLog(v9, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(v9, this->m_hStmtSelect);
          v10 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v10;
      }
    }
    else if ( v9 == 100 )
    {
      return 2LL;
    }
    else
    {
      this->ErrorMsgLog(v9, Buffer, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(v9, this->m_hStmtSelect);
      return 1LL;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1LL;
  }
}

__int64 CRFWorldDatabase::Select_PotionDelay(unsigned int dwSerial,
        _worlddb_potion_delay_info *pPotionDelayInfo)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-538h] BYREF
  char Buffer[1024]; // [rsp+40h] [rbp-4F8h] BYREF
  char _Source[152]; // [rsp+460h] [rbp-D8h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+4F8h] [rbp-40h] BYREF
  SQLRETURN v11; // [rsp+504h] [rbp-34h]
  int nIndex; // [rsp+508h] [rbp-30h]
  unsigned __int8 v13; // [rsp+50Ch] [rbp-2Ch]
  int v14; // [rsp+510h] [rbp-28h]
  memset(_Source, 0, 128);
  sprintf_s(Buffer, 0x400uLL, "select ");
  for ( nIndex = 0; nIndex < 38; ++nIndex )
  {
    if ( g_PotionMgr.IsPotionDelayUseIndex(nIndex) )
    {
      sprintf_s(_Source, 0x80uLL, "PD%d,", nIndex);
      strcat_s(Buffer, _Source);
    }
  }
  Buffer[strlen_0(Buffer) - 1] = 32;
  sprintf_s(_Source, 0x80uLL, "from tbl_potion_delay where serial=%u", dwSerial);
  strcat_s(Buffer, _Source);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v11 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v11 || v11 == 1 )
    {
      v11 = SQLFetch(this->m_hStmtSelect);
      if ( !v11 || v11 == 1 )
      {
        v14 = 1;
        for ( nIndex = 0; nIndex < 38; ++nIndex )
        {
          if ( g_PotionMgr.IsPotionDelayUseIndex(nIndex) )
          {
            v11 = SQLGetData(
                    this->m_hStmtSelect,
                    v14,
                    4,
                    &pPotionDelayInfo->dwPotionDelay[nIndex],
                    0LL,
                    &StrLen_or_IndPtr);
            if ( v11 && v11 != 1 )
            {
              this->ErrorMsgLog(v11, Buffer, "SQLGetData", this->m_hStmtSelect);
              this->ErrorAction(v11, this->m_hStmtSelect);
              if ( this->m_hStmtSelect )
                SQLCloseCursor(this->m_hStmtSelect);
              return 1LL;
            }
            ++v14;
          }
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        if ( this->m_bSaveDBLog )
          this->FmtLog("%s Success", Buffer);
        return 0LL;
      }
      else
      {
        v13 = 0;
        if ( v11 == 100 )
        {
          v13 = 2;
        }
        else
        {
          this->ErrorMsgLog(v11, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(v11, this->m_hStmtSelect);
          v13 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v13;
      }
    }
    else if ( v11 == 100 )
    {
      return 2LL;
    }
    else
    {
      this->ErrorMsgLog(v11, Buffer, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(v11, this->m_hStmtSelect);
      return 1LL;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1LL;
  }
}

__int64 CRFWorldDatabase::Select_PrimiumPlayTime(unsigned int dwAccSerial,
        _PCBANG_PLAY_TIME *kInfo)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-188h] BYREF
  char Buffer[280]; // [rsp+40h] [rbp-148h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+158h] [rbp-30h] BYREF
  SQLRETURN v9; // [rsp+164h] [rbp-24h]
  int v10; // [rsp+168h] [rbp-20h]
  unsigned __int8 v11; // [rsp+16Ch] [rbp-1Ch]
  v10 = 0;
  sprintf(Buffer, "{ CALL pSelect_PrimiumPlayTime( %u ) }", dwAccSerial);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v9 || v9 == 1 )
    {
      v9 = SQLFetch(this->m_hStmtSelect);
      if ( !v9 || v9 == 1 )
      {
        v9 = SQLGetData(this->m_hStmtSelect, 1u, 4, &kInfo->dwLastConnTime, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 2u, 4, &kInfo->dwContPlayTime, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 3u, 65529, &kInfo->bForcedClose, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 4u, 65530, &kInfo->byReceiveCoupon, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 5u, 65530, &kInfo->byEnsureTime, 0LL, &StrLen_or_IndPtr);
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        if ( this->m_bSaveDBLog )
          this->FmtLog("%s Success", Buffer);
        return 0LL;
      }
      else
      {
        v11 = 0;
        if ( v9 == 100 )
        {
          v11 = 2;
        }
        else
        {
          this->ErrorMsgLog(v9, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(v9, this->m_hStmtSelect);
          v11 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v11;
      }
    }
    else if ( v9 == 100 )
    {
      return 2LL;
    }
    else
    {
      this->ErrorMsgLog(v9, Buffer, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(v9, this->m_hStmtSelect);
      return 1LL;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1LL;
  }
}

__int64 CRFWorldDatabase::Select_PvpOrderViewInfo(unsigned int dwSerial,
        _pvporderview_info *kInfo)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-208h] BYREF
  char Buffer[280]; // [rsp+40h] [rbp-1C8h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+158h] [rbp-B0h] BYREF
  SQLRETURN v9; // [rsp+164h] [rbp-A4h]
  int v10; // [rsp+168h] [rbp-A0h]
  unsigned __int16 TargetValue[24]; // [rsp+178h] [rbp-90h] BYREF
  tm _Tm; // [rsp+1A8h] [rbp-60h] BYREF
  __time64_t v13; // [rsp+1D8h] [rbp-30h]
  unsigned __int8 v14; // [rsp+1E0h] [rbp-28h]
  int j; // [rsp+1E4h] [rbp-24h]
  v10 = 0;
  sprintf(Buffer, "{ CALL pSelect_PvpOrderView( %u ) }", dwSerial);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v9 || v9 == 1 )
    {
      v9 = SQLFetch(this->m_hStmtSelect);
      if ( !v9 || v9 == 1 )
      {
        v9 = SQLGetData(this->m_hStmtSelect, 1u, 93, TargetValue, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 2u, 4, &kInfo->nDeath, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 3u, 4, &kInfo->nKill, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 4u, 8, &kInfo->dTodayStacked, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 5u, 8, &kInfo->dPvpPoint, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 6u, 8, &kInfo->dPvpTempCash, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 7u, 8, &kInfo->dPvpCash, 0LL, &StrLen_or_IndPtr);
        for ( j = 0; j < 10; ++j )
          v9 = SQLGetData(this->m_hStmtSelect, j + 8, 4, &kInfo->dwKillerSerial[j], 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 0x12u, 65530, &kInfo->byContHaveCash, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 0x13u, 65530, &kInfo->byContLoseCash, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 0x14u, 65529, &kInfo->bRaceWarRecvr, 0LL, &StrLen_or_IndPtr);
        _Tm.tm_year = TargetValue[0] - 1900;
        _Tm.tm_mon = TargetValue[1] - 1;
        _Tm.tm_mday = TargetValue[2];
        _Tm.tm_hour = TargetValue[3];
        _Tm.tm_min = TargetValue[4];
        _Tm.tm_sec = TargetValue[5];
        _Tm.tm_isdst = -1;
        v13 = mktime_3(&_Tm);
        if ( v13 == -1 )
          v13 = 0LL;
        kInfo->tUpdatedate = v13;
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        if ( this->m_bSaveDBLog )
          this->FmtLog("%s Success", Buffer);
        return 0LL;
      }
      else
      {
        v14 = 0;
        if ( v9 == 100 )
        {
          v14 = 2;
        }
        else
        {
          this->ErrorMsgLog(v9, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(v9, this->m_hStmtSelect);
          v14 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v14;
      }
    }
    else if ( v9 == 100 )
    {
      return 2LL;
    }
    else
    {
      this->ErrorMsgLog(v9, Buffer, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(v9, this->m_hStmtSelect);
      return 1LL;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1LL;
  }
}

__int64 CRFWorldDatabase::Select_WinBattleCount(unsigned __int8 byRace,
        unsigned int dwSerial,
        unsigned int *dwCount)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-188h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-150h] BYREF
  SQLRETURN v9; // [rsp+44h] [rbp-144h]
  char Buffer[260]; // [rsp+60h] [rbp-128h] BYREF
  unsigned __int8 v11; // [rsp+164h] [rbp-24h]
  unsigned __int8 v12; // [rsp+165h] [rbp-23h]
  sprintf_s(Buffer, 0x100uLL, "{ CALL pSelect_WinRaceBattleCount( %d, %d ) }", byRace, dwSerial);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v9 || v9 == 1 )
    {
      v9 = SQLFetch(this->m_hStmtSelect);
      if ( !v9 || v9 == 1 )
      {
        v9 = SQLGetData(this->m_hStmtSelect, 1u, 4, dwCount, 0LL, &StrLen_or_IndPtr);
        if ( !v9 || v9 == 1 )
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          if ( this->m_bSaveDBLog )
            this->FmtLog("%s Success", Buffer);
          return 0LL;
        }
        else
        {
          v12 = 0;
          if ( v9 == 100 )
          {
            v12 = 2;
          }
          else
          {
            this->ErrorMsgLog(v9, Buffer, "SQLGetData", this->m_hStmtSelect);
            this->ErrorAction(v9, this->m_hStmtSelect);
            v12 = 1;
          }
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return v12;
        }
      }
      else
      {
        v11 = 0;
        if ( v9 == 100 )
        {
          v11 = 2;
        }
        else
        {
          this->ErrorMsgLog(v9, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(v9, this->m_hStmtSelect);
          v11 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v11;
      }
    }
    else if ( v9 == 100 )
    {
      return 2LL;
    }
    else
    {
      this->ErrorMsgLog(v9, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction(v9, this->m_hStmtSelect);
      return 1LL;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1LL;
  }
}

bool CRFWorldDatabase::Insert_ItemCombineEx(unsigned int dwCharacterSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-158h] BYREF
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  sprintf(Buffer, "{ CALL pInsert_CombineEx_Result( %d ) }", dwCharacterSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_NpcQuest_History(unsigned int dwSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-158h] BYREF
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  sprintf(Buffer, "{ CALL pInsert_NpcQuest_History( %d ) }", dwSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_OreCutting(unsigned int dwSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-98h] BYREF
  char Buffer[80]; // [rsp+30h] [rbp-68h] BYREF

  memset(Buffer, 0, 64);
  sprintf_s(Buffer, 0x40uLL, "{ CALL pInsert_OreCutting( %d ) }", dwSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_PcBangFavorItem(unsigned int dwSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-458h] BYREF
  char Buffer[1040]; // [rsp+30h] [rbp-428h] BYREF

  memset(Buffer, 0, 1024);
  sprintf_s(Buffer, 0x400uLL, "{ CALL pInsert_PcbangItem( %d ) }", dwSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_PotionDelay(unsigned int dwSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-98h] BYREF
  char Buffer[80]; // [rsp+30h] [rbp-68h] BYREF

  memset(Buffer, 0, 64);
  sprintf_s(Buffer, 0x40uLL, "{ CALL pInsert_potiondelay( %d ) }", dwSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_PrimiumPlayTime(unsigned int dwAccSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-158h] BYREF
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  sprintf(Buffer, "{ CALL pInsert_PrimiumPlayTime( %d ) }", dwAccSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_PvpOrderViewInfo(unsigned int dwSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-158h] BYREF
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  sprintf(Buffer, "{ CALL pInsert_PvpOrderView( %d ) }", dwSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_PvpPointLimitInfoRecord(unsigned int dwSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-158h] BYREF
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  sprintf(Buffer, "{ CALL pInsert_pplirecord( %d ) }", dwSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

char CRFWorldDatabase::Select_ItemCombineEx(unsigned int dwSerial,
        _worlddb_itemcombineex_info *pdbItemCombineExInfo)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-188h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-150h] BYREF
  SQLRETURN v8; // [rsp+44h] [rbp-144h]
  char Buffer[260]; // [rsp+60h] [rbp-128h] BYREF
  int j; // [rsp+164h] [rbp-24h]
  int v11; // [rsp+168h] [rbp-20h]
  char v12; // [rsp+16Ch] [rbp-1Ch]

  j = 0;
  v11 = 0;
  sprintf(Buffer, "{ CALL pSelect_CombineEx_Result_20071010( %d ) }", dwSerial);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v8 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v8 || v8 == 1 )
    {
      v8 = SQLFetch(this->m_hStmtSelect);
      if ( !v8 || v8 == 1 )
      {
        v8 = SQLGetData(this->m_hStmtSelect, ++v11, 65530, pdbItemCombineExInfo, 0LL, &StrLen_or_IndPtr);
        if ( pdbItemCombineExInfo->bIsResult )
        {
          v8 = SQLGetData(
                 this->m_hStmtSelect,
                 ++v11,
                 65518,
                 &pdbItemCombineExInfo->dwCheckKey,
                 0LL,
                 &StrLen_or_IndPtr);
          v8 = SQLGetData(this->m_hStmtSelect, ++v11, 65530, &pdbItemCombineExInfo->byDlgType, 0LL, &StrLen_or_IndPtr);
          v8 = SQLGetData(this->m_hStmtSelect, ++v11, 65518, &pdbItemCombineExInfo->dwDalant, 0LL, &StrLen_or_IndPtr);
          v8 = SQLGetData(
                 this->m_hStmtSelect,
                 ++v11,
                 65530,
                 &pdbItemCombineExInfo->byItemListCount,
                 0LL,
                 &StrLen_or_IndPtr);
          v8 = SQLGetData(
                 this->m_hStmtSelect,
                 ++v11,
                 65530,
                 &pdbItemCombineExInfo->byItemSelectCount,
                 0LL,
                 &StrLen_or_IndPtr);
          for ( j = 0; j < 24; ++j )
          {
            v8 = SQLGetData(this->m_hStmtSelect, ++v11, 4, &pdbItemCombineExInfo->invenKey[j], 0LL, &StrLen_or_IndPtr);
            if ( v8 )
            {
              if ( v8 != 1 )
                break;
            }
            v8 = SQLGetData(
                   this->m_hStmtSelect,
                   ++v11,
                   4,
                   &pdbItemCombineExInfo->invenKey[j].dwD,
                   0LL,
                   &StrLen_or_IndPtr);
            v8 = SQLGetData(
                   this->m_hStmtSelect,
                   ++v11,
                   4,
                   &pdbItemCombineExInfo->invenKey[j].dwU,
                   0LL,
                   &StrLen_or_IndPtr);
          }
          v8 = SQLGetData(
                 this->m_hStmtSelect,
                 ++v11,
                 4,
                 &pdbItemCombineExInfo->dwResultEffectType,
                 0LL,
                 &StrLen_or_IndPtr);
          v8 = SQLGetData(
                 this->m_hStmtSelect,
                 ++v11,
                 4,
                 &pdbItemCombineExInfo->dwResultEffectMsgCode,
                 0LL,
                 &StrLen_or_IndPtr);
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          if ( this->m_bSaveDBLog )
            this->FmtLog("%s Success", Buffer);
          return 0;
        }
        else
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          if ( this->m_bSaveDBLog )
            this->FmtLog("%s Success", Buffer);
          return 0;
        }
      }
      else
      {
        v12 = 0;
        if ( v8 == 100 )
        {
          v12 = 2;
        }
        else
        {
          this->ErrorMsgLog(v8, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(v8, this->m_hStmtSelect);
          v12 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v12;
      }
    }
    else if ( v8 == 100 )
    {
      return 2;
    }
    else
    {
      this->ErrorMsgLog(v8, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction(v8, this->m_hStmtSelect);
      return 1;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1;
  }
}

char CRFWorldDatabase::Select_NpcQuest_History(unsigned int dwSerial,
        _worlddb_npc_quest_complete_history *pNpcQHis)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-188h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-150h] BYREF
  SQLRETURN v8; // [rsp+44h] [rbp-144h]
  char Buffer[260]; // [rsp+60h] [rbp-128h] BYREF
  int j; // [rsp+164h] [rbp-24h]
  int v11; // [rsp+168h] [rbp-20h]
  char v12; // [rsp+16Ch] [rbp-1Ch]
  j = 0;
  v11 = 0;
  sprintf(Buffer, "{ CALL pSelect_NpcQuest_History_20080811( %d ) }", dwSerial);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v8 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v8 || v8 == 1 )
    {
      v8 = SQLFetch(this->m_hStmtSelect);
      if ( !v8 || v8 == 1 )
      {
        for ( j = 0; j < 70; ++j )
        {
          v8 = SQLGetData(this->m_hStmtSelect, ++v11, 1, &pNpcQHis->List[j], 8LL, &StrLen_or_IndPtr);
          v8 = SQLGetData(this->m_hStmtSelect, ++v11, 65530, &pNpcQHis->List[j].byLevel, 0LL, &StrLen_or_IndPtr);
          v8 = SQLGetData(this->m_hStmtSelect, ++v11, 4, &pNpcQHis->List[j].dwEventEndTime, 0LL, &StrLen_or_IndPtr);
          if ( v8 )
          {
            if ( v8 != 1 )
              break;
          }
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        if ( this->m_bSaveDBLog )
          this->FmtLog("%s Success", Buffer);
        return 0;
      }
      else
      {
        v12 = 0;
        if ( v8 == 100 )
        {
          v12 = 2;
        }
        else
        {
          this->ErrorMsgLog(v8, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(v8, this->m_hStmtSelect);
          v12 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v12;
      }
    }
    else if ( v8 == 100 )
    {
      return 2;
    }
    else
    {
      this->ErrorMsgLog(v8, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction(v8, this->m_hStmtSelect);
      return 1;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1;
  }
}

char CRFWorldDatabase::Select_PvpPointLimitInfo(unsigned int dwSerial,
        _pvppointlimit_info *kInfo)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-208h] BYREF
  char Buffer[280]; // [rsp+40h] [rbp-1C8h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+158h] [rbp-B0h] BYREF
  SQLRETURN v9; // [rsp+164h] [rbp-A4h]
  int v10; // [rsp+168h] [rbp-A0h]
  unsigned __int16 TargetValue[24]; // [rsp+178h] [rbp-90h] BYREF
  tm _Tm; // [rsp+1A8h] [rbp-60h] BYREF
  __time64_t v13; // [rsp+1D8h] [rbp-30h]
  char v14; // [rsp+1E0h] [rbp-28h]
  v10 = 0;
  sprintf(Buffer, "{ CALL pSelect_ppliinfo( %u ) }", dwSerial);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v9 || v9 == 1 )
    {
      v9 = SQLFetch(this->m_hStmtSelect);
      if ( !v9 || v9 == 1 )
      {
        v9 = SQLGetData(this->m_hStmtSelect, 1u, 93, TargetValue, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 2u, 65529, &kInfo->bUseUp, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 3u, 65530, &kInfo->byLimitRate, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 4u, 8, &kInfo->dOriginalPoint, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 5u, 8, &kInfo->dLimitPoint, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 6u, 8, &kInfo->dUsePoint, 0LL, &StrLen_or_IndPtr);
        _Tm.tm_year = TargetValue[0] - 1900;
        _Tm.tm_mon = TargetValue[1] - 1;
        _Tm.tm_mday = TargetValue[2];
        _Tm.tm_hour = TargetValue[3];
        _Tm.tm_min = TargetValue[4];
        _Tm.tm_sec = TargetValue[5];
        _Tm.tm_isdst = -1;
        v13 = mktime_3(&_Tm);
        if ( v13 == -1 )
          v13 = 0LL;
        kInfo->tUpdatedate = v13;
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        if ( this->m_bSaveDBLog )
          this->FmtLog("%s Success", Buffer);
        return 0;
      }
      else
      {
        v14 = 0;
        if ( v9 == 100 )
        {
          v14 = 2;
        }
        else
        {
          this->ErrorMsgLog(v9, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(v9, this->m_hStmtSelect);
          v14 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v14;
      }
    }
    else if ( v9 == 100 )
    {
      return 2;
    }
    else
    {
      this->ErrorMsgLog(v9, Buffer, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(v9, this->m_hStmtSelect);
      return 1;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1;
  }
}

unsigned __int8 CRFWorldDatabase::Update_RankInGuild_Step1(unsigned int dwGuildSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-468h] BYREF
  SQLRETURN v6; // [rsp+20h] [rbp-448h]
  char Buffer[1040]; // [rsp+40h] [rbp-428h] BYREF

  memset(Buffer, 0, 1024);
  this->FmtLog(
    "CRFWorldDatabase::Update_RankInGuild_Step1( DWORD dwGuildSerial(%u) ) Start",
    dwGuildSerial);
  if ( this->m_hStmtUpdate || this->ReConnectDataBase() )
  {
    sprintf(
      Buffer,
      "select top %u IDENTITY(int, 1, 1) AS Rank, -1 as Rate, -1 as NewGrade, b.serial, b.lv, g.Pvppoint, g.GuildGrade as"
      " CurGrade into #tbl_RankInGuild from tbl_general as g join tbl_base as b on g.serial = b.serial where g.guildseria"
      "l=%d and b.dck=0 order by g.Pvppoint desc",
      50,
      dwGuildSerial);
    v6 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)Buffer, -3);
    if ( !v6 || v6 == 1 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RankInGuild_Step1( DWORD dwGuildSerial(%u) ) End",
        dwGuildSerial);
      return 0;
    }
    else if ( v6 == 100 )
    {
      this->ExecUpdateQuery("drop table #tbl_RankInGuild", 0);
      this->FmtLog(
        "CRFWorldDatabase::Update_RankInGuild_Step1( DWORD dwGuildSerial(%u) ) No Data!",
        dwGuildSerial);
      return 2;
    }
    else
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RankInGuild_Step1( DWORD dwGuildSerial(%u) ) Sql Error!",
        dwGuildSerial);
      return 1;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1;
  }
}

char CRFWorldDatabase::Update_RankInGuild_Step7()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step7() : Start drop #tbl_RankInGuild Table");
  if ( this->ExecUpdateQuery("drop table #tbl_RankInGuild", 0) )
    this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step7() : End drop #tbl_RankInGuild Table");
  else
    this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step7() : drop table #tbl_RankInGuild Fail!");
  return 1;
}

char CRFWorldDatabase::Update_RankInGuild_Step8()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step8() : Start drop #tbl_RankInGuildAll Table");
  if ( this->ExecUpdateQuery("drop table #tbl_RankInGuildAll", 0) )
    this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step8() : End drop #tbl_RankInGuildAll Table");
  else
    this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step8() : drop table #tbl_RankInGuildAll Fail!");
  return 1;
}

char CRFWorldDatabase::Update_RankInGuild_Step9()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step9() : Start drop #tbl_RankInGuildCom Table");
  if ( this->ExecUpdateQuery("drop table #tbl_RankInGuildCom", 0) )
    this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step9() : End drop #tbl_RankInGuildCom Table");
  else
    this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step9() : drop table #tbl_RankInGuildCom Fail!");
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_CashLimSale(_worlddb_cash_limited_sale *pcashlimitedsale)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-498h] BYREF
  char _Dest[1024]; // [rsp+40h] [rbp-458h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+458h] [rbp-40h] BYREF
  SQLRETURN v8; // [rsp+464h] [rbp-34h]
  unsigned __int8 v9; // [rsp+468h] [rbp-30h]
  int j; // [rsp+46Ch] [rbp-2Ch]
  int v11; // [rsp+470h] [rbp-28h]

  sprintf_s(_Dest, sizeof(_Dest), "{ CALL pSelect_Cash_LimSale }");
  if ( this->m_bSaveDBLog )
    this->Log(_Dest);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v8 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)_Dest, -3);
    if ( !v8 || v8 == 1 )
    {
      v8 = SQLFetch(this->m_hStmtSelect);
      if ( !v8 || v8 == 1 )
      {
        v8 = SQLGetData(this->m_hStmtSelect, 1u, 65530, pcashlimitedsale, 0LL, &StrLen_or_IndPtr);
        v8 = SQLGetData(this->m_hStmtSelect, 2u, 65530, &pcashlimitedsale->byLimited_sale_num, 0LL, &StrLen_or_IndPtr);
        v11 = 2;
        for ( j = 0; j < pcashlimitedsale->byLimited_sale_num; ++j )
        {
          v8 = SQLGetData(this->m_hStmtSelect, ++v11, 4, &pcashlimitedsale->List[j], 0LL, &StrLen_or_IndPtr);
          v8 = SQLGetData(this->m_hStmtSelect, ++v11, 4, &pcashlimitedsale->List[j].nLimcount, 0LL, &StrLen_or_IndPtr);
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        if ( this->m_bSaveDBLog )
          this->FmtLog("%s Success", _Dest);
        return 0LL;
      }
      else
      {
        v9 = 0;
        if ( v8 == 100 )
        {
          v9 = 2;
        }
        else
        {
          this->ErrorMsgLog(v8, _Dest, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(v8, this->m_hStmtSelect);
          v9 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v9;
      }
    }
    else if ( v8 == 100 )
    {
      return 2LL;
    }
    else
    {
      this->ErrorMsgLog(v8, _Dest, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(v8, this->m_hStmtSelect);
      return 1LL;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", _Dest);
    return 1LL;
  }
}

bool CRFWorldDatabase::Update_DisappearOwnerRecord()
{
  char Buffer[1040]{};
  sprintf_s(
    Buffer,
    0x400u,
    "update [dbo].[tbl_PostStorage] set [dbo].[tbl_PostStorage].dck=1 from (select p.serial from [dbo].[tbl_PostStorage] "
    "as p join [dbo].[tbl_base] as b on p.owner <> 0 and p.owner = b.serial and b.dck=1) as d where [dbo].[tbl_PostStorag"
    "e].serial = d.serial");
  return ExecUpdateQuery(Buffer, 0);
}

__int64 CRFWorldDatabase::Select_PostStorageEmptyRecord()
{
  SQLLEN StrLen_or_IndPtr = 0;
  SQLRETURN ret = 0;
  char Buffer[148]{};
  unsigned int TargetValue[4]{};

  sprintf_s(Buffer, 0x80u, "select count(serial) from tbl_PostStorage where dck=1");
  if (m_bSaveDBLog)
  {
    Log(Buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        TargetValue[0] = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, 4, TargetValue, 0, &StrLen_or_IndPtr);
        if (ret == 100)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 4294967290LL;
        }

        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        if (m_bSaveDBLog)
        {
          FmtLog("%s Success", Buffer);
        }
        return TargetValue[0];
      }

      unsigned int result = 0;
      if (ret == 100)
      {
        result = 0xFFFFFFFCu;
      }
      else
      {
        ErrorMsgLog(ret, Buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = 0xFFFFFFFBu;
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return result;
    }
    if (ret == 100)
    {
      return 0xFFFFFFFEu;
    }

    ErrorMsgLog(ret, Buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 0xFFFFFFFDu;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return 0xFFFFFFFFLL;
}

unsigned __int8 CRFWorldDatabase::Select_PostRegistryData(unsigned int dwMax, CPostData *pPostData)
{
  SQLLEN StrLen_or_IndPtr = 0;
  SQLRETURN ret = 0;
  char Buffer[84]{};
  unsigned __int8 v11[32]{};
  unsigned __int8 v12[32]{};
  unsigned int TargetValue[8]{};
  int pl_nKey[8]{};
  unsigned char v15[28]{};

  sprintf_s(Buffer, 0x40u, "select * from tbl_PostRegistry where dck=0");
  if (m_bSaveDBLog)
  {
    Log(Buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      while (true)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }

        ret = SQLGetData(m_hStmtSelect, 1u, 65518, TargetValue, 0, &StrLen_or_IndPtr);
        if (TargetValue[0] < dwMax)
        {
          ret = SQLGetData(m_hStmtSelect, 2u, 65529, v15, 0, &StrLen_or_IndPtr);
          ret = SQLGetData(
            m_hStmtSelect,
            3u,
            4,
            &pPostData[TargetValue[0]].m_dwSenderSerial,
            0,
            &StrLen_or_IndPtr);
          ret = SQLGetData(
            m_hStmtSelect,
            4u,
            1,
            pPostData[TargetValue[0]].m_wszSendName,
            17,
            &StrLen_or_IndPtr);
          ret = SQLGetData(
            m_hStmtSelect,
            5u,
            1,
            pPostData[TargetValue[0]].m_wszRecvName,
            17,
            &StrLen_or_IndPtr);
          ret = SQLGetData(
            m_hStmtSelect,
            6u,
            1,
            pPostData[TargetValue[0]].m_wszTitle,
            21,
            &StrLen_or_IndPtr);
          ret = SQLGetData(
            m_hStmtSelect,
            7u,
            1,
            pPostData[TargetValue[0]].m_wszContent,
            201,
            &StrLen_or_IndPtr);
          ret = SQLGetData(m_hStmtSelect, 8u, 4, pl_nKey, 0, &StrLen_or_IndPtr);
          ret = SQLGetData(m_hStmtSelect, 9u, 65511, &pPostData[TargetValue[0]].m_dwDur, 0, &StrLen_or_IndPtr);
          ret = SQLGetData(m_hStmtSelect, 0xAu, 4, &pPostData[TargetValue[0]].m_dwUpt, 0, &StrLen_or_IndPtr);
          ret = SQLGetData(m_hStmtSelect, 0xBu, 4, &pPostData[TargetValue[0]].m_dwGold, 0, &StrLen_or_IndPtr);
          ret = SQLGetData(m_hStmtSelect, 0xCu, 5, v11, 0, &StrLen_or_IndPtr);
          ret = SQLGetData(m_hStmtSelect, 0xDu, 5, v12, 0, &StrLen_or_IndPtr);
          ret = SQLGetData(
            m_hStmtSelect,
            0xEu,
            65511,
            &pPostData[TargetValue[0]].m_lnUID,
            0,
            &StrLen_or_IndPtr);
          pPostData[TargetValue[0]].m_bySendRace = v11[0];
          pPostData[TargetValue[0]].m_bySenderDgr = v12[0];
          pPostData[TargetValue[0]].m_Key.LoadDBKey(pl_nKey[0]);
          pPostData[TargetValue[0]].m_byState = 0;
        }
      }

      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (m_bSaveDBLog)
      {
        FmtLog("%s Success", Buffer);
      }
      return 0;
    }

    if (ret == 100)
    {
      return 2;
    }

    ErrorMsgLog(ret, Buffer, "SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return 1;
}

char CRFWorldDatabase::Insert_PSDefaultRecord(unsigned int dwCum)
{
  char Buffer[68]{};
  sprintf_s(Buffer, 0x40u, "{ CALL pInsert_PostStorageRecord }");
  for (unsigned int j = 0; j < dwCum; ++j)
  {
    if (!ExecUpdateQuery(Buffer, 1))
    {
      return 0;
    }
  }
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_RaceBossCurrentWinRate(
  unsigned __int8 byRace,
  char *szDate,
  unsigned int *dwTotalCnt,
  unsigned int *dwWinCnt)
{
  SQLLEN StrLen_or_IndPtr = 0;
  SQLRETURN ret = 0;
  char query[256]{};

  sprintf_s(
    query,
    "{ CALL pSelect_BossWinRate(%d, N'%s', '%s') }",
    byRace,
    g_Main.m_wszWorldName,
    szDate);

  if (m_bSaveDBLog)
  {
    Log(query);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        ret = SQLGetData(m_hStmtSelect, 1u, 65518, dwTotalCnt, 0, &StrLen_or_IndPtr);
        if (!ret || ret == 1)
        {
          ret = SQLGetData(m_hStmtSelect, 2u, 65518, dwWinCnt, 0, &StrLen_or_IndPtr);
          if (!ret || ret == 1)
          {
            if (m_hStmtSelect)
            {
              SQLCloseCursor(m_hStmtSelect);
            }
            if (m_bSaveDBLog)
            {
              FmtLog("%s Success", query);
            }
            return 0;
          }
          unsigned __int8 result = 0;
          if (ret == 100)
          {
            result = 2;
          }
          else
          {
            ErrorMsgLog(ret, query, "SQLGetData", m_hStmtSelect);
            ErrorAction(ret, m_hStmtSelect);
            result = 1;
          }
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return result;
        }

        unsigned __int8 result = 0;
        if (ret == 100)
        {
          result = 2;
        }
        else
        {
          ErrorMsgLog(ret, query, "SQLGetData", m_hStmtSelect);
          ErrorAction(ret, m_hStmtSelect);
          result = 1;
        }
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return result;
      }

      unsigned __int8 result = 0;
      if (ret == 100)
      {
        result = 2;
      }
      else
      {
        ErrorMsgLog(ret, query, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = 1;
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return result;
    }

    if (ret == 100)
    {
      return 2;
    }

    ErrorMsgLog(ret, query, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return 1;
}

__int64 CRFWorldDatabase::Select_UnmannedTraderItemStateInfoCnt(unsigned int *pdwCnt)
{
  SQLLEN StrLen_or_IndPtr = 0;
  SQLRETURN ret = 0;
  char Buffer[260]{};

  sprintf_s(Buffer, 0x100u, "select count([id]) from [dbo].[tbl_utresultstateid]");
  if (m_bSaveDBLog)
  {
    Log(Buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        *pdwCnt = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, 4, pdwCnt, 0, &StrLen_or_IndPtr);
        if (ret == 100)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 2;
        }

        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        if (m_bSaveDBLog)
        {
          FmtLog("%s Success", Buffer);
        }
        return 0;
      }

      if (ret != 100)
      {
        ErrorMsgLog(ret, Buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return 1;
    }

    if (ret == 100)
    {
      return 2;
    }

    ErrorMsgLog(ret, Buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return 1;
}

__int64 CRFWorldDatabase::Select_UnmannedTraderItemStateInfo(
  _unmannedtrader_stade_id_info *pkInfo,
  unsigned int dwMaxCnt)
{
  SQLLEN StrLen_or_IndPtr = 0;
  SQLRETURN ret = 0;
  char Buffer[260]{};

  sprintf_s(Buffer, sizeof(Buffer), "select top %u [id], [desc] from [dbo].[tbl_utresultstateid]", dwMaxCnt);
  if (m_bSaveDBLog)
  {
    Log(Buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      for (int j = 0;; ++j)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }

        ret = SQLGetData(m_hStmtSelect, 1u, 65518, &pkInfo[j].dwID, 0, &StrLen_or_IndPtr);
        ret = SQLGetData(m_hStmtSelect, 2u, 65528, pkInfo[j].wszDesc, 128, &StrLen_or_IndPtr);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (m_bSaveDBLog)
      {
        FmtLog("%s Success", Buffer);
      }
      return 0;
    }

    if (ret == 100)
    {
      return 2;
    }

    ErrorMsgLog(ret, Buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return 1;
}

bool CRFWorldDatabase::Truncate_UnmannedTraderItemStateRecord()
{
  char Buffer[1040]{};
  sprintf_s(Buffer, sizeof(Buffer), "truncate table [dbo].[tbl_utresultstateid]");
  return ExecUpdateQuery(Buffer, 1);
}

char CRFWorldDatabase::Insert_UnmannedTraderItemStateRecord(unsigned int dwRowCnt, wchar_t **ppwszStr)
{
  if (!dwRowCnt)
  {
    return 0;
  }

  SetAutoCommitMode(0);
  for (unsigned int j = 0; j < dwRowCnt; ++j)
  {
    wchar_t query[1026]{};
    swprintf_s(
      query,
      L"insert into [dbo].[tbl_utresultstateid] ([id], [desc]) values ( %u, N'%s' )",
      j,
      ppwszStr[j]);
    if (!ExecUpdateQuery(query, 1))
    {
      RollbackTransaction();
      SetAutoCommitMode(1);
      return 0;
    }
  }
  CommitTransaction();
  SetAutoCommitMode(1);
  return 1;
}

bool CRFWorldDatabase::Update_UnmannedTraderClearDanglingOwnerRecord()
{
  char Buffer[1040]{};
  sprintf_s(
    Buffer,
    "update [dbo].[tbl_utresultinfo] set [dbo].[tbl_utresultinfo].state = 0 from ( select s.type, s.serial, s.owner, r.st"
    "ate from [dbo].[tbl_utsellinfo] as s join [dbo].[tbl_utresultinfo] as r on s.owner <> 0 and r.type = s.type and s.se"
    "rial = r.serial join [dbo].[tbl_base] as b on s.owner = b.serial and b.dck = 1 ) as d where [dbo].[tbl_utresultinfo]"
    ".type = d.type and [dbo].[tbl_utresultinfo].serial = d.serial");
  return ExecUpdateQuery(Buffer, 0);
}

__int64 CRFWorldDatabase::Select_UnmannedTraderSingleItemEmptyRecordCnt()
{
  SQLLEN StrLen_or_IndPtr = 0;
  SQLRETURN ret = 0;
  char Buffer[260]{};
  unsigned int TargetValue[7]{};

  sprintf_s(Buffer, sizeof(Buffer), "select count(serial) from tbl_utresultinfo where type = 0 and state = 0");
  if (m_bSaveDBLog)
  {
    Log(Buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        TargetValue[0] = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, 4, TargetValue, 0, &StrLen_or_IndPtr);
        if (ret == 100)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 4294967290LL;
        }

        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        if (m_bSaveDBLog)
        {
          FmtLog("%s Success", Buffer);
        }
        return TargetValue[0];
      }

      unsigned int result = 0;
      if (ret == 100)
      {
        result = 0xFFFFFFFCu;
      }
      else
      {
        ErrorMsgLog(ret, Buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = 0xFFFFFFFBu;
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return result;
    }

    if (ret == 100)
    {
      return 0xFFFFFFFEu;
    }

    ErrorMsgLog(ret, Buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 0xFFFFFFFDu;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return 0xFFFFFFFFLL;
}

__int64 CRFWorldDatabase::select_atrade_taxrate(
  unsigned __int8 byRace,
  char *pwszName,
  unsigned __int8 *byCurrTax,
  unsigned __int8 *byNextTax)
{
  SQLLEN StrLen_or_IndPtr = 0;
  SQLRETURN ret = 0;
  char Buffer[132]{};

  memset(Buffer, 0, 128);
  sprintf(
    Buffer,
    "SELECT TOP 1 SuggesterName, Tax, NextTax FROM [dbo].[tbl_ATradeTaxRate] WHERE Race=%d ORDER BY serial DESC",
    byRace);
  if (m_bSaveDBLog)
  {
    Log(Buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        ret = SQLGetData(m_hStmtSelect, 1u, 1, pwszName, 17, &StrLen_or_IndPtr);
        ret = SQLGetData(m_hStmtSelect, 2u, 65530, byCurrTax, 0, &StrLen_or_IndPtr);
        ret = SQLGetData(m_hStmtSelect, 3u, 65530, byNextTax, 0, &StrLen_or_IndPtr);
        if (!ret || ret == 1)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          if (m_bSaveDBLog)
          {
            FmtLog("%s Success", Buffer);
          }
          return 0;
        }

        unsigned __int8 result = 0;
        if (ret == 100)
        {
          result = 2;
        }
        else
        {
          ErrorMsgLog(ret, Buffer, "SQLExecDirect", m_hStmtSelect);
          ErrorAction(ret, m_hStmtSelect);
          result = 1;
        }
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return result;
      }

      unsigned __int8 result = 0;
      if (ret == 100)
      {
        result = 2;
      }
      else
      {
        ErrorMsgLog(ret, Buffer, "SQLExecDirect", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = 1;
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return result;
    }

    if (ret == 100)
    {
      return 2;
    }

    ErrorMsgLog(ret, Buffer, "SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return 1;
}

char CRFWorldDatabase::Select_CheckGreetRecord(int nUseType)
{
  char Buffer[152]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;
  unsigned int targetValue[4]{};

  sprintf(Buffer, "select count(useType) from tbl_GreetMsg where useType = %d", nUseType);
  if (this->m_bSaveDBLog)
  {
    this->Log(Buffer);
  }

  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(this->m_hStmtSelect);
      if (!ret || ret == 1)
      {
        ret = SQLGetData(this->m_hStmtSelect, 1u, 65530, targetValue, 0, &strLen);
        if (ret != 100 && targetValue[0])
        {
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          if (this->m_bSaveDBLog)
          {
            this->FmtLog("%s Success", Buffer);
          }
          return 0;
        }

        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        return 2;
      }
      else
      {
        char result = 0;
        if (ret == 100)
        {
          result = 2;
        }
        else
        {
          this->ErrorMsgLog(ret, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(ret, this->m_hStmtSelect);
          result = 1;
        }
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        return result;
      }
    }
    else if (ret == 100)
    {
      return 2;
    }
    else
    {
      this->ErrorMsgLog(ret, Buffer, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(ret, this->m_hStmtSelect);
      return 1;
    }
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return 1;
}

bool CRFWorldDatabase::Insert_GreetingRecord(int nUseType, const char *wszName, const char *wszMessage)
{
  char Buffer[528]{};

  sprintf(Buffer, "{CALL pInsert_GreetingMsgRecord_071119(%d,'%s','%s')}", nUseType, wszName, wszMessage);
  return this->ExecUpdateQuery(Buffer, true);
}

char CRFWorldDatabase::LoadGreetingMsg(
  char *pwszGMGreetingmsg,
  char *pwszRaceGreetingmsgA,
  char *pwszRaceGreetingmsgB,
  char *pwszRaceGreetingmsgC,
  char *pwszGMName,
  char *pwszNameA,
  char *pwszNameB,
  char *pwszNameC)
{
  char Buffer[280]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;

  sprintf(Buffer, "select GMsg,Name from tbl_GreetMsg where usetype = %d", 0);
  if (this->m_bSaveDBLog)
  {
    this->Log(Buffer);
  }
  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(this->m_hStmtSelect);
      if (!ret || ret == 1)
      {
        ret = SQLGetData(this->m_hStmtSelect, 1u, 1, pwszRaceGreetingmsgA, 256, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, 2u, 1, pwszNameA, 17, &strLen);
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }

        sprintf(Buffer, "select GMsg,Name from tbl_GreetMsg where usetype = %d", 1);
        if (this->m_bSaveDBLog)
        {
          this->Log(Buffer);
        }
        if (this->m_hStmtSelect || this->ReConnectDataBase())
        {
          ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
          if (!ret || ret == 1)
          {
            ret = SQLFetch(this->m_hStmtSelect);
            if (!ret || ret == 1)
            {
              ret = SQLGetData(this->m_hStmtSelect, 1u, 1, pwszRaceGreetingmsgB, 256, &strLen);
              ret = SQLGetData(this->m_hStmtSelect, 2u, 1, pwszNameB, 17, &strLen);
              if (this->m_hStmtSelect)
              {
                SQLCloseCursor(this->m_hStmtSelect);
              }

              sprintf(Buffer, "select GMsg,Name from tbl_GreetMsg where usetype = %d", 2);
              if (this->m_bSaveDBLog)
              {
                this->Log(Buffer);
              }
              if (this->m_hStmtSelect || this->ReConnectDataBase())
              {
                ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
                if (!ret || ret == 1)
                {
                  ret = SQLFetch(this->m_hStmtSelect);
                  if (!ret || ret == 1)
                  {
                    ret = SQLGetData(this->m_hStmtSelect, 1u, 1, pwszRaceGreetingmsgC, 256, &strLen);
                    ret = SQLGetData(this->m_hStmtSelect, 2u, 1, pwszNameC, 17, &strLen);
                    if (this->m_hStmtSelect)
                    {
                      SQLCloseCursor(this->m_hStmtSelect);
                    }

                    sprintf(Buffer, "select GMsg,Name from tbl_GreetMsg where usetype = %d", 255);
                    if (this->m_bSaveDBLog)
                    {
                      this->Log(Buffer);
                    }
                    if (this->m_hStmtSelect || this->ReConnectDataBase())
                    {
                      ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
                      if (!ret || ret == 1)
                      {
                        ret = SQLFetch(this->m_hStmtSelect);
                        if (!ret || ret == 1)
                        {
                          ret = SQLGetData(this->m_hStmtSelect, 1u, 1, pwszGMGreetingmsg, 256, &strLen);
                          ret = SQLGetData(this->m_hStmtSelect, 2u, 1, pwszGMName, 17, &strLen);
                          if (this->m_hStmtSelect)
                          {
                            SQLCloseCursor(this->m_hStmtSelect);
                          }
                          if (this->m_bSaveDBLog)
                          {
                            this->FmtLog("%s Success", Buffer);
                          }
                          return 1;
                        }
                        else
                        {
                          if (ret != 100)
                          {
                            this->ErrorMsgLog(ret, Buffer, "SQLFetch", this->m_hStmtSelect);
                            this->ErrorAction(ret, this->m_hStmtSelect);
                          }
                          if (this->m_hStmtSelect)
                          {
                            SQLCloseCursor(this->m_hStmtSelect);
                          }
                          return 0;
                        }
                      }
                      else
                      {
                        if (ret != 100)
                        {
                          this->ErrorMsgLog(ret, Buffer, "SQLExecDirect", this->m_hStmtSelect);
                          this->ErrorAction(ret, this->m_hStmtSelect);
                        }
                        return 0;
                      }
                    }
                    else
                    {
                      this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
                      return 0;
                    }
                  }
                  else
                  {
                    if (ret != 100)
                    {
                      this->ErrorMsgLog(ret, Buffer, "SQLFetch", this->m_hStmtSelect);
                      this->ErrorAction(ret, this->m_hStmtSelect);
                    }
                    if (this->m_hStmtSelect)
                    {
                      SQLCloseCursor(this->m_hStmtSelect);
                    }
                    return 0;
                  }
                }
                else
                {
                  if (ret != 100)
                  {
                    this->ErrorMsgLog(ret, Buffer, "SQLExecDirect", this->m_hStmtSelect);
                    this->ErrorAction(ret, this->m_hStmtSelect);
                  }
                  return 0;
                }
              }
              else
              {
                this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
                return 0;
              }
            }
            else
            {
              if (ret != 100)
              {
                this->ErrorMsgLog(ret, Buffer, "SQLFetch", this->m_hStmtSelect);
                this->ErrorAction(ret, this->m_hStmtSelect);
              }
              if (this->m_hStmtSelect)
              {
                SQLCloseCursor(this->m_hStmtSelect);
              }
              return 0;
            }
          }
          else
          {
            if (ret != 100)
            {
              this->ErrorMsgLog(ret, Buffer, "SQLExecDirect", this->m_hStmtSelect);
              this->ErrorAction(ret, this->m_hStmtSelect);
            }
            return 0;
          }
        }
        else
        {
          this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
          return 0;
        }
      }
      else
      {
        if (ret != 100)
        {
          this->ErrorMsgLog(ret, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(ret, this->m_hStmtSelect);
        }
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        return 0;
      }
    }
    else
    {
      if (ret != 100)
      {
        this->ErrorMsgLog(ret, Buffer, "SQLExecDirect", this->m_hStmtSelect);
        this->ErrorAction(ret, this->m_hStmtSelect);
      }
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 0;
  }
}

unsigned __int8 CRFWorldDatabase::Select_LimitInfo(unsigned __int8 *pData, unsigned __int64 tDataSize)
{
  char query[1024]{};
  unsigned __int16 columnNumber[14]{};

  sprintf_s(query, sizeof(query), "select [effect] from [dbo].[tbl_sf_delay] where [aserial] = 0");
  unsigned __int8 ret = SQLExecDirect_RetErrCode(query);
  if (ret)
  {
    return ret;
  }
  ret = SQLFetch_RetErrCode(query);
  if (ret)
  {
    return ret;
  }
  columnNumber[0] = 1;
  ret = SQLGetData_Binary_RetErrCode(query, columnNumber, pData, tDataSize);
  if (ret)
  {
    return ret;
  }
  SelectCleanUp(query);
  return 0;
}

char CRFWorldDatabase::Select_BattleTournamentInfo(TournamentWinner *pWinnerInfo, int nMax)
{
  char Buffer[152]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;

  sprintf_s(
    Buffer,
    sizeof(Buffer),
    "Select top %d CharacterSerial,CharacterName,BattleWinGrade from tbl_battletournament",
    nMax);
  if (this->m_bSaveDBLog)
  {
    this->Log(Buffer);
  }

  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      for (int j = 0; ; ++j)
      {
        ret = SQLFetch(this->m_hStmtSelect);
        if (ret == 100)
        {
          break;
        }
        if (ret && ret != 1)
        {
          this->ErrorMsgLog(ret, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(ret, this->m_hStmtSelect);
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          return 0;
        }
        if (j >= nMax)
        {
          break;
        }
        ret = SQLGetData(this->m_hStmtSelect, 1u, 4, &pWinnerInfo[j], 0, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, 2u, 1, pWinnerInfo[j].wszCharName, 17, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, 3u, 65530, &pWinnerInfo[j].byGrade, 0, &strLen);
        if (ret && ret != 1)
        {
          if (ret != 100)
          {
            this->ErrorMsgLog(ret, Buffer, "SQLGetData", this->m_hStmtSelect);
            this->ErrorAction(ret, this->m_hStmtSelect);
          }
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          return 0;
        }
      }
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      if (this->m_bSaveDBLog)
      {
        this->FmtLog("%s Success", Buffer);
      }
      return 1;
    }
    else if (ret == 100)
    {
      return 1;
    }
    else
    {
      this->ErrorMsgLog(ret, Buffer, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(ret, this->m_hStmtSelect);
      return 0;
    }
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return 0;
}

__int64 CRFWorldDatabase::Select_GodenBoxItem(_worlddb_golden_box_item *goldenboxitem, int *pnSerial)
{
  char query[1024]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;

  sprintf_s(query, sizeof(query), "{ CALL pSelect_GoldBoxItem }");
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(this->m_hStmtSelect);
      if (!ret || ret == 1)
      {
        int column = 1;
        ret = SQLGetData(this->m_hStmtSelect, 1u, 4, pnSerial, 0, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, ++column, 65530, &goldenboxitem->bydck, 0, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, ++column, 4, &goldenboxitem->dwStarterBoxCnt, 0, &strLen);
        for (int j = 0; ; ++j)
        {
          const unsigned __int8 loopCount = CGoldenBoxItemMgr::Instance()->GetLoopCount();
          if (j >= loopCount)
          {
            break;
          }
          int maxCount = -1;
          ret = SQLGetData(
            this->m_hStmtSelect,
            ++column,
            4,
            &goldenboxitem->nBox_item_code[j],
            0,
            &strLen);
          ret = SQLGetData(this->m_hStmtSelect, ++column, 4, &maxCount, 0, &strLen);
          goldenboxitem->wBox_item_max[j] = static_cast<unsigned __int16>(maxCount);
          ret = SQLGetData(
            this->m_hStmtSelect,
            ++column,
            65530,
            &goldenboxitem->bygolden_item_num[j],
            0,
            &strLen);
        }
        for (int k = 0; ; ++k)
        {
          const unsigned __int8 loopCount = CGoldenBoxItemMgr::Instance()->GetLoopCount();
          if (k >= loopCount)
          {
            break;
          }
          for (int m = 0; m < 100; ++m)
          {
            ret = SQLGetData(
              this->m_hStmtSelect,
              ++column,
              4,
              &goldenboxitem->List[k][m],
              0,
              &strLen);
            ret = SQLGetData(
              this->m_hStmtSelect,
              ++column,
              4,
              &goldenboxitem->List[k][m].wGoldencount,
              0,
              &strLen);
          }
        }
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        if (this->m_bSaveDBLog)
        {
          this->FmtLog("%s Success", query);
        }
        return 0;
      }
      else
      {
        unsigned __int8 result = 0;
        if (ret == 100)
        {
          result = 2;
        }
        else
        {
          this->ErrorMsgLog(ret, query, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(ret, this->m_hStmtSelect);
          result = 1;
        }
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        return result;
      }
    }
    else if (ret == 100)
    {
      return 2;
    }
    else
    {
      this->ErrorMsgLog(ret, query, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(ret, this->m_hStmtSelect);
      return 1;
    }
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return 1;
}

char CRFWorldDatabase::Select_GuildRoomInfo(_guildroom_info *pInfo)
{
  char query[256]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;
  unsigned __int8 count = 0;

  memset_0(query, 0, sizeof(query));
  sprintf_s(
    query,
    "select gr.guildserial, g.id, gr.roomtype, gr.roomrace, gr.roomrentdate from tbl_GuildRoom as gr inner join tbl_Guild"
    " as g on gr.guildserial = g.serial where gr.dck = 0 order by g.serial desc");

  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }

  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
    if (!ret || ret == 1)
    {
      count = 0;
      do
      {
        ret = SQLFetch(this->m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }
        ret = SQLGetData(this->m_hStmtSelect, 1u, 65518, &pInfo->info[count], 0, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, 2u, 1, pInfo->info[count].uszGuildName, 17, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, 3u, 65530, &pInfo->info[count].byRoomType, 0, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, 4u, 65530, &pInfo->info[count].byRace, 0, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, 5u, 93, &pInfo->info[count++].ts, 0, &strLen);
        if (count >= 0x5Au)
        {
          break;
        }
      } while (!ret || ret == 1);

      pInfo->byCount = count;
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      if (this->m_bSaveDBLog)
      {
        this->FmtLog("%s Success", query);
      }
      return 1;
    }
    else
    {
      if (ret != 100)
      {
        this->ErrorMsgLog(ret, query, "SQLExecDirect", this->m_hStmtSelect);
        this->ErrorAction(ret, this->m_hStmtSelect);
      }
      return 0;
    }
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return 0;
}

char CRFWorldDatabase::Select_UsedLimitItemRecordNum(unsigned int *pdwUsedNum)
{
  char query[132]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;

  memset(query, 0, 128);
  sprintf_s(query, 0x80uLL, "select count(serial) from tbl_StoreLimitItem_061212 where dck=0");
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }

  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(this->m_hStmtSelect);
      if (!ret || ret == 1)
      {
        ret = SQLGetData(this->m_hStmtSelect, 1u, 65518, pdwUsedNum, 0, &strLen);
        if (!ret || ret == 1)
        {
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          if (this->m_bSaveDBLog)
          {
            this->FmtLog("%s Success", query);
          }
          return 0;
        }

        this->ErrorMsgLog(ret, query, "SQLGetData", this->m_hStmtSelect);
        this->ErrorAction(ret, this->m_hStmtSelect);
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        return 1;
      }

      unsigned __int8 result = 0;
      if (ret == 100)
      {
        result = 2;
      }
      else
      {
        this->ErrorMsgLog(ret, query, "SQLFetch", this->m_hStmtSelect);
        this->ErrorAction(ret, this->m_hStmtSelect);
        result = 1;
      }
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      return result;
    }

    if (ret == 100)
    {
      return 2;
    }

    this->ErrorMsgLog(ret, query, "SQLExecDirect", this->m_hStmtSelect);
    this->ErrorAction(ret, this->m_hStmtSelect);
    return 1;
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return 1;
}

char CRFWorldDatabase::Select_TotalRecordNum(unsigned int *pdwTotalNum)
{
  char query[132]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;

  memset(query, 0, 128);
  sprintf_s(query, 0x80uLL, "select count(serial) from tbl_StoreLimitItem_061212");
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }

  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(this->m_hStmtSelect);
      if (!ret || ret == 1)
      {
        ret = SQLGetData(this->m_hStmtSelect, 1u, 65518, pdwTotalNum, 0, &strLen);
        if (!ret || ret == 1)
        {
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          if (this->m_bSaveDBLog)
          {
            this->FmtLog("%s Success", query);
          }
          return 0;
        }

        this->ErrorMsgLog(ret, query, "SQLGetData", this->m_hStmtSelect);
        this->ErrorAction(ret, this->m_hStmtSelect);
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        return 1;
      }

      unsigned __int8 result = 0;
      if (ret == 100)
      {
        result = 2;
      }
      else
      {
        this->ErrorMsgLog(ret, query, "SQLFetch", this->m_hStmtSelect);
        this->ErrorAction(ret, this->m_hStmtSelect);
        result = 1;
      }
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      return result;
    }

    if (ret == 100)
    {
      return 2;
    }

    this->ErrorMsgLog(ret, query, "SQLExecDirect", this->m_hStmtSelect);
    this->ErrorAction(ret, this->m_hStmtSelect);
    return 1;
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_StoreLimitItem(_qry_case_all_store_limit_item *pData)
{
  char query[516]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;
  int j = 0;
  unsigned int count = 0;
  int targetValue[8]{};
  int limitValues[4]{};
  const int maxItems = 16;

  memset(query, 0, 512);
  sprintf_s(
    query,
    0x200uLL,
    "select k0,k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12,k13,k14,k15,num0,num1,num2,num3,num4,num5,num6,num7,num8,num9,num10"
    ",num11,num12,num13,num14,num15,serial,type,typeserial,storeinx,resettime from tbl_StoreLimitItem_061212 where dck=0");

  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }

  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
    if (!ret || ret == 1)
    {
      count = 0;
      while (true)
      {
        ret = SQLFetch(this->m_hStmtSelect);
        if (ret && ret != 1)
        {
          pData->dwCount = count;
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          if (this->m_bSaveDBLog)
          {
            this->FmtLog("%s Success", query);
          }
          return 0;
        }

        for (j = 1; j <= maxItems; ++j)
        {
          ret = SQLGetData(this->m_hStmtSelect, j, 4, targetValue, 0, &strLen);
          if (ret && ret != 1)
          {
            this->ErrorMsgLog(ret, query, "SQLExecDirect", this->m_hStmtSelect);
            this->ErrorAction(ret, this->m_hStmtSelect);
            if (this->m_hStmtSelect)
            {
              SQLCloseCursor(this->m_hStmtSelect);
            }
            return 1;
          }
          pData->pStoreList[count].ItemData[j - 1].Key.LoadDBKey(targetValue[0]);
        }

        while (true)
        {
          if (j > 2 * maxItems)
          {
            ret = SQLGetData(this->m_hStmtSelect, j, 65518, &pData->pStoreList[count], 0, &strLen);
            ret = SQLGetData(this->m_hStmtSelect, j + 1, 5, &pData->pStoreList[count].byType, 0, &strLen);
            ret = SQLGetData(this->m_hStmtSelect, j + 2, 65518, &pData->pStoreList[count].nTypeSerial, 0, &strLen);
            ret = SQLGetData(this->m_hStmtSelect, j + 3, 65518, &pData->pStoreList[count].dwStoreIndex, 0, &strLen);
            ret = SQLGetData(
              this->m_hStmtSelect,
              j + 4,
              65511,
              &pData->pStoreList[count++].dwLimitInitTime,
              0,
              &strLen);
            break;
          }

          ret = SQLGetData(this->m_hStmtSelect, j, 4, limitValues, 0, &strLen);
          if (ret && ret != 1)
          {
            this->ErrorMsgLog(ret, query, "SQLExecDirect", this->m_hStmtSelect);
            this->ErrorAction(ret, this->m_hStmtSelect);
            if (this->m_hStmtSelect)
            {
              SQLCloseCursor(this->m_hStmtSelect);
            }
            return 1;
          }
          pData->pStoreList[count].ItemData[j - maxItems - 1].nLimitNum = limitValues[0];
          ++j;
        }
      }
    }

    if (ret == 100)
    {
      return 2;
    }

    this->ErrorMsgLog(ret, query, "SQLExecDirect", this->m_hStmtSelect);
    this->ErrorAction(ret, this->m_hStmtSelect);
    return 1;
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return 1;
}

bool CRFWorldDatabase::Insert_GoldenBoxItem()
{
  char Buffer[1040]{};

  sprintf_s(Buffer, sizeof(Buffer), "{ CALL pInsert_GoldBoxItem( %d ) }", 0);
  return this->ExecUpdateQuery(Buffer, true);
}

bool CRFWorldDatabase::Insert_DefaultWeeklyGuildPvpPointSumRecord()
{
  char Buffer[272]{};

  sprintf(Buffer, "{ CALL pInsert_DefaultRecordWeeklyGuildPVPPointSum }");
  return this->ExecUpdateQuery(Buffer, false);
}

unsigned __int8 CRFWorldDatabase::Select_Economy_History(
  _worlddb_economy_history_info_array *pEconomyData,
  unsigned int dwDate)
{
  SQLLEN StrLen_or_IndPtr{};
  char Buffer[260]{};
  int rowCount = 0;

  sprintf(Buffer, "{ CALL pSelect_Last_Economy_History ( %d )}", dwDate);
  if (m_bSaveDBLog)
  {
    Log(Buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if (!ret || ret == 1)
    {
      while (true)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }

        ret = SQLGetData(
          m_hStmtSelect,
          1u,
          8,
          pEconomyData->EconomyData[rowCount].dTradeDalant,
          0LL,
          &StrLen_or_IndPtr);
        ret = SQLGetData(
          m_hStmtSelect,
          2u,
          8,
          &pEconomyData->EconomyData[rowCount].dTradeDalant[1],
          0LL,
          &StrLen_or_IndPtr);
        ret = SQLGetData(
          m_hStmtSelect,
          3u,
          8,
          &pEconomyData->EconomyData[rowCount].dTradeDalant[2],
          0LL,
          &StrLen_or_IndPtr);
        ret = SQLGetData(
          m_hStmtSelect,
          4u,
          8,
          &pEconomyData->EconomyData[rowCount],
          0LL,
          &StrLen_or_IndPtr);
        ret = SQLGetData(
          m_hStmtSelect,
          5u,
          8,
          &pEconomyData->EconomyData[rowCount].dTradeGold[1],
          0LL,
          &StrLen_or_IndPtr);
        ret = SQLGetData(
          m_hStmtSelect,
          6u,
          8,
          &pEconomyData->EconomyData[rowCount].dTradeGold[2],
          0LL,
          &StrLen_or_IndPtr);
        ret = SQLGetData(
          m_hStmtSelect,
          7u,
          4,
          &pEconomyData->EconomyData[rowCount].dwManageValue,
          0LL,
          &StrLen_or_IndPtr);

        SQLUSMALLINT column = 8;
        for (int j = 0; j < 3; ++j)
        {
          for (int k = 0; k < 3; ++k)
          {
            ret = SQLGetData(
              m_hStmtSelect,
              column++,
              8,
              &pEconomyData->EconomyData[rowCount].dMineOre[k][j],
              0LL,
              &StrLen_or_IndPtr);
            if (ret)
            {
              if (ret != 1)
              {
                break;
              }
            }
          }
        }

        for (int j = 0; j < 3; ++j)
        {
          for (int m = 0; m < 3; ++m)
          {
            ret = SQLGetData(
              m_hStmtSelect,
              column++,
              8,
              &pEconomyData->EconomyData[rowCount].dCutOre[m][j],
              0LL,
              &StrLen_or_IndPtr);
            if (ret)
            {
              if (ret != 1)
              {
                break;
              }
            }
          }
        }

        ++rowCount;
        if (ret)
        {
          if (ret != 1)
          {
            break;
          }
        }
      }

      pEconomyData->wRowCount = static_cast<unsigned __int16>(rowCount);
      if (m_hStmtUpdate)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (m_bSaveDBLog)
      {
        FmtLog("%s Success", Buffer);
      }
      return 0;
    }
    else if (ret == 100)
    {
      return 2;
    }
    else
    {
      ErrorMsgLog(ret, Buffer, "SQLExecDirectA", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
      return 1;
    }
  }
  else
  {
    ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1;
  }
}

bool CRFWorldDatabase::UpdateClearGuildBattleInfo(unsigned int dwStartID, unsigned int dwEndID)
{
  char buffer[1040]{};
  sprintf(buffer, "{ CALL pUpdate_ClearGuildBattleInfo( %u, %u ) }", dwStartID, dwEndID);
  return ExecUpdateQuery(buffer, 1);
}

bool CRFWorldDatabase::UpdateWinGuildBattleResult(unsigned int dwGuildSerial, unsigned int dwScore)
{
  char buffer[1040]{};
  sprintf(buffer, "{ CALL pUpdate_WinGuildBattleRank( %u, %u ) }", dwGuildSerial, dwScore);
  return ExecUpdateQuery(buffer, 1);
}

bool CRFWorldDatabase::UpdateLoseGuildBattleResult(unsigned int dwGuildSerial, unsigned int dwScore)
{
  char buffer[1040]{};
  sprintf(buffer, "{ CALL pUpdate_LoseGuildBattleRank( %u, %u ) }", dwGuildSerial, dwScore);
  return ExecUpdateQuery(buffer, 1);
}

bool CRFWorldDatabase::UpdateDrawGuildBattleResult(unsigned int dwGuildSerial, unsigned int dwScore)
{
  char buffer[1040]{};
  sprintf(buffer, "{ CALL pUpdate_DrawGuildBattleRank( %u, %u ) }", dwGuildSerial, dwScore);
  return ExecUpdateQuery(buffer, 1);
}

char CRFWorldDatabase::SelectGuildBattleRankRecord(unsigned int dwGuildSerial)
{
  SQLLEN strLenOrInd = 0;
  char buffer[1044]{};
  sprintf(buffer, "{ CALL pSelect_GuildBattleRank(%u) }", dwGuildSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        unsigned int value = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, &value, 0, &strLenOrInd);
        if (ret == SQL_NO_DATA)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 0;
        }

        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        if (m_bSaveDBLog)
        {
          FmtLog("%s Success", buffer);
        }
        return 1;
      }

      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return 0;
    }

    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0;
}

bool CRFWorldDatabase::InsertGuildBattleRankRecord(unsigned int dwGuildSerial)
{
  char buffer[1040]{};
  sprintf(buffer, "{ CALL pInsert_GuildBattleRank( %u ) }", dwGuildSerial);
  return ExecUpdateQuery(buffer, 1);
}

char CRFWorldDatabase::SelectGuildBattleRankList(
  unsigned __int8 byRace,
  _worlddb_guild_battle_rank_list *pkInfo)
{
  if (!pkInfo)
  {
    return 0;
  }

  char buffer[1048]{};
  SQLLEN strLenOrInd = 0;
  sprintf(buffer, "{ CALL pSelect_GuildBattleRankList(%u) }", byRace);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == 1)
    {
      pkInfo->wCount = 0;
      unsigned __int16 rank = 1;
      for (;; pkInfo->list[pkInfo->wCount++].nRank = rank)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }

        SQLGetData(m_hStmtSelect, 1u, 65518, &pkInfo->list[pkInfo->wCount].dwSerial, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 2u, 65530, &pkInfo->list[pkInfo->wCount].byGrade, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 3u, 1, pkInfo->list[pkInfo->wCount].wszName, 17, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 4u, 65518, &pkInfo->list[pkInfo->wCount].dwWin, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 5u, 65518, &pkInfo->list[pkInfo->wCount].dwDraw, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 6u, 65518, &pkInfo->list[pkInfo->wCount].dwLose, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 7u, 65518, &pkInfo->list[pkInfo->wCount].dwScore, 0, &strLenOrInd);

        if (pkInfo->wCount)
        {
          const unsigned __int16 prevIndex = pkInfo->wCount - 1;
          if (pkInfo->list[prevIndex].dwScore != pkInfo->list[pkInfo->wCount].dwScore
              || pkInfo->list[prevIndex].dwWin != pkInfo->list[pkInfo->wCount].dwWin
              || pkInfo->list[prevIndex].dwDraw != pkInfo->list[pkInfo->wCount].dwDraw
              || pkInfo->list[prevIndex].dwLose != pkInfo->list[pkInfo->wCount].dwLose)
          {
            rank = pkInfo->wCount + 1;
          }
        }
      }

      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (m_bSaveDBLog)
      {
        FmtLog("%s Success", buffer);
      }
      return 1;
    }
    if (ret == 100)
    {
      return 1;
    }

    ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0;
}

char CRFWorldDatabase::SelectGuildBattleRerservedList(
  unsigned int uiStartSLID,
  unsigned int uiEndSLID,
  _worlddb_guild_battle_reserved_schedule_info *pkInfo)
{
  if (!pkInfo)
  {
    return 0;
  }

  SQLLEN strLenOrInd = 0;
  char buffer[1028]{};
  unsigned int count = 0;

  sprintf(buffer, "{ CALL pSelect_GuildBattleReservedSchedule( %u, %u ) }", uiStartSLID, uiEndSLID);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == 1)
    {
      do
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }

        SQLGetData(m_hStmtSelect, 1u, 65518, &pkInfo->list[count], 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 2u, 1, pkInfo->list[count].wsz1PName, 17, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 3u, 65530, &pkInfo->list[count].by1PRace, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 4u, 65518, &pkInfo->list[count].dw2PGuildSerial, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 5u, 1, pkInfo->list[count].wsz2PName, 17, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 6u, 65530, &pkInfo->list[count].by2PRace, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 7u, 65530, &pkInfo->list[count].byStartHour, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 8u, 65530, &pkInfo->list[count].byStartMin, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 9u, 65530, &pkInfo->list[count].byEndHour, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 10u, 65530, &pkInfo->list[count++].byEndMin, 0, &strLenOrInd);
      }
      while (count < 0x2E);

      pkInfo->wCount = count;
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (m_bSaveDBLog)
      {
        FmtLog("%s Success", buffer);
      }
      return 1;
    }

    if (ret != 100)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0;
}

bool CRFWorldDatabase::UpdateClearGuildBattleScheduleInfo(unsigned int uiStartListID, unsigned int uiEndListID)
{
  char buffer[1040]{};
  sprintf(buffer, "{ CALL pUpdate_ClearGuildBattleScheduleInfo( %u, %u ) }", uiStartListID, uiEndListID);
  return ExecUpdateQuery(buffer, 1);
}

__int64 CRFWorldDatabase::SelectRowCountGuildBattleInfo()
{
  SQLLEN strLenOrInd = 0;
  char buffer[1028]{};

  sprintf(buffer, "select count(id) from [dbo].[tbl_ReservedGuildBattleInfo]");
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        unsigned int count = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, 4, &count, 0, &strLenOrInd);
        if (ret == 100)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 0xFFFFFFFA;
        }

        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        if (m_bSaveDBLog)
        {
          FmtLog("%s Success", buffer);
        }
        return count;
      }

      unsigned int result = 0;
      if (ret == 100)
      {
        result = static_cast<unsigned int>(-4);
      }
      else
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = static_cast<unsigned int>(-5);
      }

      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return result;
    }
    if (ret == 100)
    {
      return 0xFFFFFFFE;
    }

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 0xFFFFFFFD;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return static_cast<__int64>(0xFFFFFFFF);
}

__int64 CRFWorldDatabase::SelectRowCountGuildBattleScheduleInfo()
{
  SQLLEN strLenOrInd = 0;
  char buffer[260]{};

  sprintf(buffer, "select count(id) from [dbo].[tbl_GuildBattleScheduleInfo]");
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        unsigned int count = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, 4, &count, 0, &strLenOrInd);
        if (ret == 100)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 0xFFFFFFFB;
        }

        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        if (m_bSaveDBLog)
        {
          FmtLog("%s Success", buffer);
        }
        return count;
      }

      unsigned int result = 0;
      if (ret == 100)
      {
        result = static_cast<unsigned int>(-3);
      }
      else
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = static_cast<unsigned int>(-4);
      }

      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return result;
    }
    if (ret == 100)
    {
      return 0xFFFFFFFE;
    }

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return static_cast<__int64>(0xFFFFFFFF);
}

char CRFWorldDatabase::LoadGuildBattleInfo(
  unsigned int dwStartID,
  unsigned int dwRowCnt,
  _worlddb_guild_battle_info *pkInfo)
{
  if (!pkInfo)
  {
    return 0;
  }

  SQLLEN strLenOrInd = 0;
  char buffer[1028]{};
  unsigned int count = 0;

  sprintf(buffer, "{ CALL pSelect_ReservedGuildBattleInfo(%u,%u) }", dwStartID, dwRowCnt + dwStartID);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == 1)
    {
      do
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }
        SQLGetData(m_hStmtSelect, 1u, 65518, &pkInfo->list[count], 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 2u, 65518, &pkInfo->list[count].dwP1GuildSerial, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 3u, 65518, &pkInfo->list[count].dwP2GuildSerial, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 4u, 65518, &pkInfo->list[count].dwMapID, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 5u, 65530, &pkInfo->list[count++].byNumber, 0, &strLenOrInd);
      }
      while (dwRowCnt > count);

      pkInfo->wCount = count;
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (m_bSaveDBLog)
      {
        FmtLog("%s Success", buffer);
      }
      return 1;
    }

    if (ret != 100)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0;
}

unsigned __int8 CRFWorldDatabase::LoadGuildBattleScheduleInfo(
  unsigned int uiStartListID,
  unsigned int uiScheduleUnitCnt,
  _worlddb_guild_battle_schedule_list *pkInfo)
{
  if (!uiScheduleUnitCnt || !pkInfo)
  {
    return 0;
  }

  SQLLEN strLenOrInd = 0;
  char buffer[1028]{};

  sprintf(buffer, "{ CALL pSelect_ReservedGuildBattleSchedule( %u, %u ) }", uiStartListID, uiStartListID);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == 1)
    {
      unsigned int count = 0;
      pkInfo->wCount = 0;
      do
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }
        SQLGetData(m_hStmtSelect, 1u, 65518, &pkInfo->list[count], 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 2u, 65518, &pkInfo->list[count].dwSLID, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 3u, 65530, &pkInfo->list[count].ucState, 0, &strLenOrInd);

        _WORD targetValue[24]{};
        SQLGetData(m_hStmtSelect, 4u, 93, targetValue, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 5u, 65519, &pkInfo->list[count].wTumeMin, 0, &strLenOrInd);

        tm timeInfo{};
        timeInfo.tm_year = targetValue[0] - 1900;
        timeInfo.tm_mon = targetValue[1] - 1;
        timeInfo.tm_mday = targetValue[2];
        timeInfo.tm_hour = targetValue[3];
        timeInfo.tm_min = targetValue[4];
        timeInfo.tm_sec = targetValue[5];
        timeInfo.tm_isdst = -1;
        __time64_t timestamp = mktime_3(&timeInfo);
        if (timestamp == -1)
        {
          timestamp = 0;
        }
        pkInfo->list[count++].tStartTime = timestamp;
        pkInfo->wCount = count;
      }
      while (uiScheduleUnitCnt > count);

      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (m_bSaveDBLog)
      {
        FmtLog("%s Success", buffer);
      }
      return 0;
    }

    if (ret == 100)
    {
      return 2;
    }

    ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

bool CRFWorldDatabase::DeleteGuildBattleInfo()
{
  char buffer[1040]{};
  sprintf(buffer, "truncate table tbl_ReservedGuildBattleInfo");
  return ExecUpdateQuery(buffer, 1);
}

bool CRFWorldDatabase::DeleteGuildBattleScheduleInfo()
{
  char buffer[1040]{};
  sprintf(buffer, "truncate table [dbo].[tbl_GuildBattleScheduleInfo]");
  return ExecUpdateQuery(buffer, 1);
}

char CRFWorldDatabase::InsertGuildBattleDefaultRecord(unsigned int dwRowCnt)
{
  if (!dwRowCnt)
  {
    return 0;
  }
  if (!DeleteGuildBattleInfo())
  {
    return 0;
  }

  char buffer[1028]{};
  for (unsigned int j = 0; j < dwRowCnt; ++j)
  {
    sprintf(buffer, "{ CALL pInsert_GuildBattleInfo( %u, 0, 0, 0, 0 ) }", j);
    if (!ExecUpdateQuery(buffer, 1))
    {
      DeleteGuildBattleInfo();
      return 0;
    }
  }
  return 1;
}

char CRFWorldDatabase::InsertGuildBattleScheduleDefaultRecord(
  unsigned int uiDayCnt,
  unsigned int uiMapCnt,
  unsigned __int8 byMaxHour,
  unsigned __int8 byUnitTimeCntPerTime)
{
  if (!uiDayCnt || !uiMapCnt || !byUnitTimeCntPerTime)
  {
    return 0;
  }
  if (uiDayCnt > 0x1E || byMaxHour > 0x18u)
  {
    return 0;
  }
  if (!DeleteGuildBattleScheduleInfo())
  {
    return 0;
  }

  char buffer[1028]{};
  const unsigned int unitPerDay = byUnitTimeCntPerTime * byMaxHour;
  for (unsigned int j = 0; j < uiDayCnt; ++j)
  {
    for (unsigned int k = 0; k < uiMapCnt; ++k)
    {
      const unsigned int slid = k + uiMapCnt * j;
      for (unsigned int m = 0; m < unitPerDay; ++m)
      {
        const unsigned int id = j * unitPerDay * uiMapCnt + m + k * unitPerDay;
        sprintf(
          buffer,
          "INSERT INTO [dbo].[tbl_GuildBattleScheduleInfo]( [ID], [SLID], [State], [StartTime], [BattleTurm] )VALUES( %d, %d, 0, 0, 0 )",
          id,
          slid);
        if (!ExecUpdateQuery(buffer, 1))
        {
          DeleteGuildBattleScheduleInfo();
          return 0;
        }
      }
    }
  }
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_CharNumInWorld(unsigned int dwAccountSerial,
        unsigned __int8 *byCharNum)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-188h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-150h] BYREF
  SQLRETURN v8; // [rsp+44h] [rbp-144h]
  char Buffer[260]; // [rsp+60h] [rbp-128h] BYREF
  int v10; // [rsp+164h] [rbp-24h]
  int v11; // [rsp+168h] [rbp-20h]
  char v12; // [rsp+16Ch] [rbp-1Ch]
  char v13; // [rsp+16Dh] [rbp-1Bh]
  v10 = 0;
  v11 = 0;
  sprintf(Buffer, "{ CALL pSelect_CharNumInWorld( %d ) }", dwAccountSerial);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v8 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v8 || v8 == 1 )
    {
      v8 = SQLFetch(this->m_hStmtSelect);
      if ( !v8 || v8 == 1 )
      {
        v8 = SQLGetData(this->m_hStmtSelect, 1u, 65530, byCharNum, 0LL, &StrLen_or_IndPtr);
        if ( !v8 || v8 == 1 )
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          if ( this->m_bSaveDBLog )
            this->FmtLog("%s Success", Buffer);
          return 0;
        }
        else
        {
          v13 = 0;
          if ( v8 == 100 )
          {
            v13 = 2;
          }
          else
          {
            this->ErrorMsgLog(v8, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
            this->ErrorAction(v8, this->m_hStmtSelect);
            v13 = 1;
          }
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return v13;
        }
      }
      else
      {
        v12 = 0;
        if ( v8 == 100 )
        {
          v12 = 2;
        }
        else
        {
          this->ErrorMsgLog(v8, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
          this->ErrorAction(v8, this->m_hStmtSelect);
          v12 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v12;
      }
    }
    else if ( v8 == 100 )
    {
      return 2;
    }
    else
    {
      this->ErrorMsgLog(v8, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction(v8, this->m_hStmtSelect);
      return 1;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1;
  }
}

unsigned __int8 CRFWorldDatabase::Select_Exist_Economy(
  unsigned int dwDate,
  _worlddb_economy_history_info *pEconomyData)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-198h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-160h] BYREF
  SQLRETURN v8; // [rsp+44h] [rbp-154h]
  char Buffer[260]; // [rsp+60h] [rbp-138h] BYREF
  int v10; // [rsp+164h] [rbp-34h]
  char v11; // [rsp+168h] [rbp-30h]
  SQLUSMALLINT v12; // [rsp+16Ch] [rbp-2Ch]
  int j; // [rsp+170h] [rbp-28h]
  int k; // [rsp+174h] [rbp-24h]
  int m; // [rsp+178h] [rbp-20h]
  v10 = 0;
  sprintf(Buffer, "{ CALL pSelect_Exist_Economy( %d ) }", dwDate);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v8 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v8 || v8 == 1 )
    {
      v8 = SQLFetch(this->m_hStmtSelect);
      if ( !v8 || v8 == 1 )
      {
        v8 = SQLGetData(this->m_hStmtSelect, 1u, 8, pEconomyData->dTradeDalant, 0LL, &StrLen_or_IndPtr);
        v8 = SQLGetData(this->m_hStmtSelect, 2u, 8, &pEconomyData->dTradeDalant[1], 0LL, &StrLen_or_IndPtr);
        v8 = SQLGetData(this->m_hStmtSelect, 3u, 8, &pEconomyData->dTradeDalant[2], 0LL, &StrLen_or_IndPtr);
        v8 = SQLGetData(this->m_hStmtSelect, 4u, 8, pEconomyData, 0LL, &StrLen_or_IndPtr);
        v8 = SQLGetData(this->m_hStmtSelect, 5u, 8, &pEconomyData->dTradeGold[1], 0LL, &StrLen_or_IndPtr);
        v8 = SQLGetData(this->m_hStmtSelect, 6u, 8, &pEconomyData->dTradeGold[2], 0LL, &StrLen_or_IndPtr);
        v8 = SQLGetData(this->m_hStmtSelect, 7u, 4, &pEconomyData->dwManageValue, 0LL, &StrLen_or_IndPtr);
        v12 = 8;
        for ( j = 0; j < 3; ++j )
        {
          for ( k = 0; k < 3; ++k )
          {
            v8 = SQLGetData(this->m_hStmtSelect, v12++, 8, &pEconomyData->dMineOre[k][j], 0LL, &StrLen_or_IndPtr);
            if ( v8 )
            {
              if ( v8 != 1 )
                break;
            }
          }
        }
        for ( j = 0; j < 3; ++j )
        {
          for ( m = 0; m < 3; ++m )
          {
            v8 = SQLGetData(this->m_hStmtSelect, v12++, 8, &pEconomyData->dCutOre[m][j], 0LL, &StrLen_or_IndPtr);
            if ( v8 )
            {
              if ( v8 != 1 )
                break;
            }
          }
        }
        if ( !v8 || v8 == 1 )
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          if ( this->m_bSaveDBLog )
            this->FmtLog("%s Success", Buffer);
          return 0;
        }
        else
        {
          this->ErrorMsgLog(v8, Buffer, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction(v8, this->m_hStmtSelect);
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return 1;
        }
      }
      else
      {
        v11 = 0;
        if ( v8 == 100 )
        {
          v11 = 2;
        }
        else
        {
          this->ErrorMsgLog(v8, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(v8, this->m_hStmtSelect);
          v11 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v11;
      }
    }
    else if ( v8 == 100 )
    {
      return 2;
    }
    else
    {
      this->ErrorMsgLog(v8, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction(v8, this->m_hStmtSelect);
      return 1;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1;
  }
}

unsigned __int8 CRFWorldDatabase::Select_Limit_Run_Record()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-4A8h] BYREF
  char _Dest[1024]; // [rsp+40h] [rbp-468h] BYREF
  unsigned __int8 v6; // [rsp+444h] [rbp-64h]
  unsigned int TargetValue[8]; // [rsp+454h] [rbp-54h] BYREF
  unsigned __int16 ColumnNumber[14]; // [rsp+474h] [rbp-34h] BYREF

  memset(_Dest, 0, sizeof(_Dest));
  sprintf_s(_Dest, sizeof(_Dest), "select [aserial] from [dbo].[tbl_sf_delay] where [aserial] = 0");
  v6 = this->SQLExecDirect_RetErrCode(_Dest);
  if ( v6 )
    return v6;
  v6 = this->SQLFetch_RetErrCode(_Dest);
  if ( v6 )
    return v6;
  TargetValue[0] = 0;
  ColumnNumber[0] = 1;
  v6 = this->SQLGetData_RetErrCode(_Dest, ColumnNumber, 4, TargetValue);
  if ( v6 )
    return v6;
  this->SelectCleanUp(_Dest);
  return 0;
}

unsigned __int8 CRFWorldDatabase::Select_MacroData(unsigned int dwSerial,
        _AIOC_A_MACRODATA *pMacro)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  char query[260]{};

  sprintf(
    query,
    "Select hp, fp, sp, action0, action1, action2, action3, action4, action5, action6, action7, action8, action9, chat0, "
    "chat1, chat2, chat3, chat4, hpvalue, fpvalue, spvalue from tbl_macro where serial=%d order by belt asc",
    dwSerial);

  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }

  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
  {
    if (sqlRet == SQL_NO_DATA)
    {
      return 2;
    }

    this->ErrorMsgLog(sqlRet, query, "_SQLExecDirect", this->m_hStmtSelect);
    this->ErrorAction(sqlRet, this->m_hStmtSelect);
    return 1;
  }

  int fetchedRowCount = 0;
  for (;; ++fetchedRowCount)
  {
    unsigned int potionKinds[3]{};
    unsigned int actionSlots[10]{};
    char chatColumns[5][256]{};
    unsigned int potionValues[3]{};

    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
    {
      break;
    }

    sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 4, &potionKinds[0], 0, &indicator);
    sqlRet = SQLGetData(this->m_hStmtSelect, 2u, 4, &potionKinds[1], 0, &indicator);
    sqlRet = SQLGetData(this->m_hStmtSelect, 3u, 4, &potionKinds[2], 0, &indicator);

    for (unsigned int actionIndex = 0; actionIndex < 10; ++actionIndex)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 4u + actionIndex, 4, &actionSlots[actionIndex], 0, &indicator);
    }

    for (unsigned int chatIndex = 0; chatIndex < 5; ++chatIndex)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 14u + chatIndex, 1, chatColumns[chatIndex], 255, &indicator);
    }

    sqlRet = SQLGetData(this->m_hStmtSelect, 19u, 4, &potionValues[0], 0, &indicator);
    sqlRet = SQLGetData(this->m_hStmtSelect, 20u, 4, &potionValues[1], 0, &indicator);
    sqlRet = SQLGetData(this->m_hStmtSelect, 21u, 4, &potionValues[2], 0, &indicator);

    if (fetchedRowCount < 1)
    {
      for (unsigned int potionIndex = 0; potionIndex < 3; ++potionIndex)
      {
        pMacro->mcr_Potion[fetchedRowCount].Potion[potionIndex] = potionKinds[potionIndex];
        pMacro->mcr_Potion[fetchedRowCount].PotionValue[potionIndex] = potionValues[potionIndex];
      }
    }

    if (fetchedRowCount < 3)
    {
      for (unsigned int actionIndex = 0; actionIndex < 10; ++actionIndex)
      {
        pMacro->mcr_Action[fetchedRowCount].Action[actionIndex] = actionSlots[actionIndex];
      }
    }

    if (fetchedRowCount < 2)
    {
      for (unsigned int chatIndex = 0; chatIndex < 5; ++chatIndex)
      {
        strcpy_0(pMacro->mcr_Chat[fetchedRowCount].Chat[chatIndex], chatColumns[chatIndex]);
      }
    }
  }

  if (!fetchedRowCount)
  {
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    return 2;
  }

  if (sqlRet == SQL_SUCCESS_WITH_INFO || (fetchedRowCount == 3 && sqlRet == SQL_NO_DATA))
  {
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    if (this->m_bSaveDBLog)
    {
      this->FmtLog("%s Success", query);
    }
    return 0;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLGetData", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  if (this->m_hStmtSelect)
  {
    SQLCloseCursor(this->m_hStmtSelect);
  }
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_PatriarchComm(unsigned int dwSerial,
        _patriarch_comm_list *pOutList)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-138h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-100h] BYREF
  SQLRETURN v8; // [rsp+44h] [rbp-F4h]
  char szToday[56]; // [rsp+58h] [rbp-E0h] BYREF
  char Buffer[144]; // [rsp+90h] [rbp-A8h] BYREF

  GetTodayStr(szToday);
  sprintf(Buffer, "{ CALL pSelect_PatriarchComm( %d, '%s' ) }", dwSerial, szToday);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( !this->m_hStmtSelect && !this->ReConnectDataBase() )
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1LL;
  }
  v8 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
  if ( v8 && v8 != 1 )
  {
    if ( v8 == 100 )
      return 2LL;
    this->ErrorMsgLog(v8, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
    this->ErrorAction(v8, this->m_hStmtSelect);
    return 1LL;
  }
  while ( 1 )
  {
    v8 = SQLFetch(this->m_hStmtSelect);
    if ( v8 && v8 != 1 )
    {
      if ( v8 != 100 )
      {
        this->ErrorMsgLog(v8, Buffer, "SQLFetch", this->m_hStmtSelect);
        this->ErrorAction(v8, this->m_hStmtSelect);
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return 1LL;
      }
      goto LABEL_27;
    }
    v8 = SQLGetData(this->m_hStmtSelect, 1u, 4, &pOutList->List[pOutList->dwCount], 0LL, &StrLen_or_IndPtr);
    v8 = SQLGetData(this->m_hStmtSelect, 2u, 1, pOutList->List[pOutList->dwCount].pszDepDate, 9LL, &StrLen_or_IndPtr);
    if ( v8 )
    {
      if ( v8 != 1 )
        break;
    }
    ++pOutList->dwCount;
  }
  if ( v8 == 100 )
  {
LABEL_27:
    if ( this->m_hStmtSelect )
      SQLCloseCursor(this->m_hStmtSelect);
    if ( this->m_bSaveDBLog )
      this->FmtLog("%s Success", Buffer);
    return 0LL;
  }
  this->ErrorMsgLog(v8, Buffer, "SQLFetch", this->m_hStmtSelect);
  this->ErrorAction(v8, this->m_hStmtSelect);
  if ( this->m_hStmtSelect )
    SQLCloseCursor(this->m_hStmtSelect);
  return 1LL;
}

unsigned __int8 CRFWorldDatabase::Select_PostContent(unsigned int dwPostSerial,
        char *wszContent,
        int nSize)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-108h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-D0h] BYREF
  SQLRETURN v9; // [rsp+44h] [rbp-C4h]
  int v10; // [rsp+48h] [rbp-C0h]
  char Buffer[132]; // [rsp+60h] [rbp-A8h] BYREF
  char v12; // [rsp+E4h] [rbp-24h]
  v10 = 0;
  memset(Buffer, 0, 128);
  sprintf_s(Buffer, 0x80uLL, "select content from tbl_PostStorage where serial=%d", dwPostSerial);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v9 || v9 == 1 )
    {
      v9 = SQLFetch(this->m_hStmtSelect);
      if ( !v9 || v9 == 1 )
      {
        v9 = SQLGetData(this->m_hStmtSelect, 1u, 1, wszContent, nSize, &StrLen_or_IndPtr);
        if ( !v9 || v9 == 1 )
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          if ( this->m_bSaveDBLog )
            this->FmtLog("%s Success", Buffer);
          return 0;
        }
        else
        {
          this->ErrorMsgLog(v9, Buffer, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction(v9, this->m_hStmtSelect);
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return 1;
        }
      }
      else
      {
        v12 = 0;
        if ( v9 == 100 )
        {
          v12 = 2;
        }
        else
        {
          this->ErrorMsgLog(v9, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(v9, this->m_hStmtSelect);
          v12 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v12;
      }
    }
    else if ( v9 == 100 )
    {
      return 2;
    }
    else
    {
      this->ErrorMsgLog(v9, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction(v9, this->m_hStmtSelect);
      return 1;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1;
  }
}

unsigned __int8 CRFWorldDatabase::Select_PostStorageList(unsigned int dwOwner,
        _post_storage_list *pListData)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-288h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-250h] BYREF
  SQLRETURN v8; // [rsp+44h] [rbp-244h]
  char Buffer[528]; // [rsp+60h] [rbp-228h] BYREF

  memset(Buffer, 0, 512);
  pListData->dwCount = 0;
  sprintf_s(
    Buffer,
    0x200uLL,
    "select top %d postinx,serial,poststate,sendname,title,k,d,u,gold,uid,sindex from tbl_PostStorage where owner=%d and "
    "poststate<%d and dck=0",
    50,
    dwOwner,
    100);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v8 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v8 || v8 == 1 )
    {
      while ( 1 )
      {
        v8 = SQLFetch(this->m_hStmtSelect);
        if ( v8 )
        {
          if ( v8 != 1 )
            break;
        }
        v8 = SQLGetData(
               this->m_hStmtSelect,
               1u,
               65530,
               &pListData->List[pListData->dwCount].byIndex,
               0LL,
               &StrLen_or_IndPtr);
        v8 = SQLGetData(this->m_hStmtSelect, 2u, 65518, &pListData->List[pListData->dwCount], 0LL, &StrLen_or_IndPtr);
        v8 = SQLGetData(
               this->m_hStmtSelect,
               3u,
               65530,
               &pListData->List[pListData->dwCount].byState,
               0LL,
               &StrLen_or_IndPtr);
        v8 = SQLGetData(
               this->m_hStmtSelect,
               4u,
               1,
               pListData->List[pListData->dwCount].wszSendName,
               17LL,
               &StrLen_or_IndPtr);
        v8 = SQLGetData(
               this->m_hStmtSelect,
               5u,
               1,
               pListData->List[pListData->dwCount].wszTitle,
               21LL,
               &StrLen_or_IndPtr);
        v8 = SQLGetData(this->m_hStmtSelect, 6u, 4, &pListData->List[pListData->dwCount].nK, 0LL, &StrLen_or_IndPtr);
        v8 = SQLGetData(
               this->m_hStmtSelect,
               7u,
               65511,
               &pListData->List[pListData->dwCount].dwDur,
               0LL,
               &StrLen_or_IndPtr);
        v8 = SQLGetData(
               this->m_hStmtSelect,
               8u,
               4,
               &pListData->List[pListData->dwCount].dwUpt,
               0LL,
               &StrLen_or_IndPtr);
        v8 = SQLGetData(
               this->m_hStmtSelect,
               9u,
               65518,
               &pListData->List[pListData->dwCount].dwGold,
               0LL,
               &StrLen_or_IndPtr);
        v8 = SQLGetData(
               this->m_hStmtSelect,
               0xAu,
               65511,
               &pListData->List[pListData->dwCount].lnUID,
               0LL,
               &StrLen_or_IndPtr);
        v8 = SQLGetData(
               this->m_hStmtSelect,
               0xBu,
               5,
               &pListData->List[pListData->dwCount].wStorageIndex,
               0LL,
               &StrLen_or_IndPtr);
        ++pListData->dwCount;
      }
      if ( this->m_hStmtSelect )
        SQLCloseCursor(this->m_hStmtSelect);
      if ( this->m_bSaveDBLog )
        this->FmtLog("%s Success", Buffer);
      return 0;
    }
    else if ( v8 == 100 )
    {
      return 2;
    }
    else
    {
      this->ErrorMsgLog(v8, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction(v8, this->m_hStmtSelect);
      return 1;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1;
  }
}

unsigned __int8 CRFWorldDatabase::Select_PvpRankInfo(unsigned __int8 byRace,
        char *szDate,
        _PVP_RANK_DATA *rankData)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-188h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-150h] BYREF
  SQLRETURN v9; // [rsp+44h] [rbp-144h]
  char Buffer[260]; // [rsp+60h] [rbp-128h] BYREF
  int v11; // [rsp+164h] [rbp-24h]
  memset(Buffer, 0, 256);
  v11 = 0;
  sprintf(
    Buffer,
    "select top 100 Rank,Lv,Rate,Grade,PvpPoint,Name,GuildName,serial,guildserial from tbl_PvpRank%s where race = %d orde"
    "r by grade desc, rate ",
    szDate,
    byRace);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v9 || v9 == 1 )
    {
      do
      {
        v9 = SQLFetch(this->m_hStmtSelect);
        if ( v9 && v9 != 1 )
          break;
        v9 = SQLGetData(this->m_hStmtSelect, 1u, 65508, &rankData[v11], 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 2u, 65508, &rankData[v11].byLv, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 3u, 65519, &rankData[v11].wRate, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 4u, 65508, &rankData[v11].byGrade, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 5u, 8, &rankData[v11].dPvpPoint, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 6u, 1, rankData[v11].wszName, 17LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 7u, 1, rankData[v11].wszGuildName, 17LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 8u, 4, &rankData[v11].dwAvatorSerial, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 9u, 4, &rankData[v11++].dwGuildSerial, 0LL, &StrLen_or_IndPtr);
        if ( v11 >= 100 )
          break;
      }
      while ( !v9 || v9 == 1 );
      if ( this->m_hStmtSelect )
        SQLCloseCursor(this->m_hStmtSelect);
      if ( this->m_bSaveDBLog )
        this->FmtLog("%s Success", Buffer);
      return 0;
    }
    else if ( v9 == 100 )
    {
      return 2;
    }
    else
    {
      this->ErrorMsgLog(v9, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction(v9, this->m_hStmtSelect);
      return 1;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1;
  }
}

unsigned __int8 CRFWorldDatabase::Select_RegeAvator_For_Lobby_Logout(unsigned int dwAccountSerial,
        _rege_char_data *pRegeCharData)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-488h] BYREF
  char _Dest[1024]; // [rsp+40h] [rbp-448h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+458h] [rbp-30h] BYREF
  SQLRETURN v9; // [rsp+464h] [rbp-24h]
  int v10; // [rsp+468h] [rbp-20h]

  sprintf_s(
    _Dest,
    sizeof(_Dest),
    "select Serial,Name,Slot,Lv,Dalant,Gold from tbl_base where AccountSerial=%d and DCK=0",
    dwAccountSerial);
  if ( this->m_bSaveDBLog )
    this->Log(_Dest);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)_Dest, -3);
    if ( !v9 || v9 == 1 )
    {
      v10 = 0;
      while ( 1 )
      {
        v9 = SQLFetch(this->m_hStmtSelect);
        if ( v9 )
        {
          if ( v9 != 1 )
            break;
        }
        v9 = SQLGetData(
               this->m_hStmtSelect,
               1u,
               65518,
               &pRegeCharData->RegeList[v10].dwCharSerial,
               0LL,
               &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 2u, 1, pRegeCharData->RegeList[v10].szCharName, 17LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 3u, 65519, &pRegeCharData->RegeList[v10], 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 4u, 4, &pRegeCharData->RegeList[v10].nLevel, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 5u, 4, &pRegeCharData->RegeList[v10].dwDalant, 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 6u, 4, &pRegeCharData->RegeList[v10++].dwGold, 0LL, &StrLen_or_IndPtr);
        if ( v10 >= 3 )
          break;
        if ( v9 && v9 != 1 )
        {
          if ( v9 != 100 )
          {
            this->ErrorMsgLog(v9, _Dest, "SQLExecDirectA", this->m_hStmtSelect);
            this->ErrorAction(v9, this->m_hStmtSelect);
            if ( this->m_hStmtSelect )
              SQLCloseCursor(this->m_hStmtSelect);
            return 1;
          }
          break;
        }
      }
      pRegeCharData->nCharNum = v10;
      if ( this->m_hStmtSelect )
        SQLCloseCursor(this->m_hStmtSelect);
      if ( this->m_bSaveDBLog )
        this->FmtLog("%s Success", _Dest);
      return 0;
    }
    else if ( v9 == 100 )
    {
      return 2;
    }
    else
    {
      this->ErrorMsgLog(v9, _Dest, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction(v9, this->m_hStmtSelect);
      return 1;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", _Dest);
    return 1;
  }
}

unsigned __int8 CRFWorldDatabase::Select_ReturnPost(unsigned int dwMax,
        unsigned int dwMasterSerial,
        _return_post_list *pRetData)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-2A8h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-270h] BYREF
  SQLRETURN v9; // [rsp+44h] [rbp-264h]
  unsigned int TargetValue[11]; // [rsp+54h] [rbp-254h] BYREF
  char Buffer[516]; // [rsp+80h] [rbp-228h] BYREF
  char v12; // [rsp+284h] [rbp-24h]
  TargetValue[0] = 0;
  memset(Buffer, 0, 512);
  pRetData->dwCount = 0;
  pRetData->bContinue = 0;
  sprintf_s(
    Buffer,
    0x200uLL,
    "select count(serial) from tbl_PostStorage where owner=%d and poststate=%d and dck=0",
    dwMasterSerial,
    100);
  if ( this->m_bSaveDBLog )
    this->Log(Buffer);
  if ( !this->m_hStmtSelect && !this->ReConnectDataBase() )
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1;
  }
  v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
  if ( v9 && v9 != 1 )
  {
    if ( v9 == 100 )
      return 2;
    this->ErrorMsgLog(v9, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
    this->ErrorAction(v9, this->m_hStmtSelect);
    return 1;
  }
  v9 = SQLFetch(this->m_hStmtSelect);
  if ( !v9 || v9 == 1 )
  {
    v9 = SQLGetData(this->m_hStmtSelect, 1u, 65518, TargetValue, 0LL, &StrLen_or_IndPtr);
    if ( !v9 || v9 == 1 )
    {
      if ( this->m_hStmtSelect )
        SQLCloseCursor(this->m_hStmtSelect);
      if ( this->m_bSaveDBLog )
        this->FmtLog("%s Success", Buffer);
      if ( TargetValue[0] )
      {
        sprintf_s(
          Buffer,
          0x200uLL,
          "select top %d serial,poststate,recvname,title,content,k,d,u,gold,err,uid from tbl_PostStorage where owner=%d a"
          "nd poststate=%d and dck=0",
          dwMax,
          dwMasterSerial,
          100);
        if ( this->m_bSaveDBLog )
          this->Log(Buffer);
        if ( !this->m_hStmtSelect && !this->ReConnectDataBase() )
        {
          this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
          return 1;
        }
        v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
        if ( v9 && v9 != 1 )
        {
          if ( v9 == 100 )
            return 2;
          this->ErrorMsgLog(v9, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
          this->ErrorAction(v9, this->m_hStmtSelect);
          return 1;
        }
        while ( 1 )
        {
          v9 = SQLFetch(this->m_hStmtSelect);
          if ( v9 )
          {
            if ( v9 != 1 )
              break;
          }
          v9 = SQLGetData(this->m_hStmtSelect, 1u, 65518, &pRetData->List[pRetData->dwCount], 0LL, &StrLen_or_IndPtr);
          v9 = SQLGetData(
                 this->m_hStmtSelect,
                 2u,
                 65530,
                 &pRetData->List[pRetData->dwCount].byState,
                 0LL,
                 &StrLen_or_IndPtr);
          v9 = SQLGetData(
                 this->m_hStmtSelect,
                 3u,
                 1,
                 pRetData->List[pRetData->dwCount].wszRecvName,
                 17LL,
                 &StrLen_or_IndPtr);
          v9 = SQLGetData(
                 this->m_hStmtSelect,
                 4u,
                 1,
                 pRetData->List[pRetData->dwCount].wszTitle,
                 21LL,
                 &StrLen_or_IndPtr);
          v9 = SQLGetData(
                 this->m_hStmtSelect,
                 5u,
                 1,
                 pRetData->List[pRetData->dwCount].wszContent,
                 201LL,
                 &StrLen_or_IndPtr);
          v9 = SQLGetData(this->m_hStmtSelect, 6u, 4, &pRetData->List[pRetData->dwCount].nK, 0LL, &StrLen_or_IndPtr);
          v9 = SQLGetData(
                 this->m_hStmtSelect,
                 7u,
                 65511,
                 &pRetData->List[pRetData->dwCount].dwDur,
                 0LL,
                 &StrLen_or_IndPtr);
          v9 = SQLGetData(
                 this->m_hStmtSelect,
                 8u,
                 65518,
                 &pRetData->List[pRetData->dwCount].dwUpt,
                 0LL,
                 &StrLen_or_IndPtr);
          v9 = SQLGetData(
                 this->m_hStmtSelect,
                 9u,
                 65518,
                 &pRetData->List[pRetData->dwCount].dwGold,
                 0LL,
                 &StrLen_or_IndPtr);
          v9 = SQLGetData(
                 this->m_hStmtSelect,
                 0xAu,
                 65530,
                 &pRetData->List[pRetData->dwCount].byErr,
                 0LL,
                 &StrLen_or_IndPtr);
          v9 = SQLGetData(
                 this->m_hStmtSelect,
                 0xBu,
                 65511,
                 &pRetData->List[pRetData->dwCount].lnUID,
                 0LL,
                 &StrLen_or_IndPtr);
          ++pRetData->dwCount;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        if ( this->m_bSaveDBLog )
          this->FmtLog("%s Success", Buffer);
      }
      return 0;
    }
    this->ErrorMsgLog(v9, Buffer, "SQLGetData", this->m_hStmtSelect);
    this->ErrorAction(v9, this->m_hStmtSelect);
    if ( this->m_hStmtSelect )
      SQLCloseCursor(this->m_hStmtSelect);
    return 1;
  }
  else
  {
    v12 = 0;
    if ( v9 == 100 )
    {
      v12 = 2;
    }
    else
    {
      this->ErrorMsgLog(v9, Buffer, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(v9, this->m_hStmtSelect);
      v12 = 1;
    }
    if ( this->m_hStmtSelect )
      SQLCloseCursor(this->m_hStmtSelect);
    return v12;
  }
}

unsigned __int8 CRFWorldDatabase::Select_RFEvent_ClassRefine(unsigned int dwAvatorSerial,
        unsigned __int8 *byRefinedCnt,
        unsigned int *dwRefineDate)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-188h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-150h] BYREF
  SQLRETURN v9; // [rsp+44h] [rbp-144h]
  char _Dest[256]; // [rsp+60h] [rbp-128h] BYREF
  unsigned __int8 v11; // [rsp+164h] [rbp-24h]
  unsigned __int8 v12; // [rsp+165h] [rbp-23h]
  unsigned __int8 v13; // [rsp+166h] [rbp-22h]

  memset(_Dest, 0, sizeof(_Dest));
  sprintf_s(
    _Dest,
    sizeof(_Dest),
    "select ClassRefineCnt,ClassRefineDate from [dbo].[tbl_event] where avatorserial = %d",
    dwAvatorSerial);
  if ( this->m_bSaveDBLog )
    this->Log(_Dest);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)_Dest, -3);
    if ( !v9 || v9 == 1 )
    {
      v9 = SQLFetch(this->m_hStmtSelect);
      if ( !v9 || v9 == 1 )
      {
        v9 = SQLGetData(this->m_hStmtSelect, 1u, 65530, byRefinedCnt, 0LL, &StrLen_or_IndPtr);
        if ( !v9 || v9 == 1 )
        {
          v9 = SQLGetData(this->m_hStmtSelect, 2u, 4, dwRefineDate, 0LL, &StrLen_or_IndPtr);
          if ( !v9 || v9 == 1 )
          {
            if ( this->m_hStmtSelect )
              SQLCloseCursor(this->m_hStmtSelect);
            if ( this->m_bSaveDBLog )
              this->FmtLog("%s Success", _Dest);
            return 0LL;
          }
          else
          {
            v13 = 0;
            if ( v9 == 100 )
            {
              v13 = 2;
            }
            else
            {
              this->ErrorMsgLog(v9, _Dest, "SQLGetData", this->m_hStmtSelect);
              this->ErrorAction(v9, this->m_hStmtSelect);
              v13 = 1;
            }
            if ( this->m_hStmtSelect )
              SQLCloseCursor(this->m_hStmtSelect);
            return v13;
          }
        }
        else
        {
          v12 = 0;
          if ( v9 == 100 )
          {
            v12 = 2;
          }
          else
          {
            this->ErrorMsgLog(v9, _Dest, "SQLGetData", this->m_hStmtSelect);
            this->ErrorAction(v9, this->m_hStmtSelect);
            v12 = 1;
          }
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return v12;
        }
      }
      else
      {
        v11 = 0;
        if ( v9 == 100 )
        {
          v11 = 2;
        }
        else
        {
          this->ErrorMsgLog(v9, _Dest, "SQLExecDirect", this->m_hStmtSelect);
          this->ErrorAction(v9, this->m_hStmtSelect);
          v11 = 1;
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return v11;
      }
    }
    else if ( v9 == 100 )
    {
      return 2LL;
    }
    else
    {
      this->ErrorMsgLog(v9, _Dest, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(v9, this->m_hStmtSelect);
      return 1LL;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", _Dest);
    return 1LL;
  }
}

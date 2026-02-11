#pragma once

#include "IdaCompat.h"

#include "CLogFile.h"
#include "CMyTimer.h"
#include "CNetProcess.h"

struct _goldbox_index
{
  unsigned __int16 wItemIndex;
  char szItemCode[64];

  _goldbox_index();
};

struct _golden_box_item_ini
{
  struct _golden_box_item_list
  {
    char m_szLimcode[64];
    unsigned __int16 m_wLimcount;
    unsigned __int16 m_wRate;
  };

  int m_bUse_event;
  int m_wYear[2];
  int m_byMonth[2];
  int m_byDay[2];
  int m_byHour[2];
  int m_byMinute[2];
  int m_EventTime[2];
  char m_szGoldenBoxcode[2][64];
  unsigned __int16 m_wGoldenBoxmax[2];
  unsigned __int8 m_bygolden_item_num[2];
  unsigned int m_dwStarterBoxCnt;
  char m_szStarterBoxCode[2][64];
  unsigned __int8 m_byLoopCnt;
  _golden_box_item_list m_golden_box_item_list[2][100];
};

struct __cppobj __declspec(align(8)) _golden_box_item_event
{
  unsigned __int8 m_event_status;
  CNetTimer m_event_timer;
  CLogFile m_event_log;
  _golden_box_item_ini m_ini;
};

struct _golden_box_item
{
  struct _golden_box_item_info
  {
    unsigned __int8 m_byTableCode;
    unsigned int m_dwIndex;
    unsigned __int16 m_wNum;
    unsigned __int16 m_wRate;
  };

  unsigned __int8 m_bydck;
  unsigned int m_dwStarterBoxCnt;
  unsigned __int8 m_byBoxTableCode[2];
  unsigned int m_dwBoxIndex[2];
  unsigned __int16 m_wBoxMax[2];
  unsigned __int8 m_bygolden_item_num[2];
  _golden_box_item_info m_golden_box_item_info[2][100];
};

struct _db_golden_box_item
{
  struct _db_golden_box_item_List
  {
    int ncode;
    unsigned __int16 wcount;
  };

  unsigned __int8 bydck;
  unsigned int dwStarterBoxCnt;
  int nBoxcode[2];
  unsigned __int16 wBoxMax[2];
  unsigned __int8 bygolden_item_num[2];
  _db_golden_box_item_List List[2][100];
};

class __cppobj CGoldenBoxItemMgr
{
public:
  struct _BoxItemOpen_output
  {
    char m_itmPdOutput[8];
    int m_nPdProCnt;
    unsigned int m_dwProb;
  };

  static CGoldenBoxItemMgr *Instance();

  bool Initialize();
  bool Load_Golden_Box_Item_Event();
  bool _init_loggers();
  void Set_Event_Status(unsigned __int8 byStatus);
  bool Load_Event_INI(_golden_box_item_ini *pIni);
  void Check_Loaded_Event_Status();
  bool SetGoldBoxItemIndex();
  unsigned __int8 GetLoopCount();
  void Set_FromINIToStruct(_golden_box_item_ini *pIni);
  void Set_ToStruct();
  void BoxItemDataCopy();

  unsigned __int8 Get_Event_Status();
  unsigned __int16 GetGoldBoxItemIndex(unsigned __int16 wIndex);
  unsigned __int16 Get_Box_Count(unsigned __int8 byIndex);
  void Set_Box_Count(unsigned __int8 byIndex);

  CLogFile _kLogger;
  CMyTimer m_tmLoopTimer;
  _golden_box_item_event m_golden_box_event;
  _golden_box_item m_golden_box_item;
  _db_golden_box_item m_golden_box_item_New;
  _db_golden_box_item m_golden_box_item_Old;
  _db_golden_box_item m_temp_db;
  unsigned __int16 m_wStarterBoxNum;
  bool m_bInit;
  _SYSTEMTIME tm;
  _SYSTEMTIME tm1;
  bool m_bConsume;
  _goldbox_index *m_pItemIndex;
  int m_nDBSerial;
  _BoxItemOpen_output *m_pBoxItemOpen;
};


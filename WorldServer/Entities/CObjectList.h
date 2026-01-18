#pragma once

#include "IdaCompat.h"

class CGameObject;

struct _object_list_point
{
  _object_list_point();
  void InitLink();
  void SetPoint(CGameObject *pItem);

  CGameObject *m_pItem;
  _object_list_point *m_pNext;
  _object_list_point *m_pPrev;
};

class __declspec(align(8)) CObjectList
{
public:
  CObjectList();
  virtual ~CObjectList();

  CGameObject *CopyItem(unsigned int dwIndex);
  bool DeleteItem(_object_list_point *pItem);
  int GetSize();
  void InitList();
  bool PushItem(_object_list_point *pItem);

  _object_list_point m_Head;
  _object_list_point m_Tail;
  int m_nSize;
};

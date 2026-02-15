#include "pch.h"
#include "CObjectList.h"
#include "CGameObject.h"


_object_list_point::_object_list_point()
{
  m_pItem = nullptr;
  InitLink();
}

void _object_list_point::InitLink()
{
  m_pNext = nullptr;
  m_pPrev = nullptr;
}

void _object_list_point::SetPoint(CGameObject *pItem)
{
  m_pItem = pItem;
  InitLink();
}

CObjectList::CObjectList()
{
  m_Head = _object_list_point();
  m_Tail = _object_list_point();
  InitList();
}

CObjectList::~CObjectList() = default;

CGameObject *CObjectList::CopyItem(unsigned int dwIndex)
{
  int index = 0;
  _object_list_point *node = m_Head.m_pNext;
  while (node != &m_Tail)
  {
    if (dwIndex == static_cast<unsigned int>(index))
    {
      return node->m_pItem;
    }
    ++index;
    node = node->m_pNext;
  }
  return nullptr;
}

bool CObjectList::DeleteItem(_object_list_point *pItem)
{
  if (pItem == nullptr)
  {
    return false;
  }
  for (_object_list_point *node = m_Head.m_pNext; node != &m_Tail; node = node->m_pNext)
  {
    if (node == pItem)
    {
      node->m_pNext->m_pPrev = node->m_pPrev;
      node->m_pPrev->m_pNext = node->m_pNext;
      node->m_pNext = nullptr;
      node->m_pPrev = nullptr;
      --m_nSize;
      return true;
    }
  }
  MyMessageBox("CObjectList", "DeleteItem error");
  return false;
}

int CObjectList::GetSize()
{
  return m_nSize;
}

void CObjectList::InitList()
{
  m_nSize = 0;
  m_Head.m_pNext = &m_Tail;
  m_Head.m_pPrev = &m_Head;
  m_Tail.m_pNext = &m_Tail;
  m_Tail.m_pPrev = &m_Head;
}

bool CObjectList::PushItem(_object_list_point *pItem)
{
  if (pItem == nullptr || pItem->m_pNext || pItem->m_pPrev)
  {
    return false;
  }

  pItem->m_pNext = &m_Tail;
  pItem->m_pPrev = m_Tail.m_pPrev;
  m_Tail.m_pPrev->m_pNext = pItem;
  m_Tail.m_pPrev = pItem;
  ++m_nSize;
  return true;
}

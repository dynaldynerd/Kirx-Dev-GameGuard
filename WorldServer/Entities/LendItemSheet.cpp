#include "pch.h"

#include "LendItemSheet.h"

#include <ctime>

#include "CPlayer.h"
#include "CMgrAvatorItemHistory.h"

LendItemSheet::Cell::Cell()
{
  memset_0(this, 0, sizeof(LendItemSheet::Cell));
}

LendItemSheet::Cell::Cell(unsigned __int8 byStCode, _STORAGE_LIST::_db_con *pItem)
{
  _nStorageCode = byStCode;
  _pkItem = pItem;
}

bool LendItemSheet::Cell::operator==(const Cell *rhs) const
{
  return _nStorageCode == rhs->_nStorageCode && _pkItem == rhs->_pkItem;
}

bool LendItemSheet::Cell::operator>(const Cell *rhs) const
{
  return _pkItem->m_dwT > rhs->_pkItem->m_dwT;
}

LendItemSheet::LendItemSheet(CPlayer *p)
  : _pkOwner(p)
{
}

LendItemSheet::~LendItemSheet() = default;

bool LendItemSheet::Initialzie()
{
  return _heapFixRow.Initialize(200);
}

bool LendItemSheet::InsertLink(unsigned __int8 byStorageCode, _STORAGE_LIST::_db_con *pkItem)
{
  Cell cell(byStorageCode, pkItem);
  return _heapFixRow.push(cell);
}

bool LendItemSheet::DeleteLink(unsigned __int8 byStorageCode, _STORAGE_LIST::_db_con *pkItem)
{
  Cell *topCell = _heapFixRow.top();
  if (!topCell)
  {
    return false;
  }

  if (topCell->_nStorageCode == byStorageCode && topCell->_pkItem == pkItem)
  {
    return _heapFixRow.pop();
  }

  Cell cell(byStorageCode, pkItem);
  return _heapFixRow.pop(cell);
}

void LendItemSheet::Release()
{
  _heapFixRow.Release();
}

__int64 LendItemSheet::CheckTime()
{
  if (_heapFixRow.empty())
  {
    return 0;
  }

  __time32_t now[4]{};
  _time32(now);

  int result = 0;
  for (int j = 0; ; ++j)
  {
    if (static_cast<unsigned __int64>(j) >= _heapFixRow.size())
    {
      break;
    }

    Cell *cell = _heapFixRow.top();
    if (!cell || cell->_pkItem->m_dwT > static_cast<unsigned int>(now[0]))
    {
      break;
    }

    if (_pkOwner->m_pUserDB)
    {
      if (cell->_nStorageCode < 8u)
      {
        _STORAGE_LIST *storage = _pkOwner->m_Param.m_pStoragePtr[cell->_nStorageCode];
        auto *src = &storage->m_pStorageList[cell->_pkItem->m_byStorageIndex];
        if (src)
        {
          if (src->m_bLoad)
          {
            if (src->m_byCsMethod)
            {
              _STORAGE_LIST::_db_con copyItem{};
              memcpy_0(&copyItem, src, sizeof(copyItem));
              _pkOwner->SendMsg_LendItemTimeExpired(cell->_nStorageCode, cell->_pkItem->m_wSerial);
              if (!_pkOwner->Emb_DelStorage(
                    cell->_nStorageCode,
                    cell->_pkItem->m_byStorageIndex,
                    false,
                    true,
                    "LendItemSheet::CheckTime"))
              {
                result = -5;
              }
              CPlayer::s_MgrItemHistory.lenditem_del_from_inven(
                copyItem.m_byTableCode,
                copyItem.m_wItemIndex,
                copyItem.m_lnUID,
                _pkOwner->m_szItemHistoryFileName);
            }
            else
            {
              result = -4;
            }
          }
          else
          {
            result = -3;
          }
        }
        else
        {
          result = -2;
        }
      }
      else
      {
        result = -1;
      }
    }
  }

  return result;
}

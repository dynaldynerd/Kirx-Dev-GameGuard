#include "pch.h"

#include "CMainThread.h"

void _POSTSTORAGE_DB_BASE::Init()
{
  memset_0(this, 0, sizeof(_POSTSTORAGE_DB_BASE));
  for (int i = 0; i < 50; ++i)
  {
    m_PostList[i].byState = static_cast<unsigned __int8>(-1);
    m_PostList[i].bRetProc = true;
  }
}

void _POSTSTORAGE_DB_BASE::UpdateInit()
{
  m_bUpdate = false;
  for (int i = 0; i < 50; ++i)
  {
    m_PostList[i].bUpdate = false;
    m_PostList[i].bNew = false;
    m_PostList[i].bUpdateIndex = false;
  }
}

void _RETURNPOST_DB_BASE::Init()
{
  memset_0(this, 0, sizeof(_RETURNPOST_DB_BASE));
  m_nMax = 30;
}

void _DELPOST_DB_BASE::Init()
{
  memset_0(this, 0, sizeof(_DELPOST_DB_BASE));
  m_nMax = 50;
}

void _POSTDATA_DB_BASE::Init()
{
  dbPost.Init();
  dbRetPost.Init();
  dbDelPost.Init();
}

void _POSTDATA_DB_BASE::UpdateInit()
{
  dbPost.UpdateInit();
  dbRetPost.Init();
  dbDelPost.Init();
}

void _AVATOR_DATA::PostUpdateInit()
{
  dbPostData.UpdateInit();
}

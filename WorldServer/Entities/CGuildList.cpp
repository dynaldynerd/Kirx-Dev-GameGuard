#include "pch.h"

#include "CGuildList.h"

#include <new>

__guild_list_page::__guild_list_page()
{
  memset_0(this, 0, sizeof(__guild_list_page));
}

CGuildList::CGuildList()
  : m_bInit(false)
{
  for (int j = 0; j < 3; ++j)
  {
    m_byMaxPage[j] = 0;
    m_pGuildList[j] = nullptr;
  }

  Init();
}

CGuildList::~CGuildList()
{
  for (int j = 0; j < 3; ++j)
  {
    if (m_pGuildList[j])
    {
      operator delete[](m_pGuildList[j]);
      m_pGuildList[j] = nullptr;
    }
  }
}

char CGuildList::Init()
{
  if (m_bInit)
  {
    return 1;
  }

  for (int j = 0; j < 3; ++j)
  {
    __guild_list_page *pages = new (std::nothrow) __guild_list_page[75];
    m_pGuildList[j] = pages;
    if (!m_pGuildList[j])
    {
      return 0;
    }
  }

  m_bInit = true;
  return 1;
}

void CGuildList::AddList(unsigned __int8 byRace, unsigned __int8 byGrade, char *pwszGuildName, char *pwszMasterName)
{
  if (byRace >= 3 || m_byMaxPage[byRace] >= 75)
  {
    return;
  }

  if (m_pGuildList[byRace][m_byMaxPage[byRace]].byListCnt >= 4)
  {
    __guild_list_page *page = &m_pGuildList[byRace][++m_byMaxPage[byRace]];
    page->GuildList[page->byListCnt].byGrade = byGrade;
    strcpy_0(page->GuildList[page->byListCnt].wszGuildName, pwszGuildName);
    strcpy_0(page->GuildList[page->byListCnt].wszMasterName, pwszMasterName);
    ++page->byListCnt;
  }
  else
  {
    __guild_list_page *page = &m_pGuildList[byRace][m_byMaxPage[byRace]];
    page->GuildList[page->byListCnt].byGrade = byGrade;
    strcpy_0(page->GuildList[page->byListCnt].wszGuildName, pwszGuildName);
    strcpy_0(page->GuildList[page->byListCnt].wszMasterName, pwszMasterName);
    ++page->byListCnt;
  }
}

void CGuildList::SetGrade(unsigned __int8 byRace, char *pwszGuildName, unsigned __int8 byGrade)
{
  for (int j = 0; j < m_byMaxPage[byRace]; ++j)
  {
    for (int k = 0; k < m_pGuildList[byRace][j].byListCnt; ++k)
    {
      if (!strcmp_0(m_pGuildList[byRace][j].GuildList[k].wszGuildName, pwszGuildName))
      {
        m_pGuildList[byRace][j].GuildList[k].byGrade = byGrade;
        return;
      }
    }
  }
}

void CGuildList::SetGuildMaster(unsigned __int8 byRace, char *pwszGuildName, char *pwszMasterName)
{
  for (int j = 0; j < m_byMaxPage[byRace]; ++j)
  {
    for (int k = 0; k < m_pGuildList[byRace][j].byListCnt; ++k)
    {
      if (!strcmp_0(m_pGuildList[byRace][j].GuildList[k].wszGuildName, pwszGuildName))
      {
        strcpy_0(m_pGuildList[byRace][j].GuildList[k].wszMasterName, pwszMasterName);
        return;
      }
    }
  }
}

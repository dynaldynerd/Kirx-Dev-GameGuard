#pragma once

#pragma pack(push, 1)
  struct GuildListResultMessage
  {
    unsigned __int8 byMaxPage;
    unsigned __int8 byPage;
    unsigned __int8 byListCnt;
    __guild_list_page::__list GuildList[4];

    unsigned __int16 size() const
    {
      unsigned __int8 listCount = byListCnt;
      if (listCount > 4u)
      {
        listCount = 0;
      }
      return static_cast<unsigned __int16>(143 - 35 * (4 - listCount));
    }
  };
#pragma pack(pop)


#pragma once

#include "../IdaCompat.h"

// Web/Control -> Zone packet structs.

// login_web_agent_server_request_wazo.h

#pragma pack(push, 1)
struct _login_web_agent_server_request_wazo
{
  char byLoginCode;
};
#pragma pack(pop)

static_assert(sizeof(_login_web_agent_server_request_wazo) == 1, "web login packet size mismatch");

// login_controll_server_request_wazo.h

#pragma pack(push, 1)
struct _login_controll_server_request_wazo
{
  char byLoginCode;
};
#pragma pack(pop)

static_assert(sizeof(_login_controll_server_request_wazo) == 1, "control login packet size mismatch");

// send_race_boss_msg_from_web_request_wazo.h

#pragma pack(push, 1)
struct _send_race_boss_msg_from_web_request_wazo
{
  unsigned int dwWebSendDBID;
  char szCharacterName[17];
  char wszMsg[255];
};
#pragma pack(pop)

static_assert(sizeof(_send_race_boss_msg_from_web_request_wazo) == 276, "web raceboss request packet size mismatch");

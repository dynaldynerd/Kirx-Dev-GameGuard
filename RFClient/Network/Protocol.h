#pragma once

#define system_msg 1
#define check_msg 101
#define secure_msg 102

#define CHECK_KEY_NUM 4
#define KEY_NUM CHECK_KEY_NUM

#define enter_world_request_zone 1
#define enter_world_result_zone 2
#define reged_char_request_zone 8
#define reged_char_result_zone 9
#define sel_char_request_zone 14
#define sel_char_result_zone 15
#define server_notify_inform_zone 16
#define not_arranged_char_inform_zocl 21
#define alive_char_request_clzo 22
#define alive_char_result_zocl 23

#define ui_msg 13
#define uilock_inform_request_zocl 133

#define check_query 1
#define check_answer 2

#define check_speed_hack_ask 2
#define check_speed_hack_ans 3

#define NST_GAME_SERVER 0

#define MAX_GAME_SERVER 3
#define GST_NONE -1
#define GST_GATE 0
#define GST_WORLD 1
#define GST_ZONE 2

#define WORLD_ENTER_NOT_RECIVED 0xFF
#define WORLD_ENTER_SUCCESS 0
#define USER_INFO_NOT_RECIVED 0xFF
#define USER_INFO_SUCCESS 0
#define CHAR_SELECT_NOT_RECIVED 0xFF
#define CHAR_SELECT_SUCCESS 0

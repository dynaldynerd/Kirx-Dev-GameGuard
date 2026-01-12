# TODO / Notes

## Latest Progress
- AccountServer Settings UI: World List grid now uses explicit columns and Edit/Delete buttons at the end.
- World server can connect to the current AccountServer implementation.
- Client login still fails to get server list (AccountServer returns DB error in login flow).

## Next Steps
- Trace DoLogin/DoLoginAsync path in AccountServer (ret codes + DB call results) and log why DB error occurs.
- Verify DB connection settings and required stored procedures: Select_StaffInfoEx, Select_StaffExpire, Select_UserInfo, Insert_User, Select_UserSerial, Exist_UserBan, Select_UserAccountBlockInfo.
- Confirm AccountServer replies to LoginServer with _world_list_result_aclo after WorldListRequest and LoginServer forwards to client.
- Re-verify packet layouts for select-world flow if world list still wrong after DB fixes.

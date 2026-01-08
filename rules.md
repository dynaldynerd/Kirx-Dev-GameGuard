we will create a server emulator for a game
- on packet analysis, never pass PacketEnvelope to function handler, always use struct, for example:
don't put LoginAccountRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token), but instead LoginAccountRequest(PublicConnection connection, _login_account_request_loac_blit packet, CancellationToken token)

- always put packet struct on RFPackets library
- never ever ever create TryParse a packet function, instead on the RFPackets library, on the struct add bool load(byte[]) for parsing and ToArray for serealization

for accout server, on the original code, they 2 separate thread for dealing with packet and database,
hence we would usually see PushDQSData/queue from packet analysis, which then be passed to database thread, on there after done they would return back the result to dqscompleteprocess which is on the same thread as packet analysis for replying to the requester
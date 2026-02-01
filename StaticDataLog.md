## CLevel::LoadLevel static strings (from IDA)
- ".bsp"
- ".r3m"
- ".r3t"
- "\\" (path separator)
- "Lgt.R3T"
- "\\sky2\\"
- "sky2.r3e"
- "\\sky\\"
- "sky.r3e"
- ".rvp"
- "EXT.spt"
- Error msg (cp949 bytes): "\xC1\xB8\xC0\xE7\xC7\xCF\xC1\xF6\xBE\xCA\xC0\xBA \xB8\xCA\xC0\xBB \xB7\xCE\xB5\xF9\xC7\xCF\xB7\xC1 \xC7\xCF\xB0\xED\xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9."
- Error msg (cp949 bytes): "<-\xC0\xCC \xB8\xCA\xC6\xC4\xC0\xCF\xC0\xCC \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9."
- Error msg empty string: ""


## CSkyBox::LoadSkyBox / GetMaterialNameNum static data (from IDA)
- GetMaterialNameNum buf init: {'0','0','0','0','0','\0','0','0','0'}
## CEntity / R3 texture + memory static data (from IDA)
- Warning strings (labels; values not present in IDA dump): aAiAaiai_1, aAai
- R3 warnings/errors labels used: aR3tAaia_0, aR3tAaia, aAiAaiai_4, aA_3, aAiAaiaiAiau, aAiAaiai, aAiAaiai_5, aAiAaiai_2, aAiAaiai_3, aAo_2, aAlphaSortTrue
- CVertexBuffer/CIndexBuffer error labels: byte_140884910, byte_140884930, aAiu, aAiao
- Error/Warning suffix label: asc_140884708 (used to append suffix before callback)
- pass_word[32] dwords (from old R3Engine r3d3dtex.cpp; used by sub_1404FFFB0 UnLockDDS):
  0x764D802E, 0xF0D1F82E, 0x81863FBD, 0x3F3F2C58, 0x6F672E2E, 0x783F403F, 0xC0F13F3C, 0x9F3BF6A5,
  0xD73F20C1, 0x85E9C1C8, 0x56EFBD86, 0x2EFBA13F, 0x4C618687, 0xB44E3B21, 0x97AE5778, 0x2E4A2E3F,
  0x442E4C3F, 0xE85FC5CD, 0xBDEBECE9, 0x6CF7BBBE, 0x2EE4F22E, 0x9F973F3F, 0xB921B39D, 0x3F546576,
  0xF0C6F6E6, 0xB2E2DB79, 0xEB2E2E4B, 0xABCAD3D3, 0x9CEDC7EA, 0x65D0D9C7, 0x35FAB448, 0x9B6A2E2E


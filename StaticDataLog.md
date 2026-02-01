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

## CBsp / MergeFile / Error static data (from IDA + old R3Engine reference)
- CBsp::LoadBsp error strings (cp949 bytes):
  - aR3engineAe: "\x52\x33\x65\x6E\x67\x69\x6E\x65\x20\xC3\xCA\xB1\xE2\xC8\xAD\x20\xBA\xCE\xC5\xCD\xC7\xCF\xBC\xBC\xBF\xE4\x2E" ("R3engine 초기화 부터하세요.")
  - aAaiai_0: "\x3C\x2D\x20\xC6\xC4\xC0\xCF\xC0\xCC\x20\xBE\xF8\xBD\xBF\xB4\xD9\x2E" ("<- 파일이 없슴다.")
  - aBspAai: "\x42\x53\x50\xC6\xC4\xC0\xCF\x20\xB9\xF6\xC1\xAF\xC0\xCC\x20\xB8\xC2\xC1\xF6\xBE\xCA\xBD\xBF\xB4\xD9\x2E" ("BSP파일 버젼이 맞지않슴다.")
  - aAa (second arg for BSP version): "\xC0\xE5\xBE\xF0\xC0\xCF" ("장언일")
  - aMaxVbCnt: "\x5F\x4D\x41\x58\x5F\x56\x42\x5F\x43\x4E\x54\xB8\xA6\x20\xB3\xD1\xB0\xE5\xBD\xC0\xB4\xCF\xB4\xD9\x2E" ("_MAX_VB_CNT를 넘겼습니다.")
  - aMakeedgenormal: "\x4D\x61\x6B\x65\x45\x64\x67\x65\x4E\x6F\x72\x6D\x61\x6C\x28\x29\xB4\xC2\x20\xBC\xAD\xB9\xF6\xB8\xF0\xB5\xE5\x20\xC0\xCF\xB6\xA7\xB8\xB8\x20\xBD\xE1\xBE\xDF\xB5\xCA\x2E" ("MakeEdgeNormal()는 서버모드 일때만 써야됨.")
- Error/Warning suffix (cp949 bytes from old jerror.cpp): "\x3C\x2D\xBF\xA1\xB7\xAF" ("<-에러")
- Error static strings: "error_message.txt", "message"
- CMergeFileManager::InitMergeFile static strings: "%s\\*.*", "%s%s", ".", ".."
- aRpkAaiAo: IDA label used by InitMergeFile (warning message when >255 .rpk files); value not recovered yet.
- byte_184A790F0: default EntityPath string used for merge files (from old R3Engine core): ".\\Map\\Entity\\"
- byte_184A79924: env entity name table [2][128] (IDA BSS, zero-initialized)
- unk_184A7999C: env ID table [2] (IDA BSS, zero-initialized)
- GetVertexFromCompress function pointer table: index 1=GetVertexFromBVertex, 2=GetVertexFromWVertex, 4=GetVertexFromFVertex

## CBsp::LoadExtBsp / LoadEntities / Particle script static data (from IDA + old R3Engine reference)
- EBP mismatch error (cp949 bytes): "\x3C\x2D\x65\x62\x70\xC6\xC4\xC0\xCF\xC0\xCC\x20\x42\x53\x50\xBF\xCD\x20\xB8\xC2\xC1\xF6\xBE\xCA\xBD\xBF\xB4\xD9\x2E\x20\x65\x62\x70\xB8\xA6\x20\xB4\xD9\xB8\xA5\xB9\xF6\xC0\xFC\xC0\xB8\xB7\xCE\x20\xC0\xDB\xBE\xF7\xC7\xD1\xB5\xED" ("<-ebp파일이 BSP와 맞지않슴다. ebp를 다른버전으로 작업한듯")
- EBP version warning (cp949 bytes): "\x65\x62\x70\xC6\xC4\xC0\xCF\x20\xB9\xF6\xC1\xAF\xC0\xCC\x20\xB8\xC2\xC1\xF6\xBE\xCA\xBD\xBF\xB4\xD9\x2E" ("ebp파일 버젼이 맞지않슴다.")
- Particle warning (cp949 bytes): "\x20\x3C\x2D\x20\xC6\xC4\xC0\xCF\xC0\xCC\x20\xBE\xF8\xB0\xC5\xB3\xAA\x2C\x20\xC6\xC4\xC6\xBC\xC5\xAC\x20\x73\x70\x74\xB0\xA1\x20\xBE\xC6\xB4\xCF\xB4\xD9\x2E" (" <- 파일이 없거나, 파티클 spt가 아니다.")
- Particle file-missing warning (cp949 bytes): "\x3C\x2D\xC0\xCC\x20\xC6\xC4\xC0\xCF\xC0\xCC\xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9\x2E" ("<-이 파일이없습니다.")
- Particle not-script warning (cp949 bytes): "\x3C\x2D\xC0\xCC\xC6\xC4\xC0\xCF\xC0\xBA\x20\xC6\xC4\xC6\xBC\xC5\xAC\xBD\xBA\xC5\xA9\xB8\xB3\xC6\xAE\xB0\xA1\x20\xBE\xC6\xB4\xD5\xB4\xCF\xB4\xD9\x2E" ("<-이파일은 파티클스크립트가 아닙니다.")
- Particle pos command warning (cp949 bytes): "\xBD\xC3\xC0\xDB\xC0\xA7\xC4\xA1\x20\xC1\xF6\xC1\xA4\x20\xB8\xED\xB7\xC9\xBE\xEE\xB0\xA1\x20\xBE\xC6\xB4\xD5\xB4\xCF\xB4\xD9\x2E" ("시작위치 지정 명령어가 아닙니다.")
- Particle time-before-warning (cp949 bytes): "\xC0\xCC\x20\xB8\xED\xB7\xC9\xBE\xEE\xB4\xC2\x20\x74\x69\x6D\x65\xC0\xBB\x20\xB8\xD5\xC0\xFA\x20\xC1\xF6\xC1\xA4\xC7\xD1\xB5\xDA\x20\xBD\xE1\xC1\xD6\xBC\xBC\xBF\xE4\x2E" ("이 명령어는 time을 먼저 지정한뒤 써주세요.")
- Particle max-track error (cp949 bytes): "\xC3\xD6\xB4\xEB\x20\xC6\xAE\xB7\xA2\xBC\xF6\xB8\xA6\x20\xB3\xD1\xBE\xEE\xB9\xF6\xB8\xB3\xB4\xCF\xB4\xD9\x2E" ("최대 트랙수를 넘어버립니다.")
- Script read error (cp949 bytes): "\xBD\xBA\xC5\xA9\xB8\xB3\xC6\xAE\x20\x72\x65\x61\x64\x20\xBF\xC0\xB7\xF9\x2E" ("스크립트 read 오류.")
- Missing ')' error (cp949 bytes): "\x29\xB0\xA1\xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9\x2E" (")가없습니다.")
- byte_184A79270: default SoundPath string used in CBsp::LoadSoundEntities (from old R3Engine core): ".\\Snd\\"

## CLevel / R3 material / particle warnings (additional static strings to track)
- aAi_0: map file missing message (cp949 bytes), matches "<-이 맵파일이 없습니다."
- aA_1: map missing message (cp949 bytes), matches "존재하지않은 맵을 로딩하려 하고있습니다."
- aA_0: particle num warning (value unknown; used by CParticle::InitParticle)
- aA_2: particle live_time warning (value unknown; used by CParticle::InitParticle)
- aAi_1: mipmap size error (value unknown; used by GetMipMapSkipSize)
- aAiAaiai_1: entity file missing warning (value unknown; used by CEntity::LoadEntity)
- aAai: entity file format warning (value unknown; used by CEntity::LoadEntity)
- aAiAaiai_4: R3T missing/invalid warning (value unknown; used by R3GetTexInfoR3T)
- aA_3: filename-too-long warning (value unknown; used by R3GetTexInfoR3T / R3GetPreTextureId)
- aR3tAaia_0: R3T texture-mem warning (value unknown; used by R3LoadTextureMem)
- aR3tAaia: R3T texture-release warning (value unknown; used by R3ReleaseTextureMem)
- aAo_1: R3RestoreAllTextures warning (value unknown; used when R3T texnum != 1)
- aAi: R3T lightmap DDS error (value unknown; used by LoadR3TLightMap)
- aAiR3tAai: R3T header error (value unknown; used by LoadR3TLightMap)
- aAiAaiai_0: R3X header warning (value unknown; used by LoadR3X)
- aAo_2: material num overflow warning (value unknown; used by LoadSubMaterial / ReLoadMaterial)
- aAlphaSortTrue: "alpha_sort TRUE는 0번레이어에만.." (cp949 bytes; used by material layer parse)
- aTexture: default texture path string (from old source): ".\\Texture\\"
- byte_140883FF8: R3T lightmap size error (value unknown; used by LoadR3TLightMap)
- unk_1409788D4/unk_1409788D8: UnLockDDS XOR tables (derived from pass_word if not recovered)

## LoadSubMaterial / LoadSubR3M token strings (material parser)
- "end", "*MATERIAL_NUM", "*FOG_START", "*FOG_END", "*FOG_COLOR", "*FOG_COLOR2"
- "*FOG_START2", "*FOG_END2", "*NO_FOG_SKY", "*FOG_RANGE"
- "*LENS_FLARE_SCALE", "*LENS_FLARE", "*LENS_FLARE_POS"
- "*ENVIRONMENT", "*ENVIRONMENT_ID", "*ENVIRONMENT2", "*ENVIRONMENT_ID2"
- "*weather", "*haze", "*special_weather", "*no_sun", "*no_sky_color", "*use_fog_fog2"
- "layer_num", "no_lod_texture", "detail_texture", "detail_texture_scale"
- "no_collision", "water", "layer", "light_map", "sky_sun_light", "sky_night", "sky_day", "texture_splatting"
- "map_name", "uv_scroll_u", "uv_scroll_v", "uv_scale", "uv_scale_speed", "uv_scale_end"
- "uv_rotate", "uv_lava", "uv_lava_wave", "uv_gradient_alpha_u", "uv_gradient_alpha_v"
- "uv_metal_floor", "uv_metal_wall", "ani_alpha_flicker", "ani_alpha_flicker_start"
- "ani_alpha_flicker_end", "ani_tex_speed", "ani_tex_frame", "uv_env", "alpha_sort", "tex_clamp"
- "zwrite", "bump_env", "type", "alpha", "color"


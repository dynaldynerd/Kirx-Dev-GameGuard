#pragma once

  struct FireGuardFns
  {
    int (__fastcall *Initialize)(int (__fastcall *)(int, void *, void *, int, void *), unsigned __int8 *, unsigned int) = nullptr;
    void (__fastcall *Uninitialize)() = nullptr;
    int (__fastcall *EncryptPacket)(void *, unsigned __int8 *, int) = nullptr;
    int (__fastcall *DecryptPacket)(void *, unsigned __int8 *, int) = nullptr;
    unsigned int (__fastcall *GetLastError)() = nullptr;
    void *(__fastcall *CreateUserContext)(
      unsigned int,
      unsigned __int8 *,
      unsigned int,
      unsigned __int8 *,
      unsigned int,
      unsigned int,
      void *) = nullptr;
    void (__fastcall *CloseUserContext)(void **) = nullptr;
  };


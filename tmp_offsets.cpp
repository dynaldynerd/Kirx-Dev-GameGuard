#include <stddef.h>
#include <stdio.h>
#include "CAnimus.h"

int main(){
  printf("sizeof(CGameObject)=%zu\n", sizeof(CGameObject));
  printf("sizeof(CCharacter)=%zu\n", sizeof(CCharacter));
  printf("sizeof(CAnimus)=%zu\n", sizeof(CAnimus));
  printf("offsetof(CAnimus,m_pMaster)=%zu\n", offsetof(CAnimus, m_pMaster));
  printf("offsetof(CAnimus,m_dwMasterSerial)=%zu\n", offsetof(CAnimus, m_dwMasterSerial));
  return 0;
}

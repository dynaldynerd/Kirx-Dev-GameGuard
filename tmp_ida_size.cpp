#include <stddef.h>
#include <stdio.h>
#include "WorldServerIdaPro/structs/AVATOR_DATA.h"
int main(){
  printf("sizeof(_AVATOR_DATA)=%zu\n", sizeof(_AVATOR_DATA));
  printf("offsetof(dbPostData)=%zu\n", offsetof(_AVATOR_DATA, dbPostData));
  printf("offsetof(dbAnimus)=%zu\n", offsetof(_AVATOR_DATA, dbAnimus));
  printf("sizeof(_qry_sheet_load?) n/a\n");
  return 0;
}

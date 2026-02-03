#pragma once

#include "IdaCompat.h"

class CRecordData;

extern CRecordData *s_ptblItemData;

char WriteTableData(int itemCount, CRecordData *itemTables, bool useHash, char *errCode);

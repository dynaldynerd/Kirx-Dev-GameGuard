#pragma once

#include "IdaCompat.h"

class CRecordData;

char WriteTableData(int itemCount, CRecordData *itemTables, bool useHash, char *errCode);

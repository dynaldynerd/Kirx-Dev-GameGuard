#include "pch.h"
#include "CDummyPosTable.h"

bool CDummyPosTable::LoadDummyPosition(const char *szFileName, const char *szLabel)
{
    return true;
}

int CDummyPosTable::GetRecordNum()
{
    return 0;
}

void *CDummyPosTable::GetRecord(int index)
{
    return nullptr;
}

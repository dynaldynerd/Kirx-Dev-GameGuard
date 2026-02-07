#include "pch.h"

#include "CSkyBox.h"

#include <cstdlib>
#include <cstring>

int GetMaterialNameNum(struct _R3MATERIAL *mat)
{
  char buffer[256];

  strcpy(buffer, "00000");
  memset(&buffer[6], '0', 3);
  memset_0(&buffer[9], 0, sizeof(buffer) - 9);

  int index = static_cast<int>(strlen(mat->m_name)) - 1;
  if (index >= 0)
  {
    char *out = &buffer[4];
    do
    {
      const char ch = mat->m_name[index];
      if (ch == '_')
      {
        break;
      }
      *out-- = ch;
      --index;
    }
    while (index >= 0);
  }

  return atoi(buffer);
}

void CSkyBox::LoadSkyBox(char *szFileName)
{
  int materialNameNums[514];

  SetMergeFileManager(nullptr);
  if (static_cast<unsigned int>(this->mEntity.LoadEntity(szFileName, 0)))
  {
    _R3MATERIAL *materials = this->mEntity.mMat;
    this->mIsSkyLoad = 1;
    int materialIndex = 0;
    const int materialCount = materials->m_iMatNum;
    this->mMatGroupNum = this->mEntity.mMatGroupNum;
    this->mMatGroup = this->mEntity.mMatGroup;
    this->mMatNum = materialCount;
    if (materialCount > 0)
    {
      int *materialNameCursor = materialNameNums;
      do
      {
        const __int16 materialNameNum = GetMaterialNameNum(&this->mEntity.mMat[materialIndex++]);
        *materialNameCursor = materialNameNum;
        ++materialNameCursor;
      }
      while (materialIndex < this->mMatNum);
    }
    for (unsigned int groupIndex = 0; groupIndex < this->mMatGroupNum; ++groupIndex)
      memset_0(this->mMatGroupSort, 0, 2LL * this->mMatGroupNum);

    __int64 sortIndex = 0;
    unsigned int materialNameIndex = 0;
    if (this->mMatNum)
    {
      int *materialNameIter = materialNameNums;
      do
      {
        unsigned int groupIter = 0;
        if (this->mMatGroupNum)
        {
          const int materialName = *materialNameIter;
          __int16 *sortCursor = &this->mMatGroupSort[sortIndex];
          do
          {
            if (materialName == this->mMatGroup[groupIter].MtlId)
            {
              *sortCursor = static_cast<__int16>(groupIter);
              ++sortIndex;
              ++sortCursor;
            }
            ++groupIter;
          }
          while (groupIter < this->mMatGroupNum);
        }
        ++materialNameIndex;
        ++materialNameIter;
      }
      while (materialNameIndex < this->mMatNum);
    }
  }
}


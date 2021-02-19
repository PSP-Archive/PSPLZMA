
/* 7zExtract.c */


// *** INCLUDES ***

#include "7zExtract.h"
#include "7zDecode.h"
#include "7zCrc.h"


// *** FUNCTIONS ***

SZ_RESULT SzExtract (ISzInStream *inStream, CArchiveDatabaseEx *db, UInt32 fileIndex, UInt32 *blockIndex, Byte **outBuffer, size_t *outBufferSize, size_t *offset, size_t *outSizeProcessed, ISzAlloc *allocMain, ISzAlloc *allocTemp)
{
 UInt32 folderIndex = db->FileIndexToFolderIndexMap[fileIndex];
 SZ_RESULT res = SZ_OK;
 size_t outRealSize;
 UInt32 i;


 *offset = 0;
 *outSizeProcessed = 0;

 if (folderIndex == (UInt32) -1)
 {
  allocMain->Free(*outBuffer);
  *blockIndex = folderIndex;
  *outBuffer = 0;
  *outBufferSize = 0;

  return SZ_OK;
 }

 if ((*outBuffer == 0) || (*blockIndex != folderIndex))
 {
  CFolder *folder = db->Database.Folders + folderIndex;
  CFileSize unPackSize = SzFolderGetUnPackSize(folder);
  *blockIndex = folderIndex;

  allocMain->Free(*outBuffer);
  *outBuffer = 0;
    
  RINOK(inStream->Seek(inStream, SzArDbGetFolderStreamPos(db, folderIndex, 0)));
    
  if (res == SZ_OK)
  {
   *outBufferSize = (size_t) unPackSize;

   if (unPackSize != 0)
   {
    *outBuffer = (Byte *)allocMain->Alloc((size_t)unPackSize);
    if (*outBuffer == 0) res = SZE_OUTOFMEMORY;
   }

   if (res == SZ_OK)
   {
    res = SzDecode(db->Database.PackSizes + db->FolderStartPackStreamIndex[folderIndex], folder, inStream, *outBuffer, (size_t)unPackSize, &outRealSize, allocTemp);

    if (res == SZ_OK)
    {
     if (outRealSize == (size_t) unPackSize)
     {
      if (folder->UnPackCRCDefined)
      {
       if (!CrcVerifyDigest(folder->UnPackCRC, *outBuffer, (size_t)unPackSize)) res = SZE_FAIL;
      }
     }
     else
      res = SZE_FAIL;
    }
   }
  }
 }

 if (res == SZ_OK)
 {
  CFileItem *fileItem = db->Database.Files + fileIndex;
  *offset = 0;

  for(i = db->FolderStartFileIndex[folderIndex]; i < fileIndex; i++) *offset += (UInt32)db->Database.Files[i].Size;
  *outSizeProcessed = (size_t)fileItem->Size;

  if (*offset + *outSizeProcessed > *outBufferSize) return SZE_FAIL;
 
  if (fileItem->IsFileCRCDefined)
  {
   if (!CrcVerifyDigest(fileItem->FileCRC, *outBuffer + *offset, *outSizeProcessed)) res = SZE_FAIL;
  }
 }
 return res;
}

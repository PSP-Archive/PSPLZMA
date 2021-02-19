
/* 7zItem.h */

#ifndef __7Z_ITEM_H
#define __7Z_ITEM_H


// *** INCLUDES ***

#include "7zMethodID.h"
#include "7zHeader.h"
#include "7zBuffer.h"


// *** STRUCTURES ***

typedef struct _CCoderInfo
{
 UInt32 NumInStreams;
 UInt32 NumOutStreams;
 CMethodID MethodID;
 CSzByteBuffer Properties;
} CCoderInfo;

typedef struct _CBindPair
{
 UInt32 InIndex;
 UInt32 OutIndex;
} CBindPair;

typedef struct _CFolder
{
 UInt32 NumCoders;
 CCoderInfo *Coders;
 UInt32 NumBindPairs;
 CBindPair *BindPairs;
 UInt32 NumPackStreams; 
 UInt32 *PackStreams;
 CFileSize *UnPackSizes;
 int UnPackCRCDefined;
 UInt32 UnPackCRC;
 UInt32 NumUnPackStreams;
} CFolder;

typedef struct _CFileItem
{
 CFileSize Size;
 UInt32 FileCRC;
 char *Name;
 Byte IsFileCRCDefined;
 Byte HasStream;
 Byte IsDirectory;
 Byte IsAnti;
} CFileItem;

typedef struct _CArchiveDatabase
{
 UInt32 NumPackStreams;
 CFileSize *PackSizes;
 Byte *PackCRCsDefined;
 UInt32 *PackCRCs;
 UInt32 NumFolders;
 CFolder *Folders;
 UInt32 NumFiles;
 CFileItem *Files;
} CArchiveDatabase;


// *** FUNCTIONS DECLARATIONS ***

void SzCoderInfoInit (CCoderInfo *coder);
void SzCoderInfoFree (CCoderInfo *coder, void (*freeFunc)(void *p));

void SzFolderInit (CFolder *folder);
CFileSize SzFolderGetUnPackSize (CFolder *folder);
int SzFolderFindBindPairForInStream (CFolder *folder, UInt32 inStreamIndex);
UInt32 SzFolderGetNumOutStreams (CFolder *folder);
CFileSize SzFolderGetUnPackSize (CFolder *folder);

void SzFileInit(CFileItem *fileItem);

void SzArchiveDatabaseInit(CArchiveDatabase *db);
void SzArchiveDatabaseFree(CArchiveDatabase *db, void (*freeFunc)(void *));

#endif

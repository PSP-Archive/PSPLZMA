
/* 7zIn.h */

#ifndef __7Z_IN_H
#define __7Z_IN_H


// *** INCLUDES ***

#include "7zHeader.h"
#include "7zItem.h"
#include "7zAlloc.h"


// *** STRUCTURES ***

typedef struct _CInArchiveInfo
{
 CFileSize StartPositionAfterHeader; 
 CFileSize DataStartPosition;
} CInArchiveInfo;

typedef struct _CArchiveDatabaseEx
{
 CArchiveDatabase Database;
 CInArchiveInfo ArchiveInfo;
 UInt32 *FolderStartPackStreamIndex;
 CFileSize *PackStreamStartPositions;
 UInt32 *FolderStartFileIndex;
 UInt32 *FileIndexToFolderIndexMap;
} CArchiveDatabaseEx;

typedef struct _CSzState
{
 Byte *Data;
 size_t Size;
} CSzData;

typedef struct _ISzInStream
{
 SZ_RESULT (*Read) (
					void *object,           /* pointer to ISzInStream itself */
					void **buffer,          /* out: pointer to buffer with data */
					size_t maxRequiredSize, /* max required size to read */
					size_t *processedSize); /* real processed size. processedSize can be less than maxRequiredSize. If processedSize == 0, then there are no more bytes in stream. */
 SZ_RESULT (*Seek) (void *object, CFileSize pos);
} ISzInStream;


// *** FUNCTIONS DECLARATIONS ***

void SzArDbExInit (CArchiveDatabaseEx *db);
void SzArDbExFree (CArchiveDatabaseEx *db, void (*freeFunc)(void *));
CFileSize SzArDbGetFolderStreamPos (CArchiveDatabaseEx *db, UInt32 folderIndex, UInt32 indexInFolder);
CFileSize SzArDbGetFolderFullPackSize (CArchiveDatabaseEx *db, UInt32 folderIndex);
int SzArchiveOpen (ISzInStream *inStream, CArchiveDatabaseEx *db, ISzAlloc *allocMain, ISzAlloc *allocTemp);
 
#endif

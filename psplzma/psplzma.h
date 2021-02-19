
/* psplzma.h */

#ifndef __PSP_LZMA_H
#define __PSP_LZMA_H


// *** INCLUDES ***

#include <stdio.h>
#include <string.h>
#include "7zCrc.h"
#include "7zIn.h"
#include "7zExtract.h"


// *** DEFINES ***

#define kBufferSize (1 << 17)		// 128 ko

#define SEVENZIP_GETINDEX(archive,item)	(item - archive->db.Database.Files)


// *** STRUCTURES ***

typedef struct SevenZipArchive
{
 ISzInStream inStream;				// First in structure because his address is used for access to SevenZipArchive struct
 SceUID fd;
 char filename[1024];
 ISzAlloc allocImp, allocTempImp;
 CArchiveDatabaseEx db;
} SevenZipArchive;

typedef struct SevenZipItemName
{
 char folder[1024];					// Name of the base folder
 char extract[1024];				// Name of the extracted file
} SevenZipItemName;


// *** FUNCTIONS DECLARATIONS ***

SevenZipArchive *sevenZipOpen (const char *filename);
void sevenZipClose (SevenZipArchive *archive);

int sevenZipBrowse (SevenZipArchive *archive, int (*func)(SevenZipArchive *, CFileItem *, void *), void *arg);

CFileItem *sevenZipNameToItem (SevenZipArchive *archive, const char *name);

int sevenZipExtractI (SevenZipArchive *archive, CFileItem *item, const char *foldername, const char *extractname);
int sevenZipExtractN (SevenZipArchive *archive, const char *name, const char *foldername, const char *extractname);
int sevenZipExtractAll (SevenZipArchive *archive, const char *foldername);

int sevenZipCheckI (SevenZipArchive *archive, CFileItem *item);
int sevenZipCheckN (SevenZipArchive *archive, const char *name);
int sevenZipCheckAll (SevenZipArchive *archive);

#endif

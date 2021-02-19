
/* 
	psplzma.c
	LZMA SDK 4.43 Copyright (c) 1999-2006 Igor Pavlov (2006-06-04)
*/

// *** INCLUDES ***

#include "psplzma.h"


// *** GLOBALS VARIABLES ***

Byte g_Buffer[kBufferSize];


// *** FUNCTIONS ***

SZ_RESULT SzFileReadImp (void *object, void **buffer, size_t maxRequiredSize, size_t *processedSize)
{
 SevenZipArchive *s = (SevenZipArchive *) object;
 size_t processedSizeLoc;


 if (maxRequiredSize > kBufferSize) maxRequiredSize = kBufferSize;

 processedSizeLoc = sceIoRead(s->fd,g_Buffer,maxRequiredSize);
 *buffer = g_Buffer;

 if (processedSize) *processedSize = processedSizeLoc;

 return SZ_OK;
}

SZ_RESULT SzFileSeekImp (void *object, CFileSize pos)
{
 SevenZipArchive *s = (SevenZipArchive *) object;


 int res = sceIoLseek(s->fd,(long) pos,PSP_SEEK_SET);

 if (res == (int) pos) return SZ_OK;

 return SZE_FAIL;
}


SevenZipArchive *sevenZipOpen (const char *filename)
{
 SevenZipArchive *archive;
 SceUID fd;


 // Verify parameters
 if (!(filename)) return NULL;

 // Open file
 fd = sceIoOpen(filename,PSP_O_RDONLY,0);
 if (fd < 0) return NULL;

 // Create archive
 archive = (SevenZipArchive *) malloc(sizeof(SevenZipArchive));
 if (!(archive))
 {
  sceIoClose(fd);
  return NULL;
 }

 // Initialize CRC table
 InitCrcTable();

 // Initialize archive
 strcpy(archive->filename,filename);

 archive->fd = fd;

 archive->inStream.Read = SzFileReadImp;
 archive->inStream.Seek = SzFileSeekImp;

 archive->allocImp.Alloc = SzAlloc;
 archive->allocImp.Free = SzFree;

 archive->allocTempImp.Alloc = SzAllocTemp;
 archive->allocTempImp.Free = SzFreeTemp;

 SzArDbExInit(&archive->db);

 // Open archive
 if (SzArchiveOpen(&archive->inStream,&archive->db,&archive->allocImp,&archive->allocTempImp) != SZ_OK)
 {
  free(archive);
  sceIoClose(fd);

  return NULL; 
 }
 
 return archive;
}

void sevenZipClose (SevenZipArchive *archive)
{
 // Verify parameters
 if (!(archive)) return;

 // Free database
 SzArDbExFree(&archive->db,archive->allocImp.Free);

 // Close fd
 sceIoClose(archive->fd);

 // Free archive
 free(archive);
}

int sevenZipBrowse (SevenZipArchive *archive, int (*func)(SevenZipArchive *, CFileItem *, void *), void *arg)
{
 UInt32 i;
 int ret;


 // Verify parameters
 if ((!(archive)) || (!(func))) return 1;

 // Browse
 for (i=0;i<archive->db.Database.NumFiles;i++)
 {
  ret = func(archive,archive->db.Database.Files + i,arg);
  if (ret) return ret;
 }
 
 return 0;
}

int sevenZipCreateFolders (const char *name)
{
 char folder[1024], *p;


 // Verify parameters
 if (!(name)) return 1;

 // Copy string
 strcpy(folder,name);

 // Browse string
 for (p=strchr(folder,'/');p;p=strchr(p + 1,'/'))
 {
  // End name
  *p = 0;

  // Create folder
  sceIoMkdir(folder,0777);

  // Restore name
  *p = '/';
 }

 return 0;
}

int sevenZipNameToItem_browse (SevenZipArchive *archive, CFileItem *item, void *arg)
{
 // Verify parameters
 if ((!(archive)) || (!(item))) return 1;

 // Compare name
 if (!(strcmp(item->Name,(const char *) arg))) return (int) item;

 return 0;
}

CFileItem *sevenZipNameToItem (SevenZipArchive *archive, const char *name)
{
 return (CFileItem *) sevenZipBrowse(archive,sevenZipNameToItem_browse,(void *) name);
}

int sevenZipExtract_browse (SevenZipArchive *archive, CFileItem *item, void *arg)
{
 char name[1024];
 SceUID fd;
 UInt32 processedSize;
 size_t offset, outSizeProcessed;
 UInt32 blockIndex = 0xFFFFFFFF;	// it can have any value before first call (if outBuffer = 0)
 Byte *outBuffer = 0;				// it must be 0 before first call for each new archive.
 size_t outBufferSize = 0;			// it can have any value before first call (if outBuffer = 0)


 // Verify parameters
 if ((!(archive)) || (!(item))) return 1;

 // Make name
 sprintf(name,"%s/%s",((SevenZipItemName *) arg)->folder,(((SevenZipItemName *) arg)->extract[0]) ? ((SevenZipItemName *) arg)->extract : item->Name);

 // If directory
 if (item->IsDirectory)
 {
  // Create folder
  sceIoMkdir(name,0777);

  return 0;
 }

 // Extract file
 if (SzExtract(&archive->inStream,&archive->db,SEVENZIP_GETINDEX(archive,item),&blockIndex,&outBuffer,&outBufferSize,&offset,&outSizeProcessed,&archive->allocImp,&archive->allocTempImp) != SZ_OK) return 1;

 // Open output file
 fd = sceIoOpen(name,PSP_O_RDWR | PSP_O_CREAT | PSP_O_TRUNC,0777);
 if (fd < 0) return 1;

 // Write file
 processedSize = sceIoWrite(fd,outBuffer + offset,outSizeProcessed);
 if (processedSize != outSizeProcessed)
 {
  sceIoClose(fd);
  return 1;
 }

 // Close file
 sceIoClose(fd);

 return 0;
}

int sevenZipExtractI (SevenZipArchive *archive, CFileItem *item, const char *foldername, const char *extractname)
{
 SevenZipItemName itemName;
 char temp[1024], *p;


 // Verify parameters
 if ((!(archive)) || (!(item))) return 1;

 // Make install folder name
 p = strrchr(archive->filename,'/');
 strncpy(itemName.folder,archive->filename,p - archive->filename);
 itemName.folder[p - archive->filename] = 0;

 if (foldername)
 {
  if (foldername[0]) strcpy(itemName.folder,foldername);
 }

 if (itemName.folder[strlen(itemName.folder) - 1] == '/') itemName.folder[strlen(itemName.folder) - 1] = 0;

 // Make extract name
 itemName.extract[0] = 0;
 if (extractname)
 {
  strcpy(itemName.extract,extractname);

  // Create folders string
  sprintf(temp,"%s/%s",itemName.folder,extractname);
 }
 else
 {
  // Create folders string
  sprintf(temp,"%s/%s",itemName.folder,item->Name);
 }

 // Create folder if not exists
 sevenZipCreateFolders(temp);

 // Call extract function
 return sevenZipExtract_browse(archive,item,(void *) &itemName);
}

int sevenZipExtractN (SevenZipArchive *archive, const char *name, const char *foldername, const char *extractname)
{
 return sevenZipExtractI(archive,sevenZipNameToItem(archive,name),foldername,extractname);
}

int sevenZipExtractAll (SevenZipArchive *archive, const char *foldername)
{
 SevenZipItemName itemName;
 char *p;


 // Verify parameters
 if (!(archive)) return 1;

 // Make install folder name
 p = strrchr(archive->filename,'/');
 strncpy(itemName.folder,archive->filename,p - archive->filename);
 itemName.folder[p - archive->filename] = 0;

 if (foldername)
 {
  if (foldername[0]) strcpy(itemName.folder,foldername);
 }

 if (itemName.folder[strlen(itemName.folder) - 1] != '/') strcat(itemName.folder,"/");

 // Create folder if not exists
 sevenZipCreateFolders(itemName.folder);

 itemName.folder[strlen(itemName.folder) - 1] = 0;

 // No rename file in archive when extract all
 itemName.extract[0] = 0;

 // Call browse function
 return sevenZipBrowse(archive,sevenZipExtract_browse,(void *) &itemName);
}

int sevenZipCheck_browse (SevenZipArchive *archive, CFileItem *item, void *arg)
{
 size_t offset, outSizeProcessed;
 UInt32 blockIndex = 0xFFFFFFFF;	// it can have any value before first call (if outBuffer = 0)
 Byte *outBuffer = 0;				// it must be 0 before first call for each new archive.
 size_t outBufferSize = 0;			// it can have any value before first call (if outBuffer = 0)


 // Verify parameters
 if ((!(archive)) || (!(item))) return 1;

 // Check file
 return (SzExtract(&archive->inStream,&archive->db,SEVENZIP_GETINDEX(archive,item),&blockIndex,&outBuffer,&outBufferSize,&offset,&outSizeProcessed,&archive->allocImp,&archive->allocTempImp));
}

int sevenZipCheckI (SevenZipArchive *archive, CFileItem *item)
{
 // Call check function
 return sevenZipCheck_browse(archive,item,NULL);
}

int sevenZipCheckN (SevenZipArchive *archive, const char *name)
{
 return sevenZipCheckI(archive,sevenZipNameToItem(archive,name));
}

int sevenZipCheckAll (SevenZipArchive *archive)
{
 // Verify parameters
 if (!(archive)) return 1;

 // Call check function
 return sevenZipBrowse(archive,sevenZipCheck_browse,NULL);
}

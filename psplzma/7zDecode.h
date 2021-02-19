
/* 7zDecode.h */

#ifndef __7Z_DECODE_H
#define __7Z_DECODE_H


// *** INCLUDES ***

#include "LZMA_C/LzmaDecode.h"
#include "7zItem.h"
#include "7zAlloc.h"
#include "7zIn.h"


// *** STRUCTURES ***

typedef struct _CLzmaInCallbackImp
{
 ILzmaInCallback InCallback;
 ISzInStream *InStream;
 size_t Size;
} CLzmaInCallbackImp;


// *** FUNCTIONS DECLARATIONS ***

SZ_RESULT SzDecode (const CFileSize *packSizes, const CFolder *folder, ISzInStream *stream, Byte *outBuffer, size_t outSize, size_t *outSizeProcessed, ISzAlloc *allocMain);

#endif

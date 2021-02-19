/* 
  LzmaDecode.h
  LZMA Decoder interface

  LZMA SDK 4.40 Copyright (c) 1999-2006 Igor Pavlov (2006-05-01)
  http://www.7-zip.org/

  LZMA SDK is licensed under two licenses:
  1) GNU Lesser General Public License (GNU LGPL)
  2) Common Public License (CPL)
  It means that you can select one of these two licenses and 
  follow rules of that license.

  SPECIAL EXCEPTION:
  Igor Pavlov, as the author of this code, expressly permits you to 
  statically or dynamically link your code (or bind by name) to the 
  interfaces of this file without subjecting your linked code to the 
  terms of the CPL or GNU LGPL. Any modifications or additions 
  to this file, however, are subject to the LGPL or CPL terms.
*/

#ifndef __LZMADECODE_H
#define __LZMADECODE_H


// *** INCLUDES ***

#include "LzmaTypes.h"


// *** DEFINES ***

#define CProb UInt16

#define LZMA_RESULT_OK 0
#define LZMA_RESULT_DATA_ERROR 1

#define LZMA_BASE_SIZE 1846
#define LZMA_LIT_SIZE 768

#define LZMA_PROPERTIES_SIZE 5

#define LzmaGetNumProbs(Properties) (LZMA_BASE_SIZE + (LZMA_LIT_SIZE << ((Properties)->lc + (Properties)->lp)))

#define kLzmaNeedInitId (-2)

#define LzmaDecoderInit(vs) { (vs)->RemainLen = kLzmaNeedInitId; }


// *** STRUCTURES ***

typedef struct _ILzmaInCallback
{
 int (*Read) (void *object, const unsigned char **buffer, SizeT *bufferSize);
} ILzmaInCallback;

typedef struct _CLzmaProperties
{
 int lc;
 int lp;
 int pb;
 UInt32 DictionarySize;
} CLzmaProperties;

typedef struct _CLzmaDecoderState
{
 CLzmaProperties Properties;
 CProb *Probs;

 const unsigned char *Buffer;
 const unsigned char *BufferLim;

 unsigned char *Dictionary;
 UInt32 Range;
 UInt32 Code;
 UInt32 DictionaryPos;
 UInt32 GlobalPos;
 UInt32 DistanceLimit;
 UInt32 Reps[4];
 int State;
 int RemainLen;
 unsigned char TempDictionary[4];
} CLzmaDecoderState;


// *** FUNCTIONS DECLARATIONS ***

int LzmaDecodeProperties (CLzmaProperties *propsRes, const unsigned char *propsData, int size);
int LzmaDecode (CLzmaDecoderState *vs, ILzmaInCallback *inCallback, unsigned char *outStream, SizeT outSize, SizeT *outSizeProcessed);

#endif

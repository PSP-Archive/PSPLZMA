
/* 7zDecode.c */


// *** INCLUDES ***

#include "7zDecode.h"


// *** GLOBALS VARIABLES ***

CMethodID k_Copy = { { 0x0 }, 1 };
CMethodID k_LZMA = { { 0x3, 0x1, 0x1 }, 3 };


// *** FUNCTIONS ***

int LzmaReadImp (void *object, const unsigned char **buffer, SizeT *size)
{
 CLzmaInCallbackImp *cb = (CLzmaInCallbackImp *) object;
 size_t processedSize;
 SZ_RESULT res;


 *size = 0;
 res = cb->InStream->Read((void *)cb->InStream, (void **)buffer, cb->Size, &processedSize);
 *size = (SizeT) processedSize;

 if (processedSize > cb->Size) return (int) SZE_FAIL;
 cb->Size -= processedSize;

 if (res == SZ_OK) return 0;

 return (int) res;
}

SZ_RESULT SzDecode (const CFileSize *packSizes, const CFolder *folder, ISzInStream *inStream, Byte *outBuffer, size_t outSize, size_t *outSizeProcessed, ISzAlloc *allocMain)
{
 UInt32 si;
 size_t inSize = 0, i, j, bufferSize;
 CCoderInfo *coder;
 Byte *inBuffer;
 CLzmaInCallbackImp lzmaCallback;
 CLzmaDecoderState state;			/* it's about 24-80 bytes structure, if int is 32-bit */
 SizeT outSizeProcessedLoc;
 int result;


 if (folder->NumPackStreams != 1) return SZE_NOTIMPL;
 if (folder->NumCoders != 1) return SZE_NOTIMPL;

 coder = folder->Coders;
 *outSizeProcessed = 0;

 for (si=0;si<folder->NumPackStreams;si++) inSize += (size_t) packSizes[si];

 if (AreMethodsEqual(&coder->MethodID, &k_Copy))
 {
  if (inSize != outSize) return SZE_DATA_ERROR;

  for (i=0;i<inSize;)
  {
   RINOK(inStream->Read((void *)inStream,  (void **)&inBuffer, inSize - i, &bufferSize));

   if (bufferSize == 0) return SZE_DATA_ERROR;
   if (bufferSize > inSize - i) return SZE_FAIL;

   *outSizeProcessed += bufferSize;
   for (j=0;(j<bufferSize) && (i<inSize);j++,i++) outBuffer[i] = inBuffer[j];
  }

  return SZ_OK;
 }

 if (AreMethodsEqual(&coder->MethodID, &k_LZMA))
 {
  lzmaCallback.Size = inSize;
  lzmaCallback.InStream = inStream;
  lzmaCallback.InCallback.Read = LzmaReadImp;

  if (LzmaDecodeProperties(&state.Properties, coder->Properties.Items, coder->Properties.Capacity) != LZMA_RESULT_OK) return SZE_FAIL;

  state.Probs = (CProb *)allocMain->Alloc(LzmaGetNumProbs(&state.Properties) * sizeof(CProb));

  if (state.Probs == 0) return SZE_OUTOFMEMORY;
  if (state.Properties.DictionarySize == 0)
   state.Dictionary = 0;
  else
  {
   state.Dictionary = (unsigned char *) allocMain->Alloc(state.Properties.DictionarySize);

   if (state.Dictionary == 0)
   {
    allocMain->Free(state.Probs);
    return SZE_OUTOFMEMORY;
   }
  }
  LzmaDecoderInit(&state);

  result = LzmaDecode(&state, &lzmaCallback.InCallback, outBuffer, (SizeT)outSize, &outSizeProcessedLoc);

  *outSizeProcessed = (size_t)outSizeProcessedLoc;
  allocMain->Free(state.Probs);
  allocMain->Free(state.Dictionary);

  if (result == LZMA_RESULT_DATA_ERROR) return SZE_DATA_ERROR;
  if (result != LZMA_RESULT_OK) return SZE_FAIL;

  return SZ_OK;
 }

 return SZE_NOTIMPL;
}

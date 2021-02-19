
/* 7zMethodID.h */

#ifndef __7Z_METHOD_ID_H
#define __7Z_METHOD_ID_H


// *** INCLUDES ***

#include "7zTypes.h"


// *** DEFINES ***

#define kMethodIDSize 15


// *** STRUCTURES ***
  
typedef struct _CMethodID
{
 Byte ID[kMethodIDSize];
 Byte IDSize;
} CMethodID;


// *** FUNCTIONS DECLARATIONS ***

int AreMethodsEqual (CMethodID *a1, CMethodID *a2);

#endif

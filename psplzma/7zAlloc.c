
/* 7zAlloc.c */


// *** INCLUDES ***

#include <stdlib.h>
#include "7zAlloc.h"


// *** FUNCTIONS ***

void *SzAlloc (size_t size)
{
 if (!(size)) return 0;

 return malloc(size);
}

void SzFree (void *address)
{
 if (address) free(address);
}

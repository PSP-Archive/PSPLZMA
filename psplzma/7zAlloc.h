
/* 7zAlloc.h */

#ifndef __7Z_ALLOC_H
#define __7Z_ALLOC_H


// *** INCLUDES ***

#include <stddef.h>
#include <malloc.h>


// *** DEFINES ***

#define SzAllocTemp	SzAlloc
#define SzFreeTemp	SzFree


// *** STRUCTURES ***

typedef struct _ISzAlloc
{
 void *(*Alloc) (size_t size);
 void (*Free) (void *address); /* address can be 0 */
} ISzAlloc;


// *** FUNCTIONS DECLARATIONS ***

void *SzAlloc (size_t size);
void SzFree (void *address);

#endif

TARGET = out
OBJS = $(wildcard *.c psplzma/*.c psplzma/LZMA_C/*.c)

USE_PSPSDK_LIBC = 1


INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

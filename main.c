
/* 
	7z on PSP sample based on :
	LZMA SDK 4.43 Copyright (c) 1999-2006 Igor Pavlov (2006-06-04)
*/


// *** INCLUDES ***

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "psplzma/psplzma.h"


// *** MODULE INITIALISATION ***

PSP_MODULE_INFO("Test",0x0,1,1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);


// *** FUNCTIONS ***

#define printf					pspDebugScreenPrintf

int CallbackExit (int arg1, int arg2, void *common)
{
 // Exit without saving
 sceKernelExitGame();

 return 0;
}

int CallbackThread (SceSize args, void *argp)
{
 int cbid;


 cbid = sceKernelCreateCallback("ExitCallback",CallbackExit,NULL);
 sceKernelRegisterExitCallback(cbid);

 sceKernelSleepThreadCB();

 return 0;
}

int SetupCallbacks (void)
{
 int thid;


 thid = sceKernelCreateThread("UpdateThread",CallbackThread,0x11,0xFA0,PSP_THREAD_ATTR_USER,0);
 if (thid >= 0) sceKernelStartThread(thid,0,0);

 return thid;
}

u32 waitButton (u32 codemask, u32 block)
{
 SceCtrlData pad;
 static u32 oldButtons = 0;
 

 for (;;)
 {
  // Read the buttons
  sceCtrlReadBufferPositive(&pad,1);

  // Check the buttons
  if ((oldButtons != pad.Buttons) || (!(block)))
  {
   // Save the buttons
   oldButtons = pad.Buttons;

   // Exit if ok
   if (pad.Buttons & codemask) return pad.Buttons;
  }

  // For wait without block
  sceDisplayWaitVblankStart();
 }
}

void exitError (const char *error)
{
 printf("\nError : %s\nPress X for exit\n",error);
 waitButton(PSP_CTRL_CROSS,1);

 sceKernelExitGame();
}

int funcPrintAndExtractJPG (SevenZipArchive *archive, CFileItem *item, void *arg)
{
 int n;


 // Print name of the item
 printf("%s\n",item->Name);

 // Extract only JPG files
 n = strlen(item->Name);
 if (n > 4)
 {
  if (!(strcasecmp(&item->Name[n - 4],".jpg"))) sevenZipExtractI(archive,item,"ms0:/7z/jpg",NULL);
 }

 return 0;
}

int main (void)
{
 SevenZipArchive *archive;


 // Init display and HOME button
 SetupCallbacks();
 pspDebugScreenInit();
 pspDebugScreenClear();

 // Setup Pad
 sceCtrlSetSamplingCycle(0);
 sceCtrlSetSamplingMode(0);

 printf("7z decoding sample\n");
 printf("------------------\n\n");
 printf("Portage of : LZMA SDK   Copyright (C) 1999-2006 Igor Pavlov\n");
 printf("for PSP by mph\n\n");

 // Open archive
 printf("open archive ...  ");

 archive = sevenZipOpen("ms0:/archive.7z");
 if (!(archive)) exitError("cannot open file");

 printf("archive opened\n");

 // Extract all files in ms0:/7z/
 printf("sevenZipExtractAll return : 0x%X\n",sevenZipExtractAll(archive,"ms0:/7z"));

 // Extract all files without specify folder
 printf("sevenZipExtractAll return : 0x%X\n",sevenZipExtractAll(archive,NULL));

 // Extract only one file (test/test.txt) without specify folder nor extract name
 printf("sevenZipExtractN return : 0x%X\n",sevenZipExtractN(archive,"test/test.txt",NULL,NULL));

 // Extract only one file (test/test.txt) in ms0:/extract/ nor extract name
 printf("sevenZipExtractN return : 0x%X\n",sevenZipExtractN(archive,"test/test.txt","ms0:/extract",NULL));

 // Extract only one file (test/test.txt) without specify folder with extract name = rename.txt
 printf("sevenZipExtractN return : 0x%X\n",sevenZipExtractN(archive,"test/test.txt",NULL,"rename.txt"));

 // Extract only one file (test/test.txt) in ms0:/extract with extract name = rename.txt
 printf("sevenZipExtractN return : 0x%X\n",sevenZipExtractN(archive,"test/test.txt","ms0:/extract","rename.txt"));

 // Check all files
 printf("sevenZipCheckAll return : 0x%X\n",sevenZipCheckAll(archive));

 // Check only one file (test/test.txt)
 printf("sevenZipCheckN return : 0x%X\n",sevenZipCheckN(archive,"test/test.txt"));

 // Use browse function to print files and folders of the archive and extract only *.jpg files
 printf("sevenZipBrowse return : 0x%X\n",sevenZipBrowse(archive,funcPrintAndExtractJPG,NULL));

 // Close archive
 sevenZipClose(archive);

 printf("Press X for exit\n");
 waitButton(PSP_CTRL_CROSS,1);
 
 // Exit with sceKernelExitGame to manage the modifications
 sceKernelExitGame();

 return 0;
}


@REM Ne pas afficher les commandes
@ECHO OFF

ECHO Compilation du projet

SET path=%path%;C:\cygwin\bin;C:\cygwin\usr/local/pspdev/bin
SET PSPSDK=C:\cygwin\usr/local/pspdev

make.exe

ECHO Effacer les fichiers inutiles
DEL *.o

ECHO Reduction de la taille du programme

psp-strip out.elf -o out2.elf
DEL out.elf

RENAME out2.elf EBOOT.PBP

pause
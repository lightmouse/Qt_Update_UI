
GCC and Qt Creator CANLIB demo application
------------------------------------------

The program is written in C++ and uses the Qt cross-platform UI framework. It 
has been tested with GCC under Linux and Windows (MinGW).

The program is just intended as an example of how to use CANLIB. It's not
performance optimized in any way. It may, or may not, be useful
for you.

Windows build instructions
--------------------------
Set INCLUDEPATH in demo.pro to where you have canlib.h, canstat.h, canevt.h, 
obsolete.h and predef.h.

When linking, an import library called canlib32.a is used. This library is
included as an example and is not updated regularly by Kvaser. If you suspect 
that the import library is not up-to-date (i.e. missing functionality), you need 
to re-build it:

1. Create a canlib32.def file from the exports in canlib32.lib:
dumpbin.exe /exports canlib32.lib > canlib32.def

2. Edit the new canlib32.def so that it looks like this:
LIBRARY canlib32.dll

EXPORTS
  DllGetVersion@4
  DllMain@12
  canAccept@12
  canBusOff@4
  canBusOn@4
  canClose@4
  ...

3. Use dlltool.exe to create the import library:
\mingw32\bin\dlltool.exe --input-def canlib32.def --dllname canlib32.dll --output-lib canlib32.a -k

4. Replace the old canlib32.a with the new canlib32.a

5. Rebuild the project

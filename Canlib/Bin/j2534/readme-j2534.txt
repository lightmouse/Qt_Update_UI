The following files are used to support J2534 applications. They are
distributed with the CANLIB SDK and are found in the 
(path to canlib sdk)\bin\j2534 directory.

They are currently not installed on the machine when the driver
installation package is run; they are only distrubuted with the SDK.


Basically, you run j2534_RegUpdate.exe once to setup some registry
entries, and then you run J2534Options.exe to select which CAN channel
to use, or to enable/disable logging. When these two programs have
been run, you can run your J2534 application and it will find all the
required DLLs by itself.



j2534_RegUpdate.exe
===========================================================================

This program registers the J2534 DLLs in Windows' registry.
It's a console mode program and takes the following options:

  j2534_RegUpdate [flags] [directory]
  -v         Be verbose.
  -cwd       Use current directory.
  -samedir   Use same directory as this file.
  -L         Enable logging.
  -noattend  Do not prompt for user input of any kind.
  directory  The directory where the DLLs reside.

For example, asssuming the three J2534 DLLs are in the same directory
as the j2534_RegUpdate.exe program, you can run:

j2534_RegUpdate -samedir



J2534Options.exe
===========================================================================

GUI mode program to set various options. The most important option is
probably the number of the CAN channel to use. The default value is 0.



kvj2534.dll
kvj2534c.dll
kvj2534i.dll
===========================================================================

These DLLs contains the actual J2534 support code. A J2534 application
will find the DLLs by using the registration information in the
registry, and the API is thoroughly documented in the J2534-1 standard
available from SAE.


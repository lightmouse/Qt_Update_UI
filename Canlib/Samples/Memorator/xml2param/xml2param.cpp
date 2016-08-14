/*
**                         Copyright 2009 by KVASER AB            
**                          WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description: Convert an XML file to a binary param.lif file. Use the debug
**              mode to get extra information about errors.
** ---------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <io.h>
#include <time.h>
#include <direct.h>
#include "debug.h"
#include "std.h"
#include "KvaMemoLibXML.h"

int main(int argc, char **argv)
{
  KvaXmlStatus err, status = KvaXmlStatusOK;
  int opt_debug = 0;
  int file_no   = 0, inv = 0;
  char *infile  = NULL;
  char *outfile = NULL;
  char buffer[XML_ERROR_MESSAGE_LENGTH];
  int i = 0;
  
  // Parse arguments
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i], "-d") == 0) {
      opt_debug = 1;
      continue;
    } 
    if (file_no == 0) {
      infile = argv[i];
      file_no++;
      continue;
    }
    if (file_no == 1) {
      outfile = argv[i];
      file_no++;
      continue;
    }
    if (strcmp(argv[i], "-inv") == 0) {
      inv = 1;
      continue;
    } 
  }
  
  if (file_no != 2) {
    printf("Usage: xml2param [-d] xmlfile paramfile [-inv]");
    return(1);
  }

  // Initialize
  kvaXmlInitialize();
  
  // Set debug mode
  kvaXmlDebugOutput(opt_debug);
  
  // Convert
  if (inv) {
    status = kvaFileToXml(outfile, infile);
   } else {
    status = kvaXmlToFile(infile, outfile);
   }
  if (status) {
    kvaXmlGetLastError(buffer, sizeof(buffer), &err);
    printf("Conversion failed:\n  %s\n", buffer);
  }
  
  return status;
}
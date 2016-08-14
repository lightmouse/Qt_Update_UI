#include <stdio.h>
#include "canlib.h"



//Checks if the status is an error code and displays its error message
void Check(char* id, canStatus stat)
{
    char buf[50];
    if (stat != canOK) {
        buf[0] = '\0';
        canGetErrorText(stat, buf, sizeof(buf));
        printf("%s: failed, stat=%d (%s)\n", id, (int)stat, buf);
    }
}

void main(int argc, char* argv[]){
  //Holds a handle to the CAN channel
  int handle;
  //Status returned by the Canlib calls
  canStatus stat;

  int Bitrate = canBITRATE_250K;
  char msg[8] = {1,2,3,4,5,6,7,8};

  canInitializeLibrary();
  printf("Opening channel 0\n");
  handle = canOpenChannel(0, 0);
  if(handle < 0){
    Check("canOpenChannel", handle);
    exit(1);
  }
  
  printf("Setting bitrate and going bus on\n");
  stat = canSetBusParams(handle, Bitrate, 0, 0, 0, 0, 0);
  Check("canSetBusParams", stat);
  stat = canBusOn(handle);

  printf("Writing a message to the channel and waiting for it to be sent \n");
  stat = canWrite(handle, 123, msg, 8, 0);
  Check("canWrite", stat);
  stat = canWriteSync(handle, 100);
  Check("canWriteSync", stat);
  
  printf("Going off bus and closing channel");
  stat = canBusOff(handle);
  Check("canBusOff", stat);
  stat = canClose(handle);
  Check("canClose", stat);

}

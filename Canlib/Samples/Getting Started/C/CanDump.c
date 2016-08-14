#include "canlib.h"
#include <stdio.h>
#include <conio.h>


//Prints an error message if a Canlib call fails
void Check(char* id, canStatus stat){
	char buf[50];
	if (stat != canOK) {
		buf[0] = '\0';
		canGetErrorText(stat, buf, sizeof(buf));
		printf("%s: failed, stat=%d (%s)\n", id, (int)stat, buf);
		exit(1);
	}
}


void dumpMessageLoop(int handle){
	int status = canOK;
	long id;
	unsigned int dlc, flags;
	unsigned char data[8];
	DWORD time;

	printf("Listening for messages on channel 0, press any key to close\n");

	//Loops until a key is pressed
	while (!kbhit()){

		//Waits up to 50 ms for a message
		status = canReadWait(handle, &id, data, &dlc, &flags, time, 100);
		if (status == canOK){
			if (flags & canMSG_ERROR_FRAME){
				printf("***ERROR FRAME RECEIVED***");
			}
			else {
				printf("Id: %d, Data: %d %d %d %d %d %d %d %d DLC: %d Flags: %d",
					id, dlc, data[0], data[1], data[2], data[3], data[4],
					data[5], data[6], data[7], time);
			}
		}
                //Breaks the loop if something goes wrong
		else if (status != canERR_NOMSG){
			Check("canRead", status);
			break;
		}
	}
}


void main(int argc, int* argv[]){
	int handle;
	int status;

	canInitializeLibrary();

        //Channel initialization
	handle = canOpenChannel(0, 0);
	status = canSetBusParams(handle, canBITRATE_250K, 0, 0, 0, 0, 0);
	Check("canSetBusParams", status);

	status = canBusOn(handle);
	Check("canBusOn", status);

        //Starts listening for messages
	dumpMessageLoop(handle);

        //Channel teardown
	printf("Going of bus and closing channel");
	status = canBusOff(handle);
	Check("CanBusOff", status);
	status = canClose(handle);
	Check("CanClose", status);
}

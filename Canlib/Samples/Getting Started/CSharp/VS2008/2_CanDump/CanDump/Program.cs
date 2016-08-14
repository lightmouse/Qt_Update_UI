using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using canlibCLSNET;

namespace CanDump
{
    class Program
    {
        static void Main(string[] args)
        {
            int handle;
            Canlib.canStatus status;


            //Initialize, open channel and go on bus
            Canlib.canInitializeLibrary();

            handle = Canlib.canOpenChannel(0, Canlib.canOPEN_ACCEPT_VIRTUAL);
            DisplayError((Canlib.canStatus)handle, "canSetBusParams");

            status = Canlib.canSetBusParams(handle, Canlib.canBITRATE_250K, 0, 0, 0, 0, 0);
            DisplayError(status, "canSetBusParams");

            status = Canlib.canBusOn(handle);
            DisplayError(status, "canBusOn");

            //Start dumping messages
            DumpMessageLoop(handle);

            //Go off bus and close channel
            status = Canlib.canBusOff(handle);
            DisplayError(status, "canBusOff");

            status = Canlib.canClose(handle);
            DisplayError(status, "canClose");


            //Wait for the user to press a key before exiting, in case the console closes automatically on exit.
            Console.WriteLine("Channel closed. Press any key to exit");
            Console.ReadKey();
        }


        private static void DumpMessageLoop(int handle)
        {
            Canlib.canStatus status;
            bool finished = false;

            //These variables hold the incoming message
            byte[] data = new byte[8];
            int id;
            int dlc;
            int flags;
            long time;

            Console.WriteLine("Channel opened. Press Escape to close. ");
            Console.WriteLine("   ID    DLC DATA                      Timestamp");

            while (!finished)
            {
                //Wait for 100 ms for a message on the channel
                status = Canlib.canReadWait(handle, out id, data, out dlc, out flags, out time, 100);

                //Loop until all messages from the past 100 ms have been displayed, or an error occurs
                while (status == Canlib.canStatus.canOK)
                {
                    if ((flags & Canlib.canMSG_ERROR_FRAME) != 0)
                    {
                        Console.Write("Error Frame received ****");
                    }
                    else
                    {
                        DumpMessage(id, data, dlc, flags, time);
                    }
                    status = Canlib.canRead(handle, out id, data, out dlc, out flags, out time);
                }

                //Call DisplayError and exit in case an actual error occurs
                if (status != Canlib.canStatus.canERR_NOMSG)
                {
                    DisplayError(status, "canRead/canReadWait");
                    finished = true;
                }

                //Breaks the loop if the user presses the Escape key
                if(Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                {
                    finished = true;
                }
            }
        }


        //Prints an incoming message to the screen
        private static void DumpMessage(int id, byte[] data, int dlc, int flags, long time)
        {
            Console.WriteLine(data.Length);
            Console.WriteLine("ID: {0} Length: {1} Data: {2:x2} {3:x2} {4:x2} {5:x2} {6:x2} {7:x2} {8:x2} {9:x2} \n Time: {10}  Flags: {11}",
                                             id, dlc, data[0], data[1], data[2], data[3], data[4],
                                             data[5], data[6], data[7], time, flags);
        }

        //This method prints an error if something goes wrong
        private static void DisplayError(Canlib.canStatus status, string method)
        {
            if (status < 0)
            {
                Console.WriteLine(method + " failed: " + status.ToString());
            }
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using canlibCLSNET;

namespace HelloWorld
{
    class Program
    {
        static void Main(string[] args)
        {
            int handle;
            byte[] message = {0, 1, 2, 3, 4, 5, 6, 7};
            Canlib.canStatus status;

            //Initializes library so we can call Canlib
            Console.WriteLine("Initializing Canlib");
            Canlib.canInitializeLibrary();

            //Gets a handle to channel 0
            Console.WriteLine("Opening channel 0");
            handle = Canlib.canOpenChannel(0, Canlib.canOPEN_ACCEPT_VIRTUAL);
            CheckStatus((Canlib.canStatus)handle, "canSetBusParams");

            //Sets the bitrate for the bus to 250 kb/s
            Console.WriteLine("Setting channel bitrate");
            status = Canlib.canSetBusParams(handle, Canlib.canBITRATE_250K, 0, 0, 0, 0, 0);
            CheckStatus(status, "canSetBusParams");

            //Takes the channel on bus
            Console.WriteLine("Going on bus");
            status = Canlib.canBusOn(handle);
            CheckStatus(status, "canBusOn");

            //Send a message to the channel
            Console.WriteLine("Writing a message to the channel");
            status = Canlib.canWrite(handle, 123, message, 8, 0);
            CheckStatus(status, "canWrite");

            //Wait until the message is sent
            Console.WriteLine("Waiting for the message to be transmitted");
            status = Canlib.canWriteSync(handle, 1000);
            CheckStatus(status, "canWriteSync");

            //Takes the channel off bus
            Console.WriteLine("Going off bus");
            status = Canlib.canBusOff(handle);
            CheckStatus(status, "canBusOff");

            //Closes the channel
            Console.WriteLine("Closing channel 0");
            status = Canlib.canClose(handle);
            CheckStatus(status, "canClose");

            //Wait for the user to press a key before exiting, in case the console closes automatically on exit.
            Console.WriteLine("Press any key to exit");
            Console.ReadKey();
        }


        //This method prints an error if something goes wrong
        private static void CheckStatus(Canlib.canStatus status, string method)
        {
            if (status < 0)
            {
                string errorText;
                Canlib.canGetErrorText(status, out errorText);
                Console.WriteLine(method + " failed: " + errorText);
            }
        }
    }
}

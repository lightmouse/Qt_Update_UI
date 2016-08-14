﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using canlibCLSNET;
using System.Threading;

namespace _4_Serialization
{
    class Program
    {
        static void Main(string[] args)
        {
            //A channel to the device
            int channel = 3;
            //The slot on the device to which we want to load the script
            int slot = 0;
            //The name of the file we write incoming messages to
            string outputfile = "output.txt";
            //The name of the file where we have stored the messages to send
            string inputfile = "input.txt";
            //The compiled t script file
            string scriptfile = "serialization.txe";

            //How long we should wait for program output in the loop
            int timeout = 100;
            int wait = 10000;

            Canlib.canInitializeLibrary();

            int handle = Canlib.canOpenChannel(channel, 0);

            //If there is a file with the same name as our output file on the device, delete it
            CheckStatus(Canlib.kvFileDelete(handle, outputfile), "removing output file");

            CheckStatus(Canlib.kvFileCopyToDevice(handle, inputfile, inputfile), "uploading input file");

            CheckStatus(Canlib.kvScriptLoadFile(handle, slot, ref scriptfile), "loading script file");

            CheckStatus(Canlib.kvScriptRequestText(handle, slot, Canlib.kvSCRIPT_REQUEST_TEXT_SUBSCRIBE), "Requesting text");

            CheckStatus(Canlib.kvScriptStart(handle, slot), "starting script");

            Console.WriteLine("Script started");
            for (int i = 0; i < wait; i += timeout)
            {
                int s, f;
                ulong time;
                string output;
                Canlib.canStatus status = Canlib.kvScriptGetText(handle, out s, out time, out f, out output);
                while (status == Canlib.canStatus.canOK)
                {
                    Console.WriteLine("Output: " + output);
                    status = Canlib.kvScriptGetText(handle, out s, out time, out f, out output);
                }
                if (status != Canlib.canStatus.canERR_NOMSG)
                {
                    CheckStatus(status, "Reading text");
                }
                Thread.Sleep(timeout);
            }

            CheckStatus(Canlib.kvScriptStop(handle, slot, Canlib.kvSCRIPT_STOP_NORMAL), "Stopping script");

            CheckStatus(Canlib.kvFileCopyFromDevice(handle, outputfile, outputfile), "Fetching output file");

            CheckStatus(Canlib.kvScriptUnload(handle, slot), "unload script");

            CheckStatus(Canlib.canClose(handle), "closing channel");

            Canlib.canUnloadLibrary();

            Console.WriteLine("Script stopped");

            string outputText = System.IO.File.ReadAllText(outputfile);

            Console.WriteLine("Fetched output file from device:\n {0}", outputText);
            Console.WriteLine("Press enter to exit");
            Console.ReadLine();
        }

        private static void CheckStatus(Canlib.canStatus status, string method)
        {
            if (status < 0)
            {
                Console.WriteLine(method + " failed: " + status.ToString());
            }
        }
    }
}

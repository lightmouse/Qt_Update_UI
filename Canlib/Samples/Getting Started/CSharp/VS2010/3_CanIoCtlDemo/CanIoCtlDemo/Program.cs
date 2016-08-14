using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using canlibCLSNET;

namespace CanIoCtlDemo
{
    class Program
    {
        static void Main(string[] args)
        {
            /*
             * This Main method is intended to demonstrate how the various methods and functions in this program works.
             * Feel free to modify it to suit your needs
             */

            Console.WriteLine("This is a demo of CanIoLibCtl\n");

            

            //Setting up four handles: Two on channel 0, one on channel 1 (intended to be on the same device as channel 0)
            //and one on channel 2 (intended to be a remote device).
            int hnd0, hnd1, hnd0_2, hnd2;
            Canlib.canStatus status;
            byte[] msg1 = new byte[8] { 1, 2, 3, 4, 5, 6, 7, 8 };

            Canlib.canInitializeLibrary();

            hnd0 = Canlib.canOpenChannel(0, 0);
            hnd1 = Canlib.canOpenChannel(1, 0);
            hnd2 = Canlib.canOpenChannel(2, 0);
            hnd0_2 = Canlib.canOpenChannel(0, 0);

            status = Canlib.canSetBusParams(hnd0, Canlib.canBITRATE_250K, 0, 0, 0, 0, 0);
            CheckStatus(status, "canSetBusParams");
            status = Canlib.canSetBusParams(hnd1, Canlib.canBITRATE_250K, 0, 0, 0, 0, 0);
            CheckStatus(status, "canSetBusParams");
            status = Canlib.canSetBusParams(hnd0_2, Canlib.canBITRATE_250K, 0, 0, 0, 0, 0);
            CheckStatus(status, "canSetBusParams"); 
            status = Canlib.canSetBusParams(hnd2, Canlib.canBITRATE_250K, 0, 0, 0, 0, 0);
            CheckStatus(status, "canSetBusParams");

            status = Canlib.canBusOn(hnd0);
            status = Canlib.canBusOn(hnd1);
            status = Canlib.canBusOn(hnd2);



            //Demonstrating PreferStd and PreferExt:
            //Starting with a regular message with an extended id but without the ext flag
            Console.WriteLine("\nSending a message with id 10000, ext flag off");
            SendAndReceive(hnd0, hnd1);

            //Setting extended flag to default
            Console.WriteLine("Setting ext flag to default");
            PreferExt(hnd0);
            SendAndReceive(hnd0, hnd1);

            //Setting standard flag to default
            Console.WriteLine("Setting std flag to default");
            PreferStd(hnd0);
            SendAndReceive(hnd0, hnd1);
            


            //Creating an error
            Console.WriteLine("\nTrying to create a transmit error by sending to a closed bus");
            Canlib.canBusOff(hnd1);
            SendMsg(hnd0);

            //Reading the number of errors
            int txerrors, rxerrors, overrors;
            Canlib.canReadErrorCounters(hnd0, out txerrors, out  rxerrors, out overrors);
            Console.WriteLine("Number of transmit errors: {0}", txerrors);

            //Clearing the error counter (not supported on all devices)
            Console.WriteLine("Clearing error counter");
            System.Threading.Thread.Sleep(20);
            ClearErrorCounter(hnd0);
            System.Threading.Thread.Sleep(20);
            Canlib.canReadErrorCounters(hnd0, out txerrors, out  rxerrors, out overrors);
            Console.WriteLine("Number of transmit errors: {0}", txerrors);
            //Put handle 1 back on bus<li></li>
            Canlib.canBusOn(hnd1);




            //Changing the timer scale
            Console.WriteLine("Current timer scale: {0}", GetTimerScale(hnd1));
            Console.WriteLine("\nSetting timer scale to 100ms");
            SetTimerScale(hnd1, 100000);
            Console.WriteLine("Sending 5 messages 50ms apart, notice the timestamp.");
            for (int i = 0; i < 5; i++)
            {
                SendAndReceive(hnd0, hnd1);
                System.Threading.Thread.Sleep(50);
            }
            //Setting the timer scale back to 1 ms
            SetTimerScale(hnd1, 1000);




            //Disabling timer reset on Bus On
            Console.WriteLine("\nCurrent time on channel 0: {0}", Canlib.canReadTimer(hnd0));
            Console.WriteLine("Disabling timer reset, going bus off and on");
            SetClockResetAtBusOn(hnd0, false);
            Canlib.canBusOff(hnd0);
            Canlib.canBusOn(hnd0);
            Console.WriteLine("Current time on channel 0: {0}", Canlib.canReadTimer(hnd0));
            Console.WriteLine("Enabling timer reset, going bus off and on");
            SetClockResetAtBusOn(hnd0, true);
            Canlib.canBusOff(hnd0);
            Canlib.canBusOn(hnd0);
            Console.WriteLine("Current time on channel 0: {0}", Canlib.canReadTimer(hnd0));




            //Testing transmit ACKs
            Console.WriteLine("\nCurrent TXACK level for channel 0 (0: off, 1: on): {0}", GetTXACK(hnd0));
            Console.WriteLine("Turning on TXACK for channel 0");
            SetTXACK(hnd0, 1);
            Console.WriteLine("Sending message from 0 to 1");
            SendAndReceive(hnd0, hnd1);
            //This next message is a transmit acknowledgment
            Console.WriteLine("Reading the TXACK");
            ReadMsg(hnd0);
            //Disabling transmit ACKs again
            SetTXACK(hnd0, 0);




            //Showing the read queue size and how to flush it
            Console.WriteLine("\nSending a message from channel 0 to channel 1");
            SendMsg(hnd0);
            Console.WriteLine("RX Queue level on channel 1: {0}", GetRXQueueLevel(hnd1));
            Console.WriteLine("Flushing the RX buffer");
            FlushRXBuffer(hnd1);
            Console.WriteLine("RX Queue level on channel 1: {0}", GetRXQueueLevel(hnd1));
            //Setting a limit on the RX buffer
            Console.WriteLine("Setting a limit at 5 messages on the receive buffer on channel 1");
            SetRXQueueSize(hnd1, 5);
            Console.WriteLine("Sending 10 messages to channel 1");
            //Trying to send 10 messages to channel 1
            for (int i = 0; i < 10; i++)
            {
                SendMsg(hnd0);
            }
            Console.WriteLine("RX Queue level on channel 1: {0}", GetRXQueueLevel(hnd1));
            Console.WriteLine("Flushing the RX buffer");
            FlushRXBuffer(hnd1);
            Console.WriteLine("RX Queue level on channel 1: {0}", GetRXQueueLevel(hnd1));
            SetRXQueueSize(hnd1, 10);




            //Testing Transmit Requests
            Console.WriteLine("\nTurning on transmit requests for channel 0");
            SetTransmitRequest(hnd0, true);
            Console.WriteLine("Sending message from channel 0 to 1, notice the second message");
            SendMsg(hnd0);
            ReadMsg(hnd1);
            ReadMsg(hnd0);
            SetTransmitRequest(hnd0, false);



            //Getting user IO port data (does not work on all devices, uncomment if your device supports this)

            //Canlib.canUserIoPortData user = GetUserIoPortData(hnd2);
            //Console.WriteLine("Port nr: {0}, Port value: {1}", user.portNo, user.portValue);
            //SetUserIoPortData(hnd0, user);



            
            //Trying out TX echo
            Console.WriteLine("\nTurning on transmit echo");
            status = Canlib.canBusOn(hnd0_2);
            Console.WriteLine("Sending a message from handle 0, notice that the second handle receives it");
            SendMsg(hnd0);
            ReadMsg(hnd0_2);
            SetLocalTXEcho(hnd0, false);
            SendMsg(hnd0);
            ReadMsg(hnd0_2);
            SetLocalTXEcho(hnd0, true);
            FlushRXBuffer(hnd1);
            Canlib.canBusOff(hnd0_2);



            //Testing Transmit Intervals
            Console.WriteLine("\nCurrent transmit interval: {0}", GetTXInterval(hnd0));
            Console.WriteLine("Setting transmit interval to 10 ms");
            SetTXInterval(hnd0, 10000);
            Console.WriteLine("Sending five messages and receiving them, notice the timestamps.");
            for (int i = 0; i < 5; i++)
            {
                status = Canlib.canWrite(hnd0, i, new byte[1] { 0 }, 1, 0);
            }
            status = Canlib.canWriteSync(hnd0, 700);
            for (int i = 0; i < 5; i++)
            {
                ReadMsg(hnd1);
            }




            //Disabling error frame reporting
            Console.WriteLine("\nDisabling error frame reporting for channel 1");
            SetErrorFramesReporting(hnd1, false);
            Console.WriteLine("Sending an error frame to channel 1");
            status = Canlib.canWriteWait(hnd0, 123, new byte[3]{1,2,3},3,Canlib.canMSG_ERROR_FRAME,100);
            Console.WriteLine("Trying to read the error frame");
            ReadMsg(hnd1);
            SetErrorFramesReporting(hnd1, true);
            



            //Testing channel quality
            Console.WriteLine("\nChannel 2 quality is {0}%", GetChannelQuality(hnd2));

            //Reads the round trip time
            Console.WriteLine("RTT for device on channel 2: {0} ", GetRoundTripTime(hnd2));

            //Gets the devname for a remote device
            Console.WriteLine("ASCII devname for channel 2: {0}", GetDevNameASCII(hnd2));

            //Outputs the bus type
            String busType = BusTypeToString(GetBusType(hnd2));
            Console.WriteLine("Bus type for channel 0: {0}", busType);

            //Checking when remote device was last seen
            Console.WriteLine("Device on channel 2 was last seen {0} ms ago", GetTimeSinceLastSeen(hnd2));




            //Display the throttle value
            Console.WriteLine("\nThrottle value for device on channel 0: {0}", GetThrottleScaled(hnd0));

            //Trying to set throttle (not supported on all devices)
            Console.WriteLine("Trying to set throttle value to 40");
            SetThrottleScaled(hnd0, 40);

            //Display the new throttle value
            Console.WriteLine("New throttle value: {0}", GetThrottleScaled(hnd0));

            
            //Exit
            Console.WriteLine("\n\nPress any key to exit");
            Console.ReadKey();

        }

        /*
         * CanIoCtl function library 
         */

        //Sets the extended ID flag by default
        private static void PreferExt(int handle)
        {
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_PREFER_EXT, 0);
            CheckForException(status);
        }

        //Sets the standard ID flag by default
        private static void PreferStd(int handle)
        {
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_PREFER_STD, 0);
        }

        //Clears the error counter (not supported on all devices)
        private static void ClearErrorCounter(int handle)
        {
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_CLEAR_ERROR_COUNTERS, 0);
            CheckForException(status);
        }

        //Sets the timer scale (value in microseconds, default 1000)
        private static void SetTimerScale(int handle, int scale)
        {
            if (scale < 0)
            {
                throw new ArgumentException("Cannot set timer scale to negative number");
            }
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_SET_TIMER_SCALE, scale);
            CheckForException(status);
        }

        //Gets the timer scale
        private static int GetTimerScale(int handle)
        {
            int scale;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_GET_TIMER_SCALE, out scale);
            CheckForException(status);
            return scale;
        }

        //Set TXACK: 0 = Off, 1 = On, 2 = Off even for internal use
        private static void SetTXACK(int handle, int level)
        {
            if (level < 0 || level > 2)
            {
                throw new ArgumentException("TXACK level must be 0, 1 or 2");
            }
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_SET_TXACK, level);
            CheckForException(status);
        }

        //Returns 1 if TXACK is enabled on the channel, 0 if it's disabled and 2 if it's disabled internally
        private static int GetTXACK(int handle)
        {
            int buf = -1;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_GET_TXACK, buf);
            CheckForException(status);
            return buf;
        }
        
        //Returns the number of messages in the channel's reading buffer
        private static int GetRXQueueLevel(int handle)
        {
            int buf;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_GET_RX_BUFFER_LEVEL, out buf);
            CheckForException(status);
            return buf;
        }

        //Returns the number of messages in the channel's transmit buffer
        private static int GetTXQueueLevel(int handle)
        {
            int buf;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_GET_TX_BUFFER_LEVEL, out buf);
            CheckForException(status);
            return buf;
        }

        //Empty the reading buffer
        private static void FlushRXBuffer(int handle)
        {
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_FLUSH_RX_BUFFER, 0);
        }

        //Empty the transmit buffer
        private static void FlushTXBuffer(int handle)
        {
            Canlib.canIoCtl(handle, Canlib.canIOCTL_FLUSH_TX_BUFFER, 0);
        }

        
        //Turning transmit requests on or off (off by default)
        private static void SetTransmitRequest(int handle, bool on)
        {
            int buf = on ? 1 : 0;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_SET_TXRQ, buf);
            CheckForException(status);
        }

        //Returns an event handle. For more information, see the separate sample about event handles.
        private static IntPtr GetEventHandle(int handle)
        {
            Object eventHandle = new IntPtr(-1);
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_GET_EVENTHANDLE, ref eventHandle);
            CheckForException(status);
            return (IntPtr)eventHandle;
        }
        
        //Returns the user io port data (port number and value)
        private static Canlib.canUserIoPortData GetUserIoPortData(int handle)
        {
            Object buf = new Canlib.canUserIoPortData();
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_GET_USER_IOPORT, ref buf);
            CheckForException(status);
            return (Canlib.canUserIoPortData)buf;
        }

        //Sets the user io port data (port number and value)
        private static void SetUserIoPortData(int handle, Canlib.canUserIoPortData user)
        {
            Object o = user;
            if (user == null)
            {
                throw new ArgumentNullException("Cannot set user io port data to null");
            }
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_SET_USER_IOPORT, ref o);
            CheckForException(status);
        }

        //Changes the limit on the receive buffer queue size.
        //This cannot be done while the channel is on bus.
        private static void SetRXQueueSize(int handle, int size)
        {
            if (size < 0)
            {
                throw new ArgumentException("Cannot set RX Queue size to a negative number");
            }
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_SET_RX_QUEUE_SIZE, size);
            CheckForException(status);
        }

        //Decides if the clock should reset when the channel goes on bus. Default: on.
        private static void SetClockResetAtBusOn(int handle, bool reset)
        {
            int buf = reset ? 1 : 0;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_SET_BUSON_TIME_AUTO_RESET, buf);
            CheckForException(status);
        }
        

        //Turn on or off local TX echo (default: on)
        //If two handles have the same channel and one of them sends a message, 
        //the other will receive it if local TX echo is enabled.
        private static void SetLocalTXEcho(int handle, bool echo)
        {
            int buf = echo ? 1 : 0;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_SET_LOCAL_TXECHO, buf);
            CheckForException(status);
        }


        //Turn error frame reporting on or off (default: on)
        //If this is disabled, the channel will ignore error frames
        private static void SetErrorFramesReporting(int handle, bool report)
        {
            int buf = report ? 1 : 0;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_SET_ERROR_FRAMES_REPORTING, buf);
            CheckForException(status);
        }

        //Returns the channel quality (between 0 and 100% of optimal quality)
        private static int GetChannelQuality(int handle) 
        {
            int i;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_GET_CHANNEL_QUALITY, out i);
            CheckForException(status);
            return i;
        }


        //Returns the round trip time to the device
        private static int GetRoundTripTime(int handle)
        {
            int rtt;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_GET_ROUNDTRIP_TIME, out rtt);
            CheckForException(status);
            return rtt;
        }

        //Returns the bus type (use the BusTypeToString function to get it as a string)
        private static int GetBusType(int handle)
        {
            int bus;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_GET_BUS_TYPE, out bus);
            CheckForException(status);
            return bus;
        }

        //Get the Devname for the device (remote devices only)
        private static string GetDevNameASCII(int handle)
        {
            string name;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_GET_DEVNAME_ASCII, out name);
            CheckForException(status);
            return name;
        }

        //Returns the time in ms since last communication with the device
        private static int GetTimeSinceLastSeen(int handle)
        {
            int time;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_GET_TIME_SINCE_LAST_SEEN, out time);
            CheckForException(status);
            return time;
        }

        //Returns the minimum transmission interval of the channel in microseconds (default: 0)
        private static int GetTXInterval(int handle)
        {
            int interval = -1;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_TX_INTERVAL, out interval);
            CheckForException(status);
            return interval;
        }

        //Sets the minimum transmission interval of the channel (in microseconds, cannot be set to higher than one second)
        private static void SetTXInterval(int handle, int interval)
        {
            if (interval > 1000000)
            {
                throw new ArgumentException("Too high TX Interval");
            }
            else if (interval < 0)
            {
                throw new ArgumentException("Negative TX Interval");
            }
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_TX_INTERVAL, interval);
            CheckForException(status);
        }

        //Set the throttle value (0-100, not supported on all devices)
        private static void SetThrottleScaled(int handle, int throttle)
        {
            if (throttle > 100 || throttle < 0)
            {
                throw new ArgumentException("Throttle value must be between 0 and 100");
            }
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_SET_USB_THROTTLE_SCALED, throttle);
            CheckForException(status);
        }

        //Get the throttle value
        private static int GetThrottleScaled(int handle)
        {
            int throttle;
            Canlib.canStatus status = Canlib.canIoCtl(handle, Canlib.canIOCTL_GET_USB_THROTTLE_SCALED, out throttle);
            CheckForException(status);
            return throttle;
        }

        


        /*
         * 
         * Helper functions
         * 
         */

        //Displays an error message if status is not ok
        private static void CheckStatus(Canlib.canStatus status, string method)
        {
            if (status < 0)
            {
                String errorText;
                Canlib.canGetErrorText(status, out errorText);
                Console.WriteLine(method + " failed: " + errorText);
            }
        }

        //Prints a message to the screen
        private static void DumpMessage(int hnd, int id, byte[] data, int dlc, int flags, long time)
        {
            Console.WriteLine("ID: {0} Length: {1} Data: {2:x2} {3:x2} {4:x2} {5:x2} {6:x2} {7:x2} {8:x2} {9:x2} \n Time: {10}  Flags: {11}, Hnd: {12}",
                                             id, dlc, data[0], data[1], data[2], data[3], data[4],
                                             data[5], data[6], data[7], time, flags, hnd);
        }

        //Sends a message from one handle to the other and outputs it
        private static void SendAndReceive(int hnd0, int hnd1)
        {
            SendMsg(hnd0);
            ReadMsg(hnd1);
        }

        //Sends a message from one handle
        private static void SendMsg(int hnd)
        {
            Canlib.canStatus status;
            byte[] msg1 = new byte[8] { 1, 2, 3, 4, 5, 6, 7, 8 };
            status = Canlib.canWriteWait(hnd, 10000, msg1, 8, 0, 100);
            CheckStatus(status, "canWriteWait");
        }

        //Reads a message from the handle's channel and prints it
        private static void ReadMsg(int hnd)
        {
            Canlib.canStatus status;
            int id, length, flags;
            byte[] rmsg = new byte[8];
            long time;

            status = Canlib.canRead(hnd, out id, rmsg, out length, out flags, out time);
            CheckStatus(status, "canRead");
            if (status == 0)
            {
                DumpMessage(hnd, id, rmsg, length, flags, time);
            }
        }

        //Returns the string equivalent of a bus type
        private static string BusTypeToString(int type)
        {
            String s;
            switch (type)
            {
                case Canlib.kvBUSTYPE_GROUP_INTERNAL: 
                    s = "Internal";
                    break;
                case Canlib.kvBUSTYPE_GROUP_REMOTE:
                    s = "Remote";
                    break;
                case Canlib.kvBUSTYPE_GROUP_LOCAL:
                    s = "Local";
                    break;
                case Canlib.kvBUSTYPE_GROUP_VIRTUAL:
                    s = "Virtual";
                    break;
                default:
                    s = "Unknown";
                    break;
            }
            return s;
        }


        //Throws an exception if the status is not ok.
        private static void CheckForException(Canlib.canStatus status)
        {
            if (status != Canlib.canStatus.canOK)
            {
                string s;
                Canlib.canGetErrorText(status, out s);
                throw new CanIoCtlException(s);
            }
        }

    }

    //An exception class used to handle errors from CanIoCtl
    public class CanIoCtlException : Exception
    {
        public CanIoCtlException(String s) : 
            base(s)
        {
            
        }

        
    }
}

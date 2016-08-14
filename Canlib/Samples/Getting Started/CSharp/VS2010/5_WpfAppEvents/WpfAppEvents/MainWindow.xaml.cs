﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using canlibCLSNET;
using System.ComponentModel;
using System.Threading;
using WpfAppEvents;

namespace WpfApplication1
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 
    public partial class MainWindow : Window
    {
        private int handle = -1;
        int channel = -1;
        private int readHandle = -1;
        private bool onBus = false;
        private readonly BackgroundWorker dumper;
        WaitHandle waitHandle;


        public MainWindow()
        {
            InitializeComponent();
            dumper = new BackgroundWorker();
            dumper.DoWork += DumpMessageLoop;
            dumper.WorkerReportsProgress = true;
            dumper.ProgressChanged += new ProgressChangedEventHandler(ProcessMessage);
        }


        private void initButtonClick(object sender, RoutedEventArgs e)
        {
            Canlib.canInitializeLibrary();
            statusText.Text = "Canlib initialized";
        }


        private void openChannelButton_Click(object sender, RoutedEventArgs e)
        {
            channel = Convert.ToInt32(channelBox.Text);
            int hnd = Canlib.canOpenChannel(channel, Canlib.canOPEN_ACCEPT_VIRTUAL);

            CheckStatus("Open channel", (Canlib.canStatus)hnd);
            if (hnd >= 0)
            {
                handle = hnd;
            }
        }

        private void setBitrateButton_Click(object sender, RoutedEventArgs e)
        {
            int[] bitrates = new int[4] { Canlib.canBITRATE_125K, Canlib.canBITRATE_250K, 
                                            Canlib.canBITRATE_500K, Canlib.BAUD_1M};
            int bitrate = bitrates[bitrateBox.SelectedIndex];

            Canlib.canStatus status = Canlib.canSetBusParams(handle, bitrate, 0, 0, 0, 0, 0);

            CheckStatus("Setting bitrate to " + ((ComboBoxItem)bitrateBox.SelectedValue).Content, status);

        }

        private void busOnButton_Click(object sender, RoutedEventArgs e)
        {
            Canlib.canStatus status = Canlib.canBusOn(handle);
            CheckStatus("Bus on", status);
            if (status == Canlib.canStatus.canOK)
            {
                onBus = true;

                

                if (!dumper.IsBusy)
                {
                    dumper.RunWorkerAsync();
                }

            }
        }

        private void sendButton_Click(object sender, RoutedEventArgs e)
        {
            int id = Convert.ToInt32(idBox.Text);

            TextBox[] textBoxes = {dataBox0, dataBox1, dataBox2, dataBox3, dataBox4,
                                      dataBox5, dataBox6, dataBox7};
            byte[] data = new byte[8];
            for (int i = 0; i < 8; i++)
            {
                data[i] = textBoxes[i].Text == "" ? (byte)0 : Convert.ToByte(textBoxes[i].Text);
            }

            CheckBox[] boxes = {RTRBox, STDBox, EXTBox, WakeUpBox, NERRBox, errorBox, 
                                   TXACKBox, TXRQBox, delayBox, BRSBox, ESIBox};
            int flags = 0;
            foreach (CheckBox box in boxes)
            {
                if (box.IsChecked.Value)
                {
                    flags += Convert.ToInt32(box.Tag);
                }
            }

            int dlc = Convert.ToInt32(DLCBox.Text);

            string msg = String.Format("{0}  {1}  {2:x2} {3:x2} {4:x2} {5:x2} {6:x2} {7:x2} {8:x2} {9:x2}   to handle {10}",
                                             id, dlc, data[0], data[1], data[2], data[3], data[4],
                                             data[5], data[6], data[7], handle);
            Canlib.canStatus status = Canlib.canWrite(handle, id, data, dlc, flags);
            CheckStatus("Writing message " + msg, status);
        }


        private void busOffButton_Click(object sender, RoutedEventArgs e)
        {
            Canlib.canStatus status = Canlib.canBusOff(handle);
            CheckStatus("Bus off", status);
            onBus = false;
        }

        private void closeButton_Click(object sender, RoutedEventArgs e)
        {
            Canlib.canStatus status = Canlib.canClose(handle);
            CheckStatus("Closing channel", status);
            handle = -1;
            readHandle = -1;
        }

        /*
         * Looks for messages and sends them to the output box. 
         */
        private void DumpMessageLoop(object sender, DoWorkEventArgs e)
        {
            BackgroundWorker worker = sender as BackgroundWorker;
            Canlib.canStatus status = Canlib.canStatus.canERR_NOMSG;
            int id;
            byte[] data = new byte[8];
            int dlc;
            int flags;
            long time;
            bool noError = true;
            string msg;

            //Open a new handle for reading
            readHandle = Canlib.canOpenChannel(channel, Canlib.canOPEN_ACCEPT_VIRTUAL);
            Canlib.canBusOn(readHandle);
            
            //Create a Windows event handle
            Object winHandle = new IntPtr(-1);
            status = Canlib.canIoCtl(readHandle, Canlib.canIOCTL_GET_EVENTHANDLE, ref winHandle);
            if (status != Canlib.canStatus.canOK)
            {
                worker.ReportProgress(100, status);
                noError = false;
            }

            CanlibWaitEvent kvEvent = new CanlibWaitEvent(winHandle);
            waitHandle = kvEvent;

            while (noError && onBus && readHandle >= 0)
            {
                //Wait for 100 ms for an event to occur on the channel
                bool eventHappened = waitHandle.WaitOne(100);
                if (!eventHappened)
                {
                    continue;
                }
                
                status = Canlib.canRead(readHandle, out id, data, out dlc, out flags, out time);
                while (status == Canlib.canStatus.canOK) 
                {
                    if ((flags & Canlib.canMSG_ERROR_FRAME) == Canlib.canMSG_ERROR_FRAME)
                    {
                        msg = "***ERROR FRAME RECEIVED***";
                    }
                    else
                    {
                        msg = String.Format("{0:x8}  {1}  {2:x2} {3:x2} {4:x2} {5:x2} {6:x2} {7:x2} {8:x2} {9:x2}   {10}\r",
                                                    id, dlc, data[0], data[1], data[2], data[3], data[4],
                                                    data[5], data[6], data[7], time);
                    }

                    worker.ReportProgress(0, msg);
                    status = Canlib.canRead(readHandle, out id, data, out dlc, out flags, out time);
                } 
                if (status != Canlib.canStatus.canERR_NOMSG)
                {
                    worker.ReportProgress(100, status);
                    noError = false;
                }
            }
            Canlib.canBusOff(readHandle);
        }

        /*
         * Adds the messages to the output box
         */
        private void ProcessMessage(object sender, ProgressChangedEventArgs e)
        {
            if (e.ProgressPercentage == 0)
            {
                string output = (string)e.UserState;
                outputBox.AppendText(output);
                outputBox.ScrollToEnd();
            }
            else
            {
                CheckStatus("Reading", (Canlib.canStatus)e.UserState);
            }
        }

        /*
         * Makes sure that only positive integers can be used as input
         */
        private void CheckTextBox(object sender, TextCompositionEventArgs e)
        {
            e.Handled = !isInt(e.Text);
        }

        private bool isInt(String s)
        {
            foreach (char c in s.ToCharArray())
            {
                if (!char.IsDigit(c))
                {
                    return false;
                }
            }
            return true;
        }

        /*
         * Updates the status bar, prints error message if something goes wrong
         */
        private void CheckStatus(String action, Canlib.canStatus status)
        {
            if (status != Canlib.canStatus.canOK)
            {
                String errorText = "";
                Canlib.canGetErrorText(status, out errorText);
                statusText.Text = action + " failed: " + errorText;
            }
            else
            {
                statusText.Text = action + " succeeded";
            }
        }

    }
}

﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using Microsoft.Win32.SafeHandles;

namespace WpfAppEvents
{
    class CanlibWaitEvent: WaitHandle
  {
    /// <summary>
    /// 
    /// </summary>
    /// <param name="we"></param>
    public CanlibWaitEvent(object we)
    {
      SafeWaitHandle swHandle = new SafeWaitHandle(/*pointer*/ (IntPtr)we, true);
      base.SafeWaitHandle = swHandle;
    }

    ~CanlibWaitEvent()
    {
      base.SafeWaitHandle.SetHandleAsInvalid();
    }
  }
}

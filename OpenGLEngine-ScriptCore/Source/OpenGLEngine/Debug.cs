﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OpenGLEngine
{
    public class Debug
    {
        public static void Log(string message)
        {
            InternalCalls.Debug_Log(message);
        }

        public static string[] ListTest()
        {
            return InternalCalls.Debug_ListTest();
        }

        public static void sendArray(int[] array)
        {
            InternalCalls.Debug_SendArray(array);
        }
    }
}

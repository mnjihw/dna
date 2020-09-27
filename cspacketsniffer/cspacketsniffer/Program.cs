using SharpPcap;
using SharpPcap.LibPcap;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace cspacketsniffer
{
    class Program
    {
        
        static void Main(string[] args)
        {
            var obj = new Sniffer();
            while (true)
            {
                Thread.Sleep(100);

            }
        }
    }
}

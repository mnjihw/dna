using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using PdfSharp.Pdf;
using PdfSharp.Drawing;
using System.Security.Principal;
using System.Security;
using System.Collections;
using System.Collections.Concurrent;

namespace ConsoleApp1
{
    
       
    class Program
    {



        static  void Main(string[] args)
        {
            var q = new ConcurrentQueue<string>();
            var a = Task.Run(async () =>
            {
                while(true)
                {
                    q.Enqueue(DateTime.Now.ToString());
                    await Task.Delay(1000);
                }
            });
            var b = Task.Run(async () =>
            {
                while (true)
                {
                    if (q.TryDequeue(out var s))
                    {
                        Console.WriteLine(s);
                    }
                    await Task.Delay(100);
                    
                }

            });
            Console.ReadLine();

            return;
            var processStartInfo = new ProcessStartInfo
            {
                UseShellExecute = false,
                FileName = "notepad.exe",
                Domain = ".",
                UserName = "user1",
                WorkingDirectory = @"C:\Windows\System32"
            };

            var process = new Process { StartInfo = processStartInfo};
            var secure = new SecureString();

            foreach(var c in "zxc021")
                secure.AppendChar(c);
            
            process.StartInfo.Password = secure;
            process.StartInfo.UseShellExecute = false;
            process.Start();
            return;



        }
    }
}

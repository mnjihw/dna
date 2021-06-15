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
using OpenHardwareMonitor.Hardware;

namespace ConsoleApp1
{
    
       
    class Program
    {



        static  void Main(string[] args)
        {
            Computer computer = new Computer
            {
                CPUEnabled = true,
                GPUEnabled = true,
                MainboardEnabled = true,
                FanControllerEnabled = true,
                HDDEnabled = true,
                RAMEnabled = true,
            };
            computer.Open();

            foreach(var hardware in computer.Hardware)
            {
                if(hardware.HardwareType == HardwareType.CPU)
                {
                    hardware.Update();

                    foreach(var d in hardware.Sensors)
                    {
                        //Console.WriteLine($"{d.Name} {d.Max.Value} {d.Min.Value}");
                    }
                }
                else if(hardware.HardwareType == HardwareType.Mainboard)
                {
                    hardware.Update();
                    foreach (var d in hardware.SubHardware)
                    {
                        //Console.WriteLine($"{d.Name} {d.Max.Value} {d.Min.Value}");
                        d.Update();
                        foreach(var c in d.Sensors)
                        {
                            
                            Console.WriteLine($"{c.Name} {c.Max.Value} {c.Min.Value}");
                        }
                    }
                }
                /*foreach(var subhardware in hardware.SubHardware)
                {
                    subhardware.Update();
                    if (subhardware.Sensors.Length > 0)
                    {
                        foreach(var sensor in subhardware.Sensors)
                        {
                            if(sensor.SensorType == SensorType.Fan)
                            {
                                Console.WriteLine("DD");
                            }

                        }
                    }
                }*/
            }

            
            computer.Close();
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

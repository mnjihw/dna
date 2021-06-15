using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace minidump
{
    class Program
    {
        [DllImport("Dbghelp.dll")]
        static extern bool MiniDumpWriteDump(IntPtr hProcess, uint ProcessId, IntPtr hFile, int DumpType, ref MINIDUMP_EXCEPTION_INFORMATION ExceptionParam, IntPtr UserStreamParam, IntPtr CallbackParam);
        [DllImport("kernel32.dll")]
        static extern IntPtr GetCurrentProcess();
        [DllImport("kernel32.dll")]
        static extern uint GetCurrentProcessId();
        [DllImport("kernel32.dll")]
        static extern uint GetCurrentThreadId();

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        public struct MINIDUMP_EXCEPTION_INFORMATION
        {
            public uint ThreadId;
            public IntPtr ExceptionPointers;
            public int ClientPointers;
        }


        private static int MiniDumpWithFullMemory => 2;
        static async Task CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            string assemblyPath = Assembly.GetEntryAssembly().Location;
            string dumpFileName = assemblyPath + "_" + DateTime.Now.ToString("dd.MM.yyyy.HH.mm.ss") + ".dmp";
            await using FileStream fileStream = new FileStream(dumpFileName, FileMode.Create);
            MINIDUMP_EXCEPTION_INFORMATION info = new MINIDUMP_EXCEPTION_INFORMATION
            {
                ClientPointers = 0,
                ExceptionPointers = Marshal.GetExceptionPointers(),
                ThreadId = GetCurrentThreadId()
            };
            
            MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), fileStream.SafeFileHandle.DangerousGetHandle(), MiniDumpWithFullMemory, ref info, IntPtr.Zero, IntPtr.Zero);
            
            string exeName = Path.GetFileName(assemblyPath);

            Console.WriteLine("An Unhanled exception has been detected in the application " + exeName + " .\r\nException information is saved in " + dumpFileName);
        }

        static async Task Main()
        {
            await CurrentDomain_UnhandledException(null, null);
           /* Process targetProcess = Process.GetProcessById(processId);
            using FileStream stream = new FileStream(dumpFilePath, FileMode.Create);
            {
                Boolean res = MiniDumpWriteDump(
                    targetProcess.Handle,
                                    processId,
                                    stream.SafeFileHandle.DangerousGetHandle(),
                                    MiniDumpType.WithFullMemory,
                                    IntPtr.Zero,
                                    IntPtr.Zero,
                                    IntPtr.Zero);

                int dumpError = res ? 0 : Marshal.GetLastWin32Error();
                Console.WriteLine(dumpError);
            }*/
        }
    }
}

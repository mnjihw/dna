using System;
using System.Threading.Tasks;

namespace ConsoleApp4
{
    class Program
    {
        static async Task Main()
        {
            while(true)
            {
                var text = "wow;;;;;;;;;;;;;;;;;";
                Console.WriteLine(text);
                await Task.Delay(1000);
            }
        }
    }
}

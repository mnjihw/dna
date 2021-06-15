using System;
using Tesseract.Interop;
using Tesseract;
using System.Text.RegularExpressions;
using System.Drawing;
using System.Runtime.InteropServices;
using System.IO;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Globalization;
using System.Drawing.Imaging;
using System.Net.Http;
using System.Text;
using System.Net.Http.Headers;
using System.Text.Json;

namespace ocr_test
{
    class Program
    {
       
        static async Task Main()
        {
            var client = new HttpClient();

            while (true)
            {
                
                //ocr.SetVariable("tessedit_char_whitelist", "0123456789KM/HNE￡£.:- ");
                Console.WriteLine("엔터쳐라");
                Console.ReadLine();

                Match match;

                while (true)
                {
                    IntPtr hwnd = Win32.FindWindow("PotPlayer64", null);
                    if (hwnd == IntPtr.Zero)
                    {
                        Console.WriteLine("윈도우 못찾음");
                        await Task.Delay(2000);
                         
                    }
                    if (!Win32.GetWindowRect(new HandleRef(null, hwnd), out var rect))
                    {
                        Console.WriteLine("GetWindowRect failed!");
                        await Task.Delay(2000);
                        
                    }

                    var scale = new SizeF(1.5f, 1.5f);
                    var width = (int)((rect.Right - rect.Left) * scale.Width);
                    var height = (int)((rect.Bottom - rect.Top) / 12 * scale.Height);

                    using var bitmap = new Bitmap(width, height);
                    using var graphics = Graphics.FromImage(bitmap);
                    
                    using var ocr = new TesseractEngine("./tessdata", "eng", EngineMode.Default);
                    graphics.CopyFromScreen((int)(rect.Left * scale.Width), (int)((rect.Bottom - height) * scale.Height), 0, 0, new Size(width, height), CopyPixelOperation.SourceCopy);
                    await using var ms = new MemoryStream();
                    bitmap.Save(ms, System.Drawing.Imaging.ImageFormat.Bmp);

                    var text = ocr.Process(Pix.LoadFromMemory(ms.ToArray())).GetText();

                    Console.WriteLine(text);
                    match = Regex.Match(text, @"N(\d+.\d{6})\s{0,2}[E￡£]{0,3}(\d+.\d{6})");
                    if (!match.Success)
                    {
                        Console.WriteLine("못찾음!");
                        await Task.Delay(1000);
                    }
                    else
                        break;

                }
                var latitude = double.Parse(match.Groups[1].Value);
                var longitude = double.Parse(match.Groups[2].Value);


                var result = await client.GetStringAsync($"https://maps.googleapis.com/maps/api/geocode/json?language=ko&result_type=street_address&key=AIzaSyB0uTTY3zL488TiGJRNwxyzrOaMMuKoxCA&latlng={latitude},{longitude}");
                var address = JsonDocument.Parse(result).RootElement.GetProperty("results")[0].GetProperty("formatted_address").GetString();
                Console.WriteLine(address);
                
            }
        }
    }
}

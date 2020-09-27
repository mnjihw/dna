using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using HtmlAgilityPack;


namespace csutils
{
    class Program
    {
        static async Task Func()
        {
            HttpClient httpClient = new HttpClient();

            var result = await httpClient.GetStringAsync("http://bbs.ruliweb.com/nin/board/300004?page=1");
            HtmlDocument document = new HtmlDocument();
            document.LoadHtml(result);

            var nodes = document.DocumentNode.SelectNodes("//div[@class='relative']/a[@class='deco']");
            foreach (var node in nodes)
            {
                Console.WriteLine(node.InnerText);
            }
            // selectnodes 파고들기 컬렉션 반복이나 그런거
        }
        static void Main()
        {
            
            while (true)
            {
                Console.WriteLine(Util.ImageSearch(new Point(0, 0), new Point(1000, 1000), "a.bmp"));
                Thread.Sleep(400);
            }
            return;
            Func();
            Console.ReadLine();

        }
    }
}

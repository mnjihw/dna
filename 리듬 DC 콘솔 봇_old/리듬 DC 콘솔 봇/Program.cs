//#define 민석이
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Linq;
using System.Threading;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Collections.Immutable;
using System.Collections.ObjectModel;
using System.Buffers.Text;

namespace 리듬_DC_콘솔_봇
{

    class Program
    {
      
        static async Task Main(string[] args)
        {
            
            Console.Title = "리듬 DC 콘솔 봇";

            DCManager dc = DCManager.Instance;
            await dc.WriteComment("game_nintendo", "504671", "dd", "0210", "gd");
            return;
            var galleryNames = new Dictionary<string, string>()
            {
                ["프갤"] = "programming",
                ["야갤"] = "baseball_new8",
                ["닌갤"] = "game_nintendo",
                ["시갤"] = "watch",
                ["포갤"] = "pokemon",
                ["알갤"] = "coin",
            };
            
            (string nickname, string title, string content) result;
            string galleryName = galleryNames["프갤"];

            //dc.WriteComment(galleryNames["슻갤"], "1539936", "dd", "0210", "테스트");
            //Thread.Sleep(3000);

#if !민석이
            int i = 0;
            int number = await dc.GetLatestArticleNumber(galleryName);
            
            while (true)
            {

                if (++i == 40)
                {
                    i = 0;
                    number = await dc.GetLatestArticleNumber(galleryName);

                }
                try
                {
                    var abc = await dc.ParseArticle(galleryName, number--, false);
                    if (abc == default)
                        continue;
                    var comments = await dc.GetComments(galleryName, number + 1);


                    var a = OrthographyChecker.Check(abc.title);
                    if (!string.IsNullOrEmpty(a))
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine(a);
                        Console.ForegroundColor = ConsoleColor.White;
                    }
                    var b = OrthographyChecker.Check(abc.content);
                    if (!string.IsNullOrEmpty(b))
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine(b);
                        Console.ForegroundColor = ConsoleColor.White;
                    }
                    
                    Console.WriteLine($"title: {abc.title} content: {abc.content}");
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.Message);
                    Console.WriteLine(e.StackTrace);
                    return;
                }
                await Task.Delay(1500);
            }

            return;
#endif

#if 민석이
            bool isLatest = false;
            int number = await dc.GetArticleNumber(galleryName, isLatest);

            
            while (true)
            {
                result = await dc.ParseArticle(galleryName, number, false);

                if (result.title != null)
                {
                    Print();

                    if (!isLatest)
                        ++number;
                }
                else
                {
                    if (!isLatest)
                    {
                        if (number < await dc.GetArticleNumber(galleryName, true))
                        {
                            result = await dc.ParseArticle(galleryName, number, false);

                            if (result.title != null)
                            {
                                Print();

                            }
                            else //deleted
                                ++number;

                        }
                        else
                        {
                            do
                            {
                                Console.WriteLine("글 올라올 때까지 5초 대기중...");
                                await Task.Delay(5000);
                            } while (number > await dc.GetArticleNumber(galleryName, true));
                            Console.WriteLine();

                        }
                    }

                }
                if (isLatest)
                    --number;

                await Task.Delay(3000);
            }

#endif

            async void Print()
            {
                Console.WriteLine($"[{result.nickname}-{result.title}]{Environment.NewLine}[내용] {result.content}");

                string[] comments = await dc.GetComments(galleryName, number);

                if (comments != null)
                {
                    Console.Write("[댓글] ");

                    var fiveComments = comments.Take(5).ToArray();

                    for (int i = 0; i < fiveComments.Length; ++i)
                    {
                        Console.Write(fiveComments[i]);

                        if (i != fiveComments.Length - 1)
                            Console.Write(" // ");
                    }

                    Console.WriteLine();
                }
                Console.WriteLine();
            }

        }

    }

}

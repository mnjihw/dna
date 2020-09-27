using HtmlAgilityPack;
using System;
using System.Collections.Generic;
using System.Net.Http;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Telegram.Bot;

namespace 코로나_봇
{
    class Program
    {
        static async Task Main(string[] args)
        {
            int numberOfConfirmed = 0;
            bool notifies = false;
            string chatId = "581720134";//나
            TelegramBotClient bot = new TelegramBotClient("631711537:AAG164NGqHcdoYUnV-0QzKqE0B39z4a0U_0");
            HttpClient httpClient = new HttpClient();
            int[] arr = new int[4];

            while (true)
            {
                string result = "";
                try
                {
                    result = await httpClient.GetStringAsync("http://ncov.mohw.go.kr/bdBoardList_Real.do?brdId=&brdGubun=&ncvContSeq=&contSeq=&board_id=&gubun=");
                }
                catch(Exception e)
                {
                    await Task.Delay(TimeSpan.FromSeconds(20));
                    continue;
                }
                HtmlDocument document = new HtmlDocument();
                document.LoadHtml(result);

                var nodes = document.DocumentNode.SelectSingleNode("//ul[@class='s_listin_dot']").SelectNodes("li");

                for (int i = 0; i < nodes.Count; ++i)
                {
                    var match = Regex.Match(nodes[i].InnerText, @"([\d|,]+)명");
                    if (match.Success)
                    {
                        if (i == 0)
                        {
                            int n = int.Parse(match.Groups[1].Value);
                            if (n != numberOfConfirmed)
                            {
                                numberOfConfirmed = n;
                                arr[i] = n;
                                notifies = true;
                            }
                            else
                                break;
                        }
                        else if (notifies)
                            arr[i] = int.Parse(match.Groups[1].Value.Replace(",", ""));
                    }
                }
                if (notifies)
                {
                    string message = $"확진자: {arr[0]}{Environment.NewLine}격리해제: {arr[1]}{Environment.NewLine}사망자: {arr[2]}{Environment.NewLine}검사중: {arr[3]}";
                    await bot.SendTextMessageAsync(chatId, message);
                    notifies = false;
                }
                await Task.Delay(TimeSpan.FromSeconds(30));
            }
        }
    }
}

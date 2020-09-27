using System;
using System.Net.Http;
using System.Threading.Tasks;
using Telegram.Bot;
using System.Text.Json;
using System.Text.RegularExpressions;
using Newtonsoft.Json.Linq;
using System.Linq;

namespace 오픈마켓_재입고_알리미
{
    class Program
    {
        static async Task Main()
        {
            var client = new HttpClient();
            string result = "";
            var bot = new TelegramBotClient("631711537:AAG164NGqHcdoYUnV-0QzKqE0B39z4a0U_0");
            var chatId = "581720134";

            Console.Title = "오픈마켓 재입고 알리미";
            Console.Write("원하는 상품 URL 입력: ");
            var url = Console.ReadLine();
            Console.Write("원하는 옵션을 $로 구분해 정확히 입력(예) 블랙$L): ");
            var options = Console.ReadLine().Split('$');

            client.DefaultRequestHeaders.Add("user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.97 Safari/537.36 Edg/83.0.478.45");


            while (true)
            {
                try
                {
                    result = await client.GetStringAsync(url);
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                    await bot.SendTextMessageAsync(chatId, e.ToString());
                    await Task.Delay(TimeSpan.FromSeconds(30));
                }

                Console.WriteLine($"[{DateTime.Now:HH:mm:ss}]작동중...");

                
                var match = Regex.Match(result, @"""aCombinationOption"" : (\[.+\])");
                if (!match.Success)
                {
                    await bot.SendTextMessageAsync(chatId, "aCombinationOption 못찾음 " + result);
                    return;
                }
                    
                foreach(var jObject in JArray.Parse(match.Groups[1].Value))
                {
                    int count = 0;
                    string option;

                    for(int i = 1; (option = jObject[$"optionName{i}"]?.ToString()) != null; ++i)
                    { 
                        if (options.Any(s => option.Contains(s)))
                            ++count;
                        var quantity = int.Parse(jObject["stockQuantity"].ToString());

                        if(count == options.Length && quantity != 0)
                        {
                            var message = $"{options.Aggregate((a, b) => $"{a}/{b}")} 재고 {quantity}개 들어옴";
                            await bot.SendTextMessageAsync(chatId, message);
                            Console.WriteLine(message);
                            return;
                        }
                    }
                        
                }
                

                await Task.Delay(TimeSpan.FromHours(1));
            }
        }
    }
}

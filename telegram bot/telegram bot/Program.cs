using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Telegram.Bot;

namespace telegram_bot
{
    class Program
    {
        private static readonly string chatId = "581720134";//나
        //455892097 민석
        static async Task Main(string[] args)
        {

            var bot = new TelegramBotClient("631711537:AAG164NGqHcdoYUnV-0QzKqE0B39z4a0U_0");

            bot.StartReceiving();

            bot.OnMessage += (sender, e) =>
            {
                Console.WriteLine(e.Message.Text + " " + e.Message.Chat.Id);
            };
            Console.ReadLine();
            return;

            await bot.SendTextMessageAsync(chatId, "백예린 자리 먹었음 반드시 팀뷰어로 원격해라");
             
            
        }
    }
}

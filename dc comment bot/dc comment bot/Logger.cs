using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using Telegram.Bot;

namespace dc_comment_bot
{
    public static class Logger
    {
        private static TelegramBotClient Bot { get; } = new TelegramBotClient("1557742912:AAHJpIVYERHRxiXwME78ahwnYjg6-7tAbuE"); //legal_async_bot
        private static int ChatId { get; } = 1390930895; //legal
        public static async Task LogAsync(string message)
        {
            if (message.Length > 4000)
                message = message[..4000];

            var now = $"{DateTime.Now:yyyy-MM-dd HH:mm:ss}";
            var text = $"[{now}] {message}";
            Console.WriteLine(text);
            await File.AppendAllTextAsync(@"C:\crawler\cralwer_dcinside_log.txt", $"{text}{Environment.NewLine}");
            await File.WriteAllTextAsync(@"C:\crawler\cralwer_dcinside_stat.txt", now);
        }

        public static async Task SendTelegramMessageAsync(string message)
        {
            if (message.Length > 4000)
                message = message[..4000];

            var text = $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}, {Assembly.GetEntryAssembly().GetName().Name}] {message}";
            Console.WriteLine(text);
            await Bot.SendTextMessageAsync(ChatId, text);
        }
    }
}

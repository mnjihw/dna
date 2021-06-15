using System;
using System.IO;
using System.Reflection;
using System.Threading.Tasks;
using Telegram.Bot;

namespace forebet_console
{
    public static class Logger
    {
        private static TelegramBotClient Bot { get; } = new TelegramBotClient("1557742912:AAHJpIVYERHRxiXwME78ahwnYjg6-7tAbuE"); //legal_async_bot
        private static int ChatId { get; } = 1390930895; //legal
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

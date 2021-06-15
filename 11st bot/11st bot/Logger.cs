using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using Telegram.Bot;

namespace _11st_bot
{
    public static class Logger
    {
        private static TelegramBotClient Bot { get; set; }
        private static int ChatId { get; set; }
        private static bool Initialized { get; set; }

        public static void Initialize(string token, int chatId)
        {
            Bot = new TelegramBotClient(token);
            ChatId = chatId;
            Initialized = true;
        }

        public static async Task SendTelegramMessageAsync(string message)
        {
            if (!Initialized)
                throw new InvalidOperationException("Logger hasn't been initialized.");
            if (message.Length > 4000)
                message = message[..4000];

            var text = $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] {message}";
            Console.WriteLine(text);
            await Bot.SendTextMessageAsync(ChatId, text);
        }
    }
}

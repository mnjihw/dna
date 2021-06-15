using System;
using System.IO;
using System.Reflection;
using System.Threading.Tasks;
using Telegram.Bot;

namespace Instagram_archiver.Util
{
    public static class Logger
    {
        private static TelegramBotClient Bot { get; set; }
        private static int ChatId { get; set; }
        private static string LogPath { get; set; }
        private static string LogPrefix { get; set; }
        private static bool Initialized { get; set; }

        public static void Initialize(string logPath, string logPrefix, string token, int chatId)
        {
            LogPath = logPath;
            LogPrefix = logPrefix;
            Bot = new TelegramBotClient(token);
            ChatId = chatId;
            Initialized = true;
        }
        public static async Task LogAsync(string message)
        {
            if (!Initialized)
                throw new InvalidOperationException("Logger hasn't been initialized");
            var now = $"{DateTime.Now:yyyy-MM-dd HH:mm:ss}";
            var text = $"[{now}] {message}";
            Console.WriteLine(text);

            if (Directory.Exists(LogPath))
            {
                await File.AppendAllTextAsync(@$"{LogPath}\{LogPrefix}_log.txt", $"{text}{Environment.NewLine}");
                await File.WriteAllTextAsync(@$"{LogPath}\{LogPrefix}_stat.txt", now);
            }
        }

        public static async Task SendTelegramMessageAsync(string message)
        {
            if (!Initialized)
                throw new InvalidOperationException("Logger hasn't been initialized.");
            if (message.Length > 4000)
                message = message[..4000];

            var text = $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}, {Assembly.GetEntryAssembly().GetName().Name}] {message}";
            Console.WriteLine(text);
            await Bot.SendTextMessageAsync(ChatId, text);
        }
    }
}

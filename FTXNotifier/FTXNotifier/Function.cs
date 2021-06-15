using System;
using System.Net.Http;
using System.Reflection;
using System.Security.Cryptography;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

using Amazon.Lambda.Core;
using Telegram.Bot;

// Assembly attribute to enable the Lambda function's JSON input to be converted into a .NET class.
[assembly: LambdaSerializer(typeof(Amazon.Lambda.Serialization.SystemTextJson.DefaultLambdaJsonSerializer))]

namespace FTXNotifier
{
    public class Function
    {
        private HttpClient HttpClient { get; } = new();
        private static string ApiKey { get; } = Environment.GetEnvironmentVariable("ApiKey");
        private static string SecretKey { get; } = Environment.GetEnvironmentVariable("SecretKey");
        private static string ChatId { get; } = Environment.GetEnvironmentVariable("ChatId");
        private static TelegramBotClient Bot { get; } = new(Environment.GetEnvironmentVariable("TelegramBotToken"));

        private async Task SendTelegramMessageAsync(string message)
        {
            if (message.Length > 4000)
                message = message[..4000];

            var text = $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}, {Assembly.GetEntryAssembly().GetName().Name}] {message}";
            Console.WriteLine(text);
            await Bot.SendTextMessageAsync(ChatId, text);
        }

        public async Task NotifyIfDepositCompleted(ILambdaContext context)
        {
            LambdaLogger.Log($"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] NotifyIfDepositCompleted »£√‚µ ");
            
            using var hmac = new HMACSHA256(Encoding.UTF8.GetBytes(SecretKey));
            var nonce = DateTimeOffset.Now.ToUnixTimeMilliseconds();
            var hash = hmac.ComputeHash(Encoding.UTF8.GetBytes($"{nonce}GET/api/wallet/deposits"));

            HttpClient.DefaultRequestHeaders.Add("FTX-KEY", ApiKey);
            HttpClient.DefaultRequestHeaders.Add("FTX-TS", nonce.ToString());
            HttpClient.DefaultRequestHeaders.Add("FTX-SIGN", Convert.ToHexString(hash).ToLower());

            string result;
            JsonDocument json;
            try
            {
                result = await HttpClient.GetStringAsync($"https://ftx.com/api/wallet/deposits");
                LambdaLogger.Log($"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] API »£√‚«‘");
                json = JsonDocument.Parse(result);
            }
            catch (Exception)
            {
                return;
            }

            if (json.RootElement.GetProperty("success").GetBoolean())
            {
                foreach (var element in json.RootElement.GetProperty("result").EnumerateArray())
                {
                    var confirmedTime = element.GetProperty("confirmedTime").GetString();

                    if (confirmedTime != null && (DateTime.Now - DateTime.Parse(confirmedTime)).TotalMinutes <= 5)
                    {
                        LambdaLogger.Log($"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] FTX ¿‘±› øœ∑·µ ");
                        await SendTelegramMessageAsync("FTX ¿‘±› øœ∑·µ ");
                    }

                }
            }

            
        }
    }

    
}

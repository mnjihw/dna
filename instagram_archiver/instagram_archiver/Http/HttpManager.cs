using HtmlAgilityPack;
using Instagram_archiver.Util;
using System;
using System.IO;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;

namespace Instagram_archiver.Http
{
    public class HttpManager
    {
        private static Lazy<HttpManager> InstanceHolder { get; } = new (() => new ());
        public static HttpManager Instance => InstanceHolder.Value;
        private HttpManager() { }

        private static CookieContainer CookieContainer { get; } = new ();
        private static HttpClientHandler HttpClientHandler { get; } = new () { AutomaticDecompression = DecompressionMethods.All, CookieContainer = CookieContainer };
        public HttpClient HttpClient { get; } = new (HttpClientHandler) { Timeout = TimeSpan.FromSeconds(30) };

        public HtmlNode HtmlNode => HtmlDocument.DocumentNode;
        public HtmlDocument HtmlDocument { get; } = new ();

        private static async Task WaitFor(int second)
        {
            for (int i = 1; i <= second; ++i)
            {
                await Task.Delay(1000);
                Console.WriteLine($"대기중... ({(int)((double)i / second * 100)}%/100%)");
            }
        }
        public async Task<bool> GetHtmlFromAsync(string url)
        {
            string result;

            try
            {
                while (true)
                {
                    var responseMessage = await HttpClient.GetAsync(url);

                    if (responseMessage.StatusCode != HttpStatusCode.OK)
                    {
                        if (responseMessage.StatusCode == HttpStatusCode.Redirect)
                        {
                            
                            await File.WriteAllTextAsync("c.txt", responseMessage.Headers.Location.ToString());
                            await Logger.SendTelegramMessageAsync("c.txt 남긴다");
                        }
                        await Logger.SendTelegramMessageAsync($"{responseMessage.StatusCode} 뜸 {await responseMessage.Content.ReadAsStringAsync()}");
                        return false;
                    }

                    result = await responseMessage.Content.ReadAsStringAsync();

                    if (string.IsNullOrWhiteSpace(result) || result.Contains("ExpressVPN Router"))
                    {
                        if (result.Contains("ExpressVPN Router"))
                        { 
                            await Logger.SendTelegramMessageAsync("VPN 페이지 뜸");
                        }
                        await WaitFor(90);
                        continue;
                    }
                    break;
                }
            }
            catch (HttpRequestException)
            {
                return false;
            }
            catch (TaskCanceledException)
            {
                Console.WriteLine("타임아웃");
                return false;
            }
            catch (Exception e)
            {
                await Logger.SendTelegramMessageAsync(e.ToString());
                return false;
            }

            HtmlDocument.LoadHtml(result);
            return true;
        }
    }
}

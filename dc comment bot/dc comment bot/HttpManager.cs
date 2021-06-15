using HtmlAgilityPack;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;

namespace dc_comment_bot
{
    public class HttpManager
    {
        private static Lazy<HttpManager> InstanceHolder { get; } = new Lazy<HttpManager>(() => new HttpManager());
        public static HttpManager Instance => InstanceHolder.Value;
        private HttpManager() { }
        public static CookieContainer CookieContainer { get; } = new CookieContainer();

        private static HttpClientHandler HttpClientHandler { get; } = new HttpClientHandler { AutomaticDecompression = DecompressionMethods.All, CookieContainer = CookieContainer };
        public HttpClient HttpClient { get; } = new HttpClient(HttpClientHandler) { Timeout = TimeSpan.FromSeconds(30) };
        public HtmlDocument HtmlDocument { get; } = new HtmlDocument();
        public HtmlNode HtmlNode => HtmlDocument.DocumentNode;

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

            //필요하면 호출 전에 헤더 일일이 세팅하기
            //HttpClient.DefaultRequestHeaders.Clear();
            try
            {
                while (true)
                {
                    var responseMessage = await HttpClient.GetAsync(url);

                    if (responseMessage.StatusCode != HttpStatusCode.OK)
                        return false;
                    result = await responseMessage.Content.ReadAsStringAsync();

                    if (string.IsNullOrWhiteSpace(result))
                    {
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

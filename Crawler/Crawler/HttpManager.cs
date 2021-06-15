using HtmlAgilityPack;
using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace Crawler
{

    public class HttpManager
    {
       

        private HttpManager() { }
        public enum ResultType
        {
            Html, Json, PlainText,
        }
        public HttpClient HttpClient { get; } = new HttpClient(HttpClientHandler) { Timeout = TimeSpan.FromSeconds(30) };
        public HtmlNode HtmlRootNode => HtmlDocument.DocumentNode;
        public JsonElement JsonRootElement => JsonDocument.RootElement;
        public string Result { get; set; }



        private static Lazy<HttpManager> InstanceHolder { get; } = new Lazy<HttpManager>(() => new HttpManager());
        public static HttpManager Instance => InstanceHolder.Value;
        
        private static CookieContainer CookieContainer { get; } = new CookieContainer();
        private static HttpClientHandler HttpClientHandler { get; } = new HttpClientHandler { AutomaticDecompression = DecompressionMethods.All, CookieContainer = CookieContainer };
        
        private HtmlDocument HtmlDocument { get; } = new HtmlDocument();
        private JsonDocument JsonDocument { get; set; }



        private static async Task WaitFor(int second)
        {
            for (int i = 1; i <= second; ++i)
            {
                await Task.Delay(1000);
                Console.WriteLine($"대기중... ({(int)((double)i / second * 100)}%/100%)");
            }
        }
        public async Task<bool> GetAsync(string uri, ResultType resultType = ResultType.Html)
        {
            string result;

            try
            {
                while (true)
                {
                    var responseMessage = await HttpClient.GetAsync(uri);

                    if (responseMessage.StatusCode != HttpStatusCode.OK)
                        return false;
                    result = await responseMessage.Content.ReadAsStringAsync();

                    if (string.IsNullOrWhiteSpace(result))
                    {
                        await WaitFor(90); //밖에서 처리해야지 이건
                        continue;
                    }
                    break;
                }
            }
            catch (TaskCanceledException)
            {
                Console.WriteLine("타임아웃");
                return false;
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                return false;
            }

            switch (resultType)
            {
                case ResultType.Html:
                    HtmlDocument.LoadHtml(result);
                    break;
                case ResultType.Json:
                    JsonDocument = JsonDocument.Parse(result);
                    break;
                case ResultType.PlainText:
                    Result = result;
                    break;
            }
            
            return true;
        }

        public async Task<bool> PostAsync(string uri, HttpContent httpContent, ResultType resultType)
        {
            //포스트는 json이 올 때도 있고 처리가 아예 불필요할 때도 있ㅇ음
            string result;
            try
            {
                var responseMessage = await HttpClient.PostAsync(uri, httpContent);
                if (responseMessage.StatusCode != HttpStatusCode.OK)
                    return false;
                result = await responseMessage.Content.ReadAsStringAsync();
            }
            catch (TaskCanceledException)
            {
                Console.WriteLine("타임아웃");
                return false;
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                return false;
            }

            switch (resultType)
            {
                case ResultType.Html:
                    HtmlDocument.LoadHtml(result);
                    break;
                case ResultType.Json:
                    JsonDocument = JsonDocument.Parse(result);
                    break;
                case ResultType.PlainText:
                    Result = result;
                    break;
            }
            return true;
        }
    }
}

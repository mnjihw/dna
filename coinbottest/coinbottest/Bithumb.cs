using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Net.Http;
using System.Net;
using System.Windows;
using HtmlAgilityPack;
using System.Collections.Concurrent;
using System.IO;
using System.Diagnostics;
using System.Threading;

namespace coinbottest
{
    public sealed class Bithumb : IExchange
    {
        private readonly string[] listOfCoins = { "ABT", "ADA", "AE", "ANKR", "BAT", "BCD", "BCH", "BSV", "BTC", "BTG", "BTT", "CMT", "CRO", "CTXC", "DASH", "ELF", "ENJ", "EOS", "ETC", "ETH", "GNT", "GTO", "GXC", "HC", "ICX", "IOST", "ITC", "KNC", "LAMB", "LBA", "LINK", "LOOM", "LRC", "LTC", "MCO", "MITH", "MIX", "MTL", "MXC", "NPXS", "OCN", "OMG", "PAY", "PIVX", "POLY", "POWR", "PST", "QKC", "QTUM", "RDN", "REP", "SALT", "SNT", "STEEM", "STRAT", "THETA", "TRX", "VET", "WAVES", "WTC", "XEM", "XLM", "XMR", "XRP", "XVG", "ZEC", "ZIL", "ZRX" };
        private static readonly Lazy<Bithumb> _instanceHolder = new Lazy<Bithumb>(() => new Bithumb());
        public static Bithumb Instance => _instanceHolder.Value;
        private static readonly CookieContainer cookieContainer = new CookieContainer();
        private static readonly HttpClientHandler httpClientHandler = new HttpClientHandler { CookieContainer = cookieContainer };
        private readonly HttpClient _httpClient = new HttpClient(httpClientHandler);
        private string CSRFToken { get; set; }
        private Bithumb() { }


        public async Task<bool> GetCodesOfCoins(ConcurrentDictionary<string, string> codes)
        {
            string result = "";

            _httpClient.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.89 Whale/1.6.81.13 Safari/537.36");

            try
            {
                result = await _httpClient.GetStringAsync("https://www.bithumb.com");
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
            HtmlDocument document = new HtmlDocument();
            document.LoadHtml(result);

            CSRFToken = document.DocumentNode.SelectSingleNode("//input[@name='csrf_xcoin_name']").Attributes["value"].Value;
            int startIndex = result.IndexOf("var COIN = ") + "var COIN = ".Length;
            var coins = result.Substring(startIndex, result.IndexOf(';', startIndex) - startIndex);

            foreach (var property in (JObject.Parse(coins)["C0100"] as JObject).Properties())
            {
                var symbol = property.Value["symbol_name"].ToString();

                if (listOfCoins.Contains(symbol))
                    codes[symbol] = property.Name;
            }

            return true;
        }

        public void GetDeterminedClosingPrices(ConcurrentDictionary<string, string> codes, ConcurrentDictionary<string, double> prices)
        {

            _httpClient.DefaultRequestHeaders.Clear();
            _httpClient.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.89 Whale/1.6.81.13 Safari/537.36");
            _httpClient.DefaultRequestHeaders.Add("X-requested-with", "XMLHttpRequest");
            _httpClient.DefaultRequestHeaders.TryAddWithoutValidation("content-type", "application/x-www-form-urlencoded; charset=UTF-8");

            Parallel.ForEach(codes, new ParallelOptions { MaxDegreeOfParallelism = Environment.ProcessorCount }, coin =>
            {
                string result;
                JArray arr;

                var formData = new ConcurrentDictionary<string, string>
                {
                    ["crncCd"] = "C0100", //KRW
                    ["tickType"] = "01H",
                    ["csrf_xcoin_name"] = CSRFToken
                };

                formData["coinType"] = coin.Value;

                try
                {
                    result = _httpClient.PostAsync($"https://www.bithumb.com/trade_history/chart_data?_={DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()}", new FormUrlEncodedContent(formData)).Result.Content.ReadAsStringAsync().Result;
                    arr = JObject.Parse(result)["data"] as JArray;
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                    return;
                }

                TimeSpan gap, nearestTime = TimeSpan.MaxValue;
                var now = DateTime.Now;
                double nearestPrice = double.MaxValue;

                for (int i = arr.Count - 1; i > arr.Count - 25; --i)
                {
                    DateTimeOffset time = DateTimeOffset.FromUnixTimeMilliseconds((long)arr[i][0]).ToLocalTime();

                    var today9 = DateTime.Today.AddHours(9);
                    gap = (today9.TimeOfDay - time.TimeOfDay).Duration();

                    if (nearestTime > gap)
                    {
                        nearestTime = gap;
                        nearestPrice = double.Parse(arr[i][2].ToString());
                    }

                    if (time.Hour == 9)
                    {
                        if ((time.Day == now.Day && now.Hour >= 9 && now.Hour <= 23) || (time.Day == now.Day - 1 && now.Hour >= 0 && now.Hour <= 8))
                        {
                            prices[coin.Key] = double.Parse(arr[i][2].ToString());   
                            break;
                        }
                    }

                }
                if (!prices.ContainsKey(coin.Key))
                    prices[coin.Key] = nearestPrice;

                Thread.Sleep(100);
            });
        }

        public async Task<bool> GetCurrentPrices(ConcurrentDictionary<string, PricesByExchange> prices)
        {
            string result;
            JObject jObject;

            try
            {
                result = await _httpClient.GetStringAsync($"https://api.bithumb.com/public/ticker/all");
                jObject = JObject.Parse(result);
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                return false;
            }

            if (jObject["status"].ToString() == "0000")
            {
                string name;

                (jObject["data"] as JObject).Remove("date");

                foreach (var property in (jObject["data"] as JObject).Properties())
                {
                    name = property.Name;
                    if (!prices.ContainsKey(name))
                        prices[name] = new PricesByExchange();

                    prices[name].Name = name;
                    prices[name].Bithumb = double.Parse(property.Value["closing_price"].ToString());

                }
                return true;
            }
            return false;
        }
    }
}


using Newtonsoft.Json.Linq;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace coinbottest
{
    public sealed class Huobi : IExchange
    {
        private static readonly Lazy<Huobi> _instanceHolder = new Lazy<Huobi>(() => new Huobi());
        public static Huobi Instance => _instanceHolder.Value;
        private readonly HttpClient _httpClient = new HttpClient();
        private readonly string[] listOfCoins = { "ADA", "ADA/BTC", "ADX/BTC", "AE", "AE/BTC", "ALGO", "ALGO/BTC", "APPC/BTC", "ARDR/BTC", "ARPA", "AST/BTC", "ATOM", "ATOM/BTC", "ATP", "BAT", "BAT/BTC", "BCD/BTC", "BCH", "BCH/BTC", "BCX/BTC", "BFT/BTC", "BIFI/BTC", "BLZ/BTC", "BSV", "BSV/BTC", "BTC", "BTG/BTC", "BTM", "BTM/BTC", "BTS", "BTS/BTC", "BTT", "BTT/BTC", "CHAT/BTC", "CMT", "CMT/BTC", "CNNS", "CRE", "CRO", "CRO/BTC", "CTXC", "CTXC/BTC", "CVC", "CVC/BTC", "DASH", "DASH/BTC", "DBC/BTC", "DCR", "DCR/BTC", "DGB/BTC", "DGD/BTC", "DOCK/BTC", "DOGE", "DOGE/BTC", "DTA/BTC", "ELF", "ELF/BTC", "ENG/BTC", "EOS", "EOS/BTC", "ETC", "ETC/BTC", "ETH", "ETH/BTC", "EVX/BTC", "FSN/BTC", "GAS/BTC", "GNT", "GNT/BTC", "GRS/BTC", "GT", "GT/BTC", "GTC/BTC", "GXC", "HC", "HC/BTC", "HIT", "HOT/BTC", "HT", "ICX/BTC", "IOST", "IOST/BTC", "IOTA", "IOTA/BTC", "ITC", "KMD/BTC", "KNC/BTC", "LAMB", "LAMB/BTC", "LBA", "LBA/BTC", "LINK", "LINK/BTC", "LOOM/BTC", "LSK/BTC", "LTC", "LTC/BTC", "LUN/BTC", "LYM/BTC", "MANA", "MANA/BTC", "MCO/BTC", "MDS", "MTL/BTC", "MXC/BTC", "NANO", "NANO/BTC", "NAS", "NAS/BTC", "NCASH/BTC", "NEO", "NEO/BTC", "NKN", "NKN/BTC", "NPXS/BTC", "NULS", "NULS/BTC", "OCN", "OCN/BTC", "OMG", "OMG/BTC", "ONT", "ONT/BTC", "OST/BTC", "PAY/BTC", "PHX/BTC", "POLY/BTC", "POWR/BTC", "QASH/BTC", "QSP/BTC", "QTUM", "QTUM/BTC", "RCN/BTC", "RDN/BTC", "REN/BTC", "REQ/BTC", "RUFF", "RUFF/BTC", "SBTC/BTC", "SALT/BTC", "SC/BTC", "SKM", "SMT", "SNT", "SNT/BTC", "SRN/BTC", "STEEM", "STEEM/BTC", "STORJ", "STORJ/BTC", "THETA", "THETA/BTC", "TNB/BTC", "TNT/BTC", "TRX", "TRX/BTC", "TT", "VET", "VET/BTC", "VIDY", "WAN/BTC", "WAVES", "WAVES/BTC", "WICC", "WPR/BTC", "WTC", "WTC/BTC", "XEM", "XEM/BTC", "XLM", "XLM/BTC", "XMR", "XMR/BTC", "XRP", "XRP/BTC", "XTZ", "XTZ/BTC", "XVG/BTC", "XZC/BTC", "ZEC", "ZEC/BTC", "ZEN/BTC", "ZIL", "ZIL/BTC", "ZRX", "ZRX/BTC" };
        private readonly string[] namesOfPair = { "BTC", "USDT" };
        private Huobi() { }


        public void GetDeterminedClosingPrices(ConcurrentDictionary<string, double> prices)
        {
            Parallel.ForEach(listOfCoins, new ParallelOptions { MaxDegreeOfParallelism = 1 }, coin =>
            {
                string result, name;
                JArray arr;
                var now = DateTimeOffset.Now;

                int index = coin.IndexOf('/');

                name = coin.ToLower().Replace("/", "");

                if(index == -1)
                    name += "usdt";

                try
                {
                    result = _httpClient.GetStringAsync($"https://api.huobi.pro/market/history/kline?symbol={name}&period=1day&size=1").Result;
                    arr = JObject.Parse(result)["data"] as JArray;
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                    return;
                }

                if (arr.Count != 0)
                    prices[coin] = double.Parse(arr[0]["close"].ToString());

                Thread.Sleep(200);
            });
        }
        public void GetDeterminedClosingPrices2(ConcurrentDictionary<string, double> prices)
        {
            _httpClient.DefaultRequestHeaders.Clear();
            _httpClient.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.89 Whale/1.6.81.13 Safari/537.36");

            Parallel.ForEach(listOfCoins, new ParallelOptions { MaxDegreeOfParallelism = 1 }, coin =>
            {
                string result;
                JArray arr;
                TimeSpan gap, nearestTime = TimeSpan.MaxValue;
                double nearestPrice = double.MaxValue;
                var now = DateTimeOffset.Now;

                try
                {
                    result = _httpClient.GetStringAsync($"https://api.huobi.pro/market/history/kline?symbol={coin}&period=60min&size=24").Result;
                    arr = JObject.Parse(result)["data"] as JArray;
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                    return;
                }
                
                for (int i = 0; i < arr.Count; ++i)
                {
                    DateTimeOffset time = DateTimeOffset.FromUnixTimeMilliseconds((long)arr[i]["id"] * 1000).ToLocalTime();
                    var today9 = DateTime.Today.AddHours(9);

                    gap = (today9.TimeOfDay - time.TimeOfDay).Duration();

                    if (nearestTime > gap)
                    {
                        nearestTime = gap;
                        nearestPrice = double.Parse(arr[i]["close"].ToString());
                    }

                    if (time.Hour == 9)
                    {
                        if ((time.Day == now.Day && now.Hour >= 9 && now.Hour <= 23) || (time.Day == now.Day - 1 && now.Hour >= 0 && now.Hour <= 8))
                        {
                            prices[coin] = double.Parse(arr[i]["close"].ToString());
                            break;
                        }
                    }
                }
                if (!prices.ContainsKey(coin))
                    prices[coin] = nearestPrice;

                Thread.Sleep(200);
            });
        }
        public async Task<bool> GetCurrentPrices(ConcurrentDictionary<string, PricesByExchange> prices)
        {
            JObject jObject;
            string result;

            try
            {
                result = await _httpClient.GetStringAsync("https://api.huobi.pro/market/tickers");
                jObject = JObject.Parse(result);
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                return false;
            }

            if (jObject["status"].ToString() == "ok")
            {
                JToken jToken = jObject["data"];
                string name;

                foreach (var token in jToken)
                {
                    name = token["symbol"].ToString().ToUpper();

                    if (!namesOfPair.Any(s =>
                    {
                        if (name.EndsWith(s))
                        {
                            if (s == "USDT")
                                name = name.Substring(0, name.Length - s.Length);
                            else
                                name = $"{name.Substring(0, name.Length - s.Length)}/{s}";
                            return true;
                        }
                        return false;
                    }))
                        continue;

                    if (!prices.ContainsKey(name))
                        prices[name] = new PricesByExchange();

                    prices[name].Name = name;
                    prices[name].Huobi = double.Parse(token["close"].ToString());
                }
                return true;
            }
            return false;
        }
    }
}
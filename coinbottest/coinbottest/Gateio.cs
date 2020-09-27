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
    public sealed class Gateio : IExchange
    {
        private static readonly Lazy<Gateio> _instanceHolder = new Lazy<Gateio>(() => new Gateio());
        public static Gateio Instance => _instanceHolder.Value;
        private readonly HttpClient _httpClient = new HttpClient();
        private readonly string[] listOfCoins = { "ABT", "ADA", "ADA/BTC", "AE", "AE/BTC", "ALGO", "ARPA", "ATOM", "ATOM/BTC", "ATP", "BAT", "BAT/BTC", "BCD", "BCD/BTC", "BCH", "BCH/BTC", "BCHSV", "BCHSV/BTC", "BCN/BTC", "BCX/BTC", "BIFI/BTC", "BNB", "BTC", "BTG", "BTG/BTC", "BTM", "BTM/BTC", "BTS", "BTS/BTC", "BTT", "CELR", "CNNS", "COS", "CRO", "CVC", "DASH", "DASH/BTC", "DBC/BTC", "DCR", "DCR/BTC", "DOCK", "DOGE", "DOGE/BTC", "ELF", "EOS", "EOS/BTC", "ETC", "ETC/BTC", "ETH", "ETH/BTC", "FTM", "FUN", "GAS", "GAS/BTC", "GNT", "GT", "GT/BTC", "GTC/BTC", "GXS/BTC", "HC", "HC/BTC", "HIT", "HT", "ICX", "IOTA", "IOTA/BTC", "JNT/BTC", "KNC", "LAMB", "LBA", "LINK", "LRC", "LRC/BTC", "LSK", "LSK/BTC", "LTC", "LTC/BTC", "LYM/BTC", "MANA", "MBL", "MCO", "MDS", "MEDX", "MITH", "MIX", "MXC", "MXC/BTC", "NANO", "NANO/BTC", "NAS", "NAS/BTC", "NEO", "NEO/BTC", "NKN", "OAX/BTC", "OCN", "OCN/BTC", "OMG", "OMG/BTC", "ONE", "ONG", "ONT", "OST", "PAX", "PAY", "PAY/BTC", "POWR", "POWR/BTC", "PST", "QASH/BTC", "QKC", "QKC/BTC", "QLC/BTC", "QTUM", "QTUM/BTC", "RDN", "RFR", "RUFF", "RUFF/BTC", "SALT", "SBTC/BTC", "SKM", "SMT", "SNT", "SNT/BTC", "STORJ", "STORJ/BTC", "TFUEL", "THETA", "TOMO", "TRX", "TUSD", "USDC", "VET", "VIDY", "WAVES", "WAVES/BTC", "WICC", "XEM", "XEM/BTC", "XLM", "XLM/BTC", "XMR", "XMR/BTC", "XRP", "XRP/BTC", "XTZ", "XTZ/BTC", "XVG", "XVG/BTC", "ZEC", "ZEC/BTC", "ZIL", "ZRX", "ZRX/BTC" };
        private Gateio() { }

        public void GetDeterminedClosingPrices(ConcurrentDictionary<string, double> prices)
        {
            Parallel.ForEach(listOfCoins, new ParallelOptions { MaxDegreeOfParallelism = Environment.ProcessorCount }, coin =>
            {
                string result, name;
                JArray arr;

                name = coin.ToLower().Replace('/', '_');

                if(!name.Contains('_'))
                    name += "_usdt";

                try
                {
                    result = _httpClient.GetStringAsync($"https://data.gateio.co/api2/1/candlestick2/{name}?group_sec=86400&range_hour=0").Result;
                    arr = JObject.Parse(result)["data"] as JArray;
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                    return;
                }

                if (arr.Count != 0)
                    prices[coin] = double.Parse(arr[0][2].ToString());
                
                Thread.Sleep(200);
            });


        }
        public void GetDeterminedClosingPrices2(ConcurrentDictionary<string, double> prices)
        {
            Parallel.ForEach(listOfCoins, new ParallelOptions { MaxDegreeOfParallelism = Environment.ProcessorCount }, coin =>
            {
                string result;
                JArray arr;
                TimeSpan gap, nearestTime = TimeSpan.MaxValue;
                double nearestPrice = double.MaxValue;
                var now = DateTimeOffset.Now;

                try
                {
                    result = _httpClient.GetStringAsync($"https://data.gateio.co/api2/1/candlestick2/{coin}?group_sec=3600&range_hour=24").Result;
                    arr = JObject.Parse(result)["data"] as JArray;
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                    return;
                }

                for (int i = 0; i < arr.Count; ++i)
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
                            prices[coin] = double.Parse(arr[i][2].ToString());
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
            string result;
            JObject jObject;

            try
            {
                result = await _httpClient.GetStringAsync("https://data.gateio.co/api2/1/marketlist");
                jObject = JObject.Parse(result);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message + e.StackTrace);
                return false;
            }

            if (jObject["result"].ToString() == "true")
            {
                JArray jArray;
                string name;

                jArray = jObject["data"] as JArray;

                for (int i = 0; i < jArray.Count; ++i)
                {
                    name = jArray[i]["pair"].ToString().ToUpper().Replace('_', '/');

                    if (name.EndsWith("ETH") || name.EndsWith("QTUM") || name.EndsWith("SNET"))
                        continue;
                    else if (name.EndsWith("USDT"))
                        name = name.Substring(0, name.IndexOf('/'));

                    if (!prices.ContainsKey(name))
                        prices[name] = new PricesByExchange();

                    prices[name].Name = name;
                    prices[name].Gateio = double.Parse(jArray[i]["rate"].ToString());
                }
                return true;
            }
            return false;
        }
    }
}

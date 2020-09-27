using Newtonsoft.Json.Linq;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace 리듬_코인_트레이서
{
    public sealed class Huobi : IExchange
    {
        private static readonly Lazy<Huobi> _instanceHolder = new Lazy<Huobi>(() => new Huobi());
        public static Huobi Instance => _instanceHolder.Value;
        private readonly HttpClient _httpClient = new HttpClient();
        private readonly string[] namesOfPair = { "BTC", "USDT" };
        private Huobi() { }

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
                MessageBox.Show(e.ToString());
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
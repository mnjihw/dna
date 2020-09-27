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
    public sealed class Gateio : IExchange
    {
        private static readonly Lazy<Gateio> _instanceHolder = new Lazy<Gateio>(() => new Gateio());
        public static Gateio Instance => _instanceHolder.Value;
        private readonly HttpClient _httpClient = new HttpClient();
        private Gateio() { }
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
                MessageBox.Show(e.ToString());
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

using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Net.Http;
using System.Net;
using System.Collections.Concurrent;
using System.Windows;

namespace 리듬_코인_트레이서
{
    public sealed class Bithumb : IExchange
    {
        private static readonly Lazy<Bithumb> _instanceHolder = new Lazy<Bithumb>(() => new Bithumb());
        public static Bithumb Instance => _instanceHolder.Value;
        private readonly HttpClient _httpClient = new HttpClient();
        private Bithumb() { }

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
                MessageBox.Show(e.ToString());
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


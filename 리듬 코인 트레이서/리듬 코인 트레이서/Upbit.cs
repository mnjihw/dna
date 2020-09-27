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
    public sealed class Upbit : IExchange
    {
        private static readonly Lazy<Upbit> _instanceHolder = new Lazy<Upbit>(() => new Upbit());
        public static Upbit Instance => _instanceHolder.Value;
        private readonly HttpClient _httpClient = new HttpClient();
        private Upbit() { }
        private async Task<JArray> GetAllMarkets()
        {
            string result;

            try
            {
                result = await _httpClient.GetStringAsync("https://api.upbit.com/v1/market/all");
                return JArray.Parse(result);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString());
                return null;
            }
        }
        public async Task<bool> GetCurrentPrices(ConcurrentDictionary<string, PricesByExchange> prices)
        {
            string result;
            JArray jArray;

            jArray = await GetAllMarkets();

            try
            {
                result = await _httpClient.GetStringAsync($"https://api.upbit.com/v1/ticker?markets={string.Join(",", jArray.SelectTokens("$[*]['market']"))}");
                jArray = JArray.Parse(result);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString());
                return false;
            }

            string name;

            for (int i = 0; i < jArray.Count; ++i)
            {
                name = jArray[i]["market"].ToString();

                if (name.StartsWith("ETH") || name.StartsWith("USDT"))
                    continue;

                int index = name.IndexOf('-');

                if (name.StartsWith("KRW"))
                    name = name.Substring(index + 1, name.Length - index - 1);
                else
                    name = $"{name.Substring(index + 1, name.Length - index - 1)}/{name.Substring(0, index)}";

                if (!prices.ContainsKey(name))
                    prices[name] = new PricesByExchange();

                prices[name].Name = name;
                prices[name].Upbit = double.Parse(jArray[i]["trade_price"].ToString());
            }
            return true;
        }
    }
}

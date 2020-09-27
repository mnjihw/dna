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
    public sealed class Upbit : IExchange
    {
        private static readonly Lazy<Upbit> _instanceHolder = new Lazy<Upbit>(() => new Upbit());
        public static Upbit Instance => _instanceHolder.Value;
        private readonly HttpClient _httpClient = new HttpClient();
        private readonly string[] listOfCoins = { "ADA", "ADA/BTC", "ADX/BTC", "ANKR", "ANKR/BTC", "ARDR/BTC", "ARK/BTC", "ATOM", "ATOM/BTC", "BAT", "BAT/BTC", "BCH", "BCH/BTC", "BCPT/BTC", "BFT/BTC", "BNT/BTC", "BSV", "BSV/BTC", "BTC", "BTG", "BTM/BTC", "BTS/BTC", "BTT", "BTT/BTC", "CRE", "CRO/BTC", "CTXC/BTC", "CVC", "CVC/BTC", "DASH", "DASH/BTC", "DCR", "DCR/BTC", "DENT/BTC", "DGB/BTC", "DNT/BTC", "DOGE/BTC", "DTA/BTC", "ELF", "ELF/BTC", "ENG/BTC", "ENJ", "ENJ/BTC", "EOS", "EOS/BTC", "ETC", "ETC/BTC", "ETH", "ETH/BTC", "FSN/BTC", "GAS", "GNT", "GNT/BTC", "GO/BTC", "GRS/BTC", "GTO", "GTO/BTC", "ICX", "IOST", "IOST/BTC", "IOTA", "IOTX/BTC", "JNT/BTC", "KMD/BTC", "KNC", "LAMB/BTC", "LBA/BTC", "LOOM", "LOOM/BTC", "LRC/BTC", "LSK", "LSK/BTC", "LTC", "LTC/BTC", "MANA", "MANA/BTC", "MBL", "MCO", "MCO/BTC", "MEDX", "MFT", "MFT/BTC", "MTL", "MTL/BTC", "NAV/BTC", "NCASH/BTC", "NEO", "NKN/BTC", "NPXS", "NPXS/BTC", "NXS/BTC", "OCN/BTC", "OMG", "OMG/BTC", "ONG", "ONT", "OST", "OST/BTC", "PAX/BTC", "PAY/BTC", "PIVX", "PIVX/BTC", "POLY", "POLY/BTC", "POWR", "POWR/BTC", "QKC", "QTUM", "QTUM/BTC", "RCN/BTC", "REP", "REP/BTC", "RFR", "RLC/BTC", "RVN/BTC", "SC/BTC", "SNT", "SNT/BTC", "SRN/BTC", "STEEM", "STEEM/BTC", "STORJ", "STORJ/BTC", "STORM", "STORM/BTC", "STRAT", "STRAT/BTC", "SYS/BTC", "TFUEL", "THETA", "TRX", "TRX/BTC", "TT", "TUSD/BTC", "VIA/BTC", "VIB/BTC", "WAVES", "WAVES/BTC", "XEM", "XEM/BTC", "XLM", "XLM/BTC", "XMR", "XMR/BTC", "XRP", "XRP/BTC", "XVG/BTC", "XZC/BTC", "ZEC", "ZEC/BTC", "ZEN/BTC", "ZIL", "ZIL/BTC", "ZRX", "ZRX/BTC" };
        private Upbit() { }

        public void GetDeterminedClosingPrices(ConcurrentDictionary<string, double> prices)
        {
            Parallel.ForEach(listOfCoins, new ParallelOptions { MaxDegreeOfParallelism = 3 }, coin =>
            {
                string result, name;
                JArray arr;
                var now = DateTimeOffset.Now;

                int index = coin.IndexOf('/');

                if(index != -1)
                    name = $"{coin.Substring(index + 1, coin.Length - index - 1)}-{coin.Substring(0, index)}";
                else
                    name = $"KRW-{coin}";
                
                try
                {
                    result = _httpClient.GetStringAsync($"https://api.upbit.com/v1/candles/days?count=1&market={name}").Result;
                    arr = JArray.Parse(result);
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                    return;
                }

                if (arr.Count != 0)
                    prices[coin] = double.Parse(arr[0]["trade_price"].ToString());                
                Thread.Sleep(350);
            });
        }
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
                Console.WriteLine(e.Message + e.StackTrace);
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
                Console.WriteLine(e.Message + e.StackTrace);
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

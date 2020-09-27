using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace 리듬_코인_트레이서
{
    public static class Cryptowatch
    {
        private static readonly HttpClient _httpClient = new HttpClient();

        public static async Task<double> GetUSDTToUSDExchangeRate()
        {
            JObject jObject;
            string result;
            try
            {
                result = await _httpClient.GetStringAsync("https://api.cryptowat.ch/markets/kraken/usdtusd/price");
                jObject = JObject.Parse(result);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString());
                return 0;
            }

            return double.Parse(jObject["result"]["price"].ToString());
        }

    }
}

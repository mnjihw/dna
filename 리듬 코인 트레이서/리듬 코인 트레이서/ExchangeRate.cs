using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using HtmlAgilityPack;

namespace 리듬_코인_트레이서
{
    public static class ExchangeRate
    {
        private static readonly HttpClient _httpClient = new HttpClient();

        public static async Task<double> GetUSDToKRWExchangeRate()
        {
            string result;

            try
            {
                _httpClient.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.89 Whale/1.6.81.13 Safari/537.36");
                result = await _httpClient.GetStringAsync("https://www.google.com/search?q=usd+krw");
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString());
                return 0;
            }

            HtmlDocument htmlDocument = new HtmlDocument();
            htmlDocument.LoadHtml(result);

            double.TryParse(htmlDocument.DocumentNode.SelectSingleNode("//span[@class='DFlfde SwHCTb']")?.Attributes["data-value"]?.Value, out double value);

            return value;
        }
    }
}

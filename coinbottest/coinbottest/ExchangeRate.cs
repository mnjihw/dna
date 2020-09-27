using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using HtmlAgilityPack;

namespace coinbottest
{
    public static class ExchangeRate
    {
        private static readonly HttpClient _httpClient = new HttpClient();

        public static async Task<double> GetUSDExchangeRate()
        {
            string result;

            try
            {
                _httpClient.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.89 Whale/1.6.81.13 Safari/537.36");
                result = await _httpClient.GetStringAsync("https://www.google.com/search?q=usd+krw");
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message + e.StackTrace);
                return 0;
            }

            HtmlDocument htmlDocument = new HtmlDocument();
            htmlDocument.LoadHtml(result);

            double.TryParse(htmlDocument.DocumentNode.SelectSingleNode("//span[@class='DFlfde SwHCTb']")?.Attributes["data-value"]?.Value, out double value);

            return value;
        }
    }
}


/*
 * try
            {
                result = await _httpClient.GetStringAsync("https://finance.naver.com/marketindex/exchangeDetail.nhn?marketindexCd=FX_USDKRW");
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message + e.StackTrace);
                return 0;
            }

            HtmlDocument htmlDocument = new HtmlDocument();
            htmlDocument.LoadHtml(result);

            double.TryParse(htmlDocument.DocumentNode.SelectSingleNode("//option[@class=\"selectbox-default\"]")?.Attributes["value"]?.Value, out double value);

            return value;
 * */

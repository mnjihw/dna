using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.IO.Compression;
using System.IO;
using System.Net.WebSockets;
using System.Diagnostics;
using System.Collections.Concurrent;

namespace coinbottest
{
   
    static class Ex
    {
        public static string ToString2(this DateTime dt)
        {
            return $"{dt.Hour}:{dt.Minute}:{dt.Second}.{dt.Millisecond}";
        }
    }
    class Program
    {

        static async void Func()
        {
            

            //IExchange cryptowatch = Cryptowatch.Instance;
            Bithumb bithumb = Bithumb.Instance;
            //IExchange binance = Binance.Instance;
            Binance binance = Binance.Instance;
            Upbit upbit = Upbit.Instance;
            Gateio gateio = Gateio.Instance;
            Huobi huobi = Huobi.Instance;

            //double usdExchangeRate, usdtExchangeRate;
            //int bithumbPrice = 0, binancePrice;

            var codes = new ConcurrentDictionary<string, string>();
            var pricesByExchange = new ConcurrentDictionary<string, PricesByExchange>();
            var closingPrices = new ConcurrentDictionary<string, double>();

            int count = 0;

            while (true)
            {

                gateio.GetDeterminedClosingPrices(closingPrices);

                foreach (var a in closingPrices)
                    Console.WriteLine(a);
                return;
                
                await Task.WhenAll(new Task[]
                {
                    binance.GetCurrentPrices(pricesByExchange),
                    bithumb.GetCurrentPrices(pricesByExchange),
                    gateio.GetCurrentPrices(pricesByExchange),
                    huobi.GetCurrentPrices(pricesByExchange),
                    upbit.GetCurrentPrices(pricesByExchange),
                });


                foreach (var prices in pricesByExchange)
                {
                    foreach (var prop in typeof(PricesByExchange).GetProperties())
                    {
                        // price.Key코인이름
                        //prop.GetValue(price.Value) 가격
                        switch (prop.Name)
                        {
                            case "Binance":
                                if (prices.Key.StartsWith("BTC"))
                                {

                                }
                                break;
                            case "Bithumb":
                                break;
                            case "Gateio":
                                break;
                            case "Huobi":
                                break;
                            case "Upbit":
                                break;
                        }


                        if (prop.PropertyType == typeof(double) && (double)prop.GetValue(prices.Value) != 0)
                        {
                            //Console.WriteLine(prop.GetValue(price.Value));
                            ++count;
                        }
                    }
                    if (count >= 2)
                    {

                        Console.WriteLine($"카운트: {count} {prices}");
                        
                    }
                    count = 0;
                }
                return;
                await Task.Delay(1000);


            }
            return;



            /*
            var bithumbPostData = new Dictionary<string, string>
            {
                ["order_currency"] = "BTC",
                ["payment_currency"] = "KRW"
            };
            var binancePostData = new Dictionary<string, string>
            {
                ["symbol"] = "BTCUSDT"
            };

            usdExchangeRate = await ExchangeRate.GetUSDExchangeRate();

            while (true)
            {
                jObject = await bithumb.APICall("/public/ticker", bithumbPostData) as JObject;
                if (jObject["status"].ToString() == "0000")
                    bithumbPrice = int.Parse(jObject["data"]["closing_price"].ToString());
                
                usdtExchangeRate = double.Parse((await cryptowatch.APICall("/markets/kraken/usdtusd/price", null))["result"]["price"].ToString());
                jObject = await binance.APICall("/api/v1/ticker/24hr", binancePostData) as JObject;
                
                binancePrice = (int)(double.Parse(jObject["lastPrice"].ToString()) * usdtExchangeRate * usdExchangeRate);
                int difference = Math.Abs(binancePrice - bithumbPrice);
                Console.WriteLine($"빗썸: {bithumbPrice} 바이낸스: {binancePrice} 차이: {difference}({(double)difference / Math.Min(bithumbPrice, binancePrice) * 100:F2}%)");
                
                Thread.Sleep(300);
            }
            
            */


        }

        static void Main(string[] args)
        {
            Task.Run(Func);

            while (true)
                Thread.Sleep(3000);

        }
    }
}

using System;
using System.Linq;
using System.Net.Http;
using System.Threading.Tasks;
using Binance.Net;
using Binance.Net.Enums;
using Binance.Net.Objects.Spot;
using CryptoExchange.Net.Authentication;

namespace BinanceMirrorTradingBot
{
    class Program
    {
        /*
         * 숏을 칠떄: 진입 가격, 사이즈, 레버리지
         */
        static async Task Main()
        {
            var client = new HttpClient();
            client.DefaultRequestHeaders.Add("Accept", "application/json");
            var result = await client.GetStringAsync("https://api.upbit.com/v1/ticker?markets=KRW-ETH");
            Console.WriteLine(result);
            return;
            /*const string apiKey = "FRMb1DTMWBxqFjOHU9NiPJLpCsuvUAc7xUrllfSOcdrdrgrSCuMZBisIWJNUie5x";
            const string secretKey = "xfrh6h5qQOkwotIQssGhThBj3GJKdgBKeHBEPLiBTX7LgZ4gYjDlhhfcqEbQwlKL";

            BinanceClient.SetDefaultOptions(new() { ApiCredentials = new(apiKey, secretKey) });
            BinanceSocketClient.SetDefaultOptions(new() { ApiCredentials = new(apiKey, secretKey) });


            BinanceSocketClient socketClient = new ();
            BinanceClient client = new ();
            var listenKey = (await client.FuturesUsdt.UserStream.StartUserStreamAsync()).Data;

            socketClient.FuturesUsdt.SubscribeToUserDataUpdates(listenKey, null, null, null, e =>
            {
                var data = e.UpdateData;
                var a = $"{data.Price} {data.Side} {data.Quantity} {data.QuantityOfLastFilledTrade} {data.Status} {data.Type}";
                Console.WriteLine(a);
            }, null);


            Console.ReadLine();
*/


            /*   var positions = client.FuturesUsdt.GetPositionInformation(); //포지션 읽어오기

               foreach (var position in positions.Data)
               {
                   if (position.PositionAmount != 0)
                   {
                       var a = $"{position.Symbol} {position.Leverage}x 사이즈: {position.PositionAmount} 진입 가격: {position.EntryPrice}";
                       Console.WriteLine(a);
                   }
               }*/





        }
    }
}

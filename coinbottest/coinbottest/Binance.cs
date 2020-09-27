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
    public sealed class Binance : IExchange
    {
        private static readonly Lazy<Binance> _instanceHolder = new Lazy<Binance>(() => new Binance());
        public static Binance Instance => _instanceHolder.Value;
        private readonly HttpClient _httpClient = new HttpClient();
        private readonly string[] listOfCoins = { "ADA", "ADA/BTC", "ADX/BTC", "AE/BTC", "ALGO", "ALGO/BTC", "ANKR", "ANKR/BTC", "APPC/BTC", "ARDR/BTC", "ARK/BTC", "AST/BTC", "ATOM", "ATOM/BTC", "BAT", "BAT/BTC", "BCD/BTC", "BCHSV", "BCHSV/BTC", "BCN/BTC", "BCPT/BTC", "BLZ/BTC", "BNB", "BNT/BTC", "BTC", "BTG/BTC", "BTS/BTC", "BTT", "BTT/BTC", "CELR", "CHAT/BTC", "CMT/BTC", "COS", "CVC", "CVC/BTC", "DASH", "DASH/BTC", "DCR/BTC", "DENT/BTC", "DGD/BTC", "DNT/BTC", "DOCK", "DOCK/BTC", "DOGE", "DOGE/BTC", "ELF/BTC", "ENG/BTC", "ENJ", "ENJ/BTC", "EOS", "EOS/BTC", "ETC", "ETC/BTC", "ETH", "ETH/BTC", "EVX/BTC", "FTM", "FUN", "GAS/BTC", "GNT/BTC", "GO/BTC", "GRS/BTC", "GTO", "GTO/BTC", "GXS/BTC", "HC/BTC", "HOT/BTC", "ICX", "ICX/BTC", "IOST", "IOST/BTC", "IOTA", "IOTA/BTC", "IOTX/BTC", "KMD/BTC", "KNC/BTC", "LINK", "LINK/BTC", "LOOM/BTC", "LRC/BTC", "LSK/BTC", "LTC", "LTC/BTC", "LUN/BTC", "MANA/BTC", "MCO/BTC", "MFT", "MFT/BTC", "MITH", "MTL", "MTL/BTC", "NANO", "NANO/BTC", "NAS/BTC", "NAV/BTC", "NCASH/BTC", "NEO", "NEO/BTC", "NPXS", "NPXS/BTC", "NULS", "NULS/BTC", "NXS/BTC", "OAX/BTC", "OMG", "OMG/BTC", "ONE", "ONG", "ONT", "ONT/BTC", "OST/BTC", "PAX", "PAX/BTC", "PHX/BTC", "PIVX/BTC", "POLY/BTC", "POWR/BTC", "QKC/BTC", "QLC/BTC", "QSP/BTC", "QTUM", "QTUM/BTC", "RCN/BTC", "RDN/BTC", "REN/BTC", "REP/BTC", "REQ/BTC", "RLC/BTC", "RVN/BTC", "SALT/BTC", "SC/BTC", "SNT/BTC", "STEEM/BTC", "STORJ/BTC", "STORM", "STORM/BTC", "STRAT/BTC", "SYS/BTC", "TFUEL", "THETA", "THETA/BTC", "TNB/BTC", "TNT/BTC", "TOMO", "TRX", "TRX/BTC", "TUSD", "TUSD/BTC", "USDC", "VET", "VET/BTC", "VIA/BTC", "VIB/BTC", "WAN/BTC", "WAVES", "WAVES/BTC", "WPR/BTC", "WTC/BTC", "XEM/BTC", "XLM", "XLM/BTC", "XMR", "XMR/BTC", "XRP", "XRP/BTC", "XVG/BTC", "XZC/BTC", "ZEC", "ZEC/BTC", "ZEN/BTC", "ZIL", "ZIL/BTC", "ZRX", "ZRX/BTC" };
        private readonly string[] namesOfPair = { "BTC", "USDT" };
        private Binance() { }

        public void GetDeterminedClosingPrices(ConcurrentDictionary<string, double> prices)
        {
            Parallel.ForEach(listOfCoins, new ParallelOptions { MaxDegreeOfParallelism = Environment.ProcessorCount }, coin =>
            {
                string result, name;
                JArray arr;
                var now = DateTimeOffset.Now;

                int index = coin.IndexOf('/');

                name = coin.Replace("/", "");

                if(index == -1)
                    name += "USDT";

                try
                {
                    result = _httpClient.GetStringAsync($"https://api.binance.com/api/v1/klines?symbol={name}&interval=1d&startTime={now.AddDays(-1).ToUnixTimeMilliseconds()}").Result;
                    arr = JArray.Parse(result);
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                    return;
                }

                if (arr.Count != 0)
                    prices[coin] = double.Parse(arr[0][2].ToString());

                Thread.Sleep(200);
            });

        }

        public void GetDeterminedClosingPrices2(ConcurrentDictionary<string, double> prices) //1시간봉
        {
            Parallel.ForEach(listOfCoins, new ParallelOptions { MaxDegreeOfParallelism = Environment.ProcessorCount }, coin =>
            {
                string result;
                JArray arr;
                TimeSpan gap, nearestTime = TimeSpan.MaxValue;
                double nearestPrice = double.MaxValue;
                var now = DateTimeOffset.Now;

                try
                {
                    result = _httpClient.GetStringAsync($"https://api.binance.com/api/v1/klines?symbol={coin}&interval=1h&startTime={now.AddDays(-1).ToUnixTimeMilliseconds()}").Result;
                    arr = JArray.Parse(result);
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                    return;
                }

                for (int i = 0; i < arr.Count; ++i)
                {
                    DateTimeOffset time = DateTimeOffset.FromUnixTimeMilliseconds((long)arr[i][0]).ToLocalTime();
                    var today9 = DateTime.Today.AddHours(9);

                    gap = (today9.TimeOfDay - time.TimeOfDay).Duration();

                    if (nearestTime > gap)
                    {
                        nearestTime = gap;
                        nearestPrice = double.Parse(arr[i][2].ToString());
                    }

                    if (time.Hour == 9)
                    {
                        if ((time.Day == now.Day && now.Hour >= 9 && now.Hour <= 23) || (time.Day == now.Day - 1 && now.Hour >= 0 && now.Hour <= 8))
                        {
                            prices[coin] = double.Parse(arr[i][2].ToString());
                            break;
                        }
                    }
                }
                if (!prices.ContainsKey(coin))
                    prices[coin] = nearestPrice;

                Thread.Sleep(200);
            });
           
        }

        public async Task<bool> GetCurrentPrices(ConcurrentDictionary<string, PricesByExchange> prices)
        {
            JArray jArray;

            try
            {
                var result = await _httpClient.GetStringAsync("https://api.binance.com/api/v3/ticker/price");
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
                name = jArray[i]["symbol"].ToString();

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
                prices[name].Binance = double.Parse(jArray[i]["price"].ToString());
            }
            
            return true;
        }
    }
}



/*
 * 
 * using (var ws = new ClientWebSocket())
            {
                var cancellationTokenSource = new CancellationTokenSource(5000);

                Task task;
                try
                {
                    task = ws.ConnectAsync(new Uri("wss://stream.binance.com:9443/ws/!ticker@arr"), cancellationTokenSource.Token);                   
                }
                catch(WebSocketException e)
                {
                    Console.WriteLine(e.Message + e.StackTrace);
                    return;
                }
                while (!task.IsCompleted && !cancellationTokenSource.IsCancellationRequested)
                {
                    await Task.Delay(300);
                }
                if (cancellationTokenSource.IsCancellationRequested)
                {
                    Console.WriteLine("타임아웃");
                    //continue;
                }

                byte[] buf = new byte[16384];

                using (var stream = new MemoryStream())
                {
                    while (ws.State == WebSocketState.Open)
                    {
                        stream.SetLength(0);

                        while (true)
                        {
                            WebSocketReceiveResult response;
                            Task<WebSocketReceiveResult> receiveTask;

                            try
                            {
                                receiveTask = ws.ReceiveAsync(new ArraySegment<byte>(buf), cancellationTokenSource.Token);
                            }
                            catch (WebSocketException e)
                            {
                                Console.WriteLine(e.Message + e.StackTrace);
                                continue;
                            }

                            while (!receiveTask.IsCompleted && !cancellationTokenSource.IsCancellationRequested)
                            {
                                await Task.Delay(300);
                            }

                            response = await receiveTask;

                            if (cancellationTokenSource.IsCancellationRequested)
                            {
                                Console.WriteLine("타임아웃");
                                //continue;
                            }


                            stream.Write(buf, 0, response.Count);

                            if (response.EndOfMessage)
                            {
                                var res = Encoding.UTF8.GetString(stream.ToArray());

                                JArray a = null;

                                a = JArray.Parse(res);


                                for (int i = 0; i < a.Count; ++i)
                                {
                                    Console.WriteLine($"{a[i]["s"]} {a[i]["c"]}");

                                }
                                break;
                            }

                            if (response.MessageType == WebSocketMessageType.Close)
                            {
                                Console.WriteLine("끊겼음");
                                await ws.CloseAsync(WebSocketCloseStatus.NormalClosure, "closed", CancellationToken.None);
                            }
                        }
                    }
                }
                

                
            }
 * 
 * */

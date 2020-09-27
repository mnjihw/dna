using Newtonsoft.Json.Linq;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace 리듬_코인_트레이서
{
    public sealed class Binance : IExchange
    {
        private static readonly Lazy<Binance> _instanceHolder = new Lazy<Binance>(() => new Binance());
        public static Binance Instance => _instanceHolder.Value;
        private readonly HttpClient _httpClient = new HttpClient();
        private readonly string[] namesOfPair = { "BTC", "USDT" };
        private Binance() { }

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
                MessageBox.Show(e.ToString());
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
                    MessageBox.Show(e.ToString());
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
                                MessageBox.Show(e.ToString());
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

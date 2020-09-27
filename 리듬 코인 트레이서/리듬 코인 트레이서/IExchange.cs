using Newtonsoft.Json.Linq;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace 리듬_코인_트레이서
{
    public interface IExchange
    {
        Task<bool> GetCurrentPrices(ConcurrentDictionary<string, PricesByExchange> prices);
    }
}

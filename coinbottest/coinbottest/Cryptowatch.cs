using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;

namespace coinbottest
{
    public sealed class Cryptowatch 
    {
        private static readonly Lazy<Cryptowatch> _instanceHolder = new Lazy<Cryptowatch>(() => new Cryptowatch());
        public static Cryptowatch Instance => _instanceHolder.Value;
        private Cryptowatch() { }
        private readonly HttpClient _httpClient = new HttpClient();
        
        public async Task<JToken> APICall(string endPoint, Dictionary<string, string> parameters) => JObject.Parse(await _httpClient.GetStringAsync($"https://api.cryptowat.ch{endPoint}"));

    }
}

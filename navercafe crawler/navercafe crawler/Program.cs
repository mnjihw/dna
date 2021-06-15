using NaverLogin;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Net;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Numerics;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Text.Json;
using System.Diagnostics;
using System.Linq;
using System.Text.Encodings.Web;


namespace navercafe_crawler
{
    class Program
    {
        private static CookieContainer CookieContainer { get; } = new CookieContainer();
        private static HttpClientHandler HttpClientHandler { get; } = new HttpClientHandler { CookieContainer = CookieContainer };
        private static HttpClient HttpClient { get; } = new HttpClient(HttpClientHandler);
        static async Task<bool> LoginAsync(string id, string password)
        {
            string[] buf;
            string sessionKey, keyName, result;
            BigInteger n, e;


            

            result = await HttpClient.GetStringAsync("https://nid.naver.com/login/ext/keys2.nhn");

            HttpClient.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.182 Safari/537.36 Edg/88.0.705.81");

            using var rsa = new RSACryptoServiceProvider();


            buf = result.Split(',');
            sessionKey = buf[0];
            keyName = buf[1];
            n = BigInteger.Parse("0" + buf[2], NumberStyles.HexNumber);
            e = BigInteger.Parse(buf[3], NumberStyles.HexNumber);
            n = BigInteger.Parse("0" + "bd0f619bec14002a67ce4f65f2e94c9f3bf30d3cc6f2ab73aeac5e4ff36c823d30e346be2cf413d3e9052b20b5ce42fe29004c7e99230f171e8ac0ae67cdcf2c418025e87e3601b81c14047e50ffec23438814364fc3b96a3a96c66e689e3fde8a66020cdccaa4b1524ba18836c5af9b214d2b85221a20a7a70383ee7c5546a7", NumberStyles.HexNumber);

            var rsaKeyInfo = new RSAParameters { Modulus = n.ToByteArray(true, true), Exponent = e.ToByteArray(true, true) }; //실제로는 n이 빅인트임
            //var rsaKeyInfo = new RSAParameters { Modulus = Convert.FromHexString(buf[2]), Exponent = Convert.FromHexString(buf[3])}; //실제로는 n이 빅인트임


            rsa.ImportParameters(rsaKeyInfo);


            var data = Convert.ToChar(sessionKey.Length) + sessionKey + Convert.ToChar(id.Length) + id + Convert.ToChar(password.Length) + password; //완료
            var uuid = Guid.NewGuid();


            var jsonSerializerOptions = new JsonSerializerOptions { Encoder = JavaScriptEncoder.UnsafeRelaxedJsonEscaping };
            var o = JsonSerializer.Serialize(new
            {
                a = $"{uuid}-4",
                b = "1.3.4",
                d = new dynamic[]
                {
                    new
                    {
                        i = "id",
                        b = new
                        {
                            a = new[] { $"0,{id}" }
                        },
                        d = id,
                        e = false,
                        f = false,
                    },
                    new
                    {
                        i = password,
                        e = true,
                        f = false,
                    }
                },
                h = "1f",
                i = new
                {
                    a = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.182 Safari/537.36 Edg/88.0.705.81"
                }
            }, jsonSerializerOptions); //완료

            var encData = LZString.CompressToEncodedURIComponent(o); //완료
            var bvsd = JsonSerializer.Serialize(new
            {
                uuid = $"{uuid}", 
                encData
            }, jsonSerializerOptions); //완료


            //Convert.ToHexString(rsa.Encrypt(Encoding.UTF8.GetBytes(data), false)).ToLower(),
            var arr = rsa.Encrypt(Encoding.UTF8.GetBytes(data), false).Reverse().ToArray();
            


            //arr = arr.Reverse().ToArray();

            //Array.Reverse(arr);

            /* var postData = new Dictionary<string, string>
             {
                 ["localechange"] = "",
                 ["dynamicKey"] = "",
                 ["encpw"] = Convert.ToHexString(arr).ToLower(),
                 ["enctp"] = "1",
                 ["svctype"] = "1",
                 ["smart_LEVEL"] = "1",
                 ["bvsd"] = bvsd,
                 ["encnm"] = keyName,
                 ["locale"] = "ko_KR",
                 ["url"] = "https://www.naver.com",
                 ["id"] = "",
                 ["pw"] = "",
                 ["nvlong"] = "on",
             };*/


            var postData = new Dictionary<string, string>
            {
                ["encnm"] = keyName,
                ["encpw"] = Convert.ToHexString(arr).ToLower(),
                //["encpw"] = Convert.ToHexString(arr.Reverse().ToArray()).ToLower(),
                //["encpw"] = Convert.ToHexString(arr.Reverse().ToArray()).ToLower(),
                ["bvsd"] = bvsd,
            };
            
            //bvsd, encData 정확히 일치

            using var formUrlEncodedContent = new FormUrlEncodedContent(postData);
            
            formUrlEncodedContent.Headers.ContentType = MediaTypeHeaderValue.Parse("application/x-www-form-urlencoded");
            result = await (await HttpClient.PostAsync("https://nid.naver.com/nidlogin.login", formUrlEncodedContent)).Content.ReadAsStringAsync();

            Console.WriteLine(result);

            if (result.Contains("https://nid.naver.com/login/sso/finalize.nhn?url"))
                return true;
            else
                return false;
        }

        static async Task Main()
        {
            await LoginAsync("memscan", "wbeltmtjq2");

        }
    }
}

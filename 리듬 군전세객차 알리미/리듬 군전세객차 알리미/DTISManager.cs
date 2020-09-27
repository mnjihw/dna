using HtmlAgilityPack;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

//동규

namespace 리듬_군전세객차_알리미
{
    public sealed class DTISManager
    {
        private static readonly Lazy<DTISManager> _instanceHolder = new Lazy<DTISManager>(() => new DTISManager());
        public static DTISManager Instance => _instanceHolder.Value;

        public enum MilitaryType
        {
            MinistryOfNationalDefense = 1, MarineCorps = 4, Army, Navy, AirForce
        }
        public HashSet<string> queryResults = new HashSet<string>();
        public readonly (HashSet<string> desiredStation, int price)[] desiredStationsDatas = { (new HashSet<string> { "서울역", "대전역" }, 24000), (new HashSet<string> { "서울역", "김천구미역" }, 35000), (new HashSet<string> { "서울역", "동대구역" }, 43500), (new HashSet<string> { "동대구역", "오송역" }, 25000), (new HashSet<string> { "동대구역", "수원역" }, 30000), (new HashSet<string> { "동대구역", "천안아산역" }, 30000), (new HashSet<string> { "동대구역", "영등포역" }, 35000), (new HashSet<string> { "동대구역", "광명역" }, 41000), (new HashSet<string> { default, default}, 0) };

        public readonly List<(HashSet<string> desiredStation, int price)> reservedTrains = new List<(HashSet<string>, int)>();
        private static readonly CookieContainer _cookieContainer = new CookieContainer();
        private static readonly HttpClientHandler _httpClientHandler = new HttpClientHandler { CookieContainer = _cookieContainer };
        private static readonly HttpClient _httpClient = new HttpClient(_httpClientHandler) { Timeout = TimeSpan.FromSeconds(15) };
        private string Id { get; set; }
        private string Password { get; set; }

        private DTISManager() { }
  

        public async Task<bool> Login(MilitaryType militaryType, string id, string password)
        {
            string result = default;

            var postData = new Dictionary<string, string>
            {
                ["military"] = ((int)militaryType).ToString(),
                ["sId"] = id,
                ["sPw"] = "",
                ["sEncId"] = EncryptString(id),
                ["sEncPw"] = EncryptString(password)
            };

            InitializeDefaultHeaders(_httpClient);

            await TryUntilSuccess(new Func<Task>(async () =>
            {
                await _httpClient.GetStringAsync("http://www.dtis.mil.kr/internet/dtis_rail/index.public.jsp");
            }));

            _httpClient.DefaultRequestHeaders.Add("Referer", "http://www.dtis.mil.kr/internet/dtis_rail/index.public.jsp");
            _httpClient.DefaultRequestHeaders.Add("Origin", "http://www.dtis.mil.kr");
            _httpClient.DefaultRequestHeaders.TryAddWithoutValidation("Content-Type", "application/x-www-form-urlencoded");

            await TryUntilSuccess(new Func<Task>(async () =>
            {
                result = await (await _httpClient.PostAsync("http://www.dtis.mil.kr/internet/dtis_rail/index.public.jsp", new FormUrlEncodedContent(postData))).Content.ReadAsStringAsync();
            }));

            if (result.Contains("비밀번호가 틀렸습니다") || result.Contains("패스워드가 올바르지 않습니다"))
                return false;
            Id = id;
            Password = password;
            return true;
        }

        public async Task<List<Dictionary<string, string>>> GetTrainsInformation((string departingStation, string arrivingStation) desiredSection, bool isRoundTrip, GroupBox groupBox, string goingDate, string comingDate)
        {
            var availableTrains = new List<Dictionary<string, string>>();
            string result = default;

            var postData = new Dictionary<string, string>
            {
                ["fromDt"] = DateTime.Now.ToShortDateString(),
                ["toDt"] = DateTime.Now.AddDays(3).ToShortDateString()
            };

            InitializeDefaultHeaders(_httpClient);
            _httpClient.DefaultRequestHeaders.Add("Referer", "http://www.dtis.mil.kr/internet/dtis_rail/WSCWWMLPTEmbrktnAppMgtTF.public.jsp");
            _httpClient.DefaultRequestHeaders.Add("Origin", "http://www.dtis.mil.kr");
            _httpClient.DefaultRequestHeaders.TryAddWithoutValidation("Content-Type", "application/x-www-form-urlencoded");

            await TryUntilSuccess(new Func<Task>(async () =>
            {
                result = await (await _httpClient.PostAsync("http://www.dtis.mil.kr/internet/dtis_rail/WSCWWMLPTEmbrktnAppMgtTF.public.jsp", new FormUrlEncodedContent(postData))).Content.ReadAsStringAsync();
                
                if (result.Contains("로그인 후 사용해 주시기 바랍니다"))
                {
                    await Login(MilitaryType.AirForce, Id, Password);
                    throw new Exception("Logged in");
                }
            }));

            HtmlDocument htmlDocument = new HtmlDocument();

            htmlDocument.LoadHtml(result);
            var rowNodes = htmlDocument.DocumentNode.SelectNodes("//div[@id='tblContent']/table/*");

            foreach (var rowNode in rowNodes)
            {
                var nodes = rowNode.SelectNodes("./td[@class='request_td font_011']");
                var operatingDate = nodes[0].InnerText.Trim();

                var tuple = (nodes[5].InnerText, nodes[6].InnerText);

                if (operatingDate == goingDate && tuple == desiredSection || (isRoundTrip && operatingDate == comingDate && tuple == (desiredSection.arrivingStation, desiredSection.departingStation)))
                {
                    var node = rowNode.SelectSingleNode("./td[@class='request_td font_012']/a");
                    var tag = GetTagOfSelectedRadioButton(groupBox);

                    if (node != null && (tag == "0" && nodes[7].InnerText != "11:00") || (tag == "1" && nodes[7].InnerText == "11:00") || (tag == "2" && nodes[7].InnerText == "17:45") || tag == "3")
                    {
                        var value = node.Attributes["onclick"].Value;
                        int startIndex = value.IndexOf("RmndrSeatRsvtn\',\'") + "RmndrSeatRsvtn\',\'".Length;
                        var splitString = value.Substring(startIndex, value.IndexOf('\'', startIndex) - startIndex).Split(new[] { "||" }, StringSplitOptions.None);

                        availableTrains.Add(new Dictionary<string, string>
                        {
                            ["p_dailymvmgtno"] = splitString[0],
                            ["p_mvdt"] = splitString[1],
                            ["operatingDate"] = operatingDate,
                            ["trainName"] = nodes[3].InnerText.Trim(),
                            ["departingTime"] = nodes[7].InnerText,
                            ["arrivingTime"] = nodes[8].InnerText
                        });
                    }
                }
            }

            return availableTrains;
        }

        public async Task CheckForAvailableSeats(Dictionary<string, string> trainInfo)
        {
            string result = default;

            InitializeDefaultHeaders(_httpClient);
            _httpClient.DefaultRequestHeaders.Add("Referer", "http://www.dtis.mil.kr/internet/dtis_rail/WSCWWMLPTEmbrktnAppMgtTF.public.jsp");

            await TryUntilSuccess(new Func<Task>(async () =>
            {
                result = await _httpClient.GetStringAsync($"http://www.dtis.mil.kr/internet/dtis_rail/rail.process.public.jsp?flag=proc&procType=RmndrSeatRsvtn&p_dailymvmgtno={trainInfo["p_dailymvmgtno"]}&p_mvdt={trainInfo["p_mvdt"]}");

                if (result.Contains("로그인 후 사용해 주시기 바랍니다"))
                {
                    await Login(MilitaryType.AirForce, Id, Password);
                    throw new Exception("Logged in");
                }
            }));

            int startIndex = result.IndexOf("\"", result.IndexOf("returnValue")) + "\"".Length;
            result = result.Substring(startIndex, result.IndexOf("\"", startIndex) - startIndex);

            var splitFullString = result.Split(new [] { "//" }, StringSplitOptions.None);
            foreach(var splitString in splitFullString)            

            {
                if (!string.IsNullOrEmpty(splitString))
                {
                    Console.WriteLine($"[{DateTime.Now:HH:mm:ss}] {splitString}");
                    var splitSubString = splitString.Split('|');
                    var departureStation = splitSubString[5];
                    var arrivalStation = splitSubString[6];
                    var section = new HashSet<string> { departureStation, arrivalStation };
                    (HashSet<string> desiredStation, int price) tuple = default;
                   
                    if (desiredStationsDatas.Any(desiredStationsData =>
                    {
                        if (desiredStationsData.desiredStation.SetEquals(section))
                        {
                            tuple = (section, desiredStationsData.price);

                            if (reservedTrains.Any(reservedTrain => !reservedTrain.desiredStation.SequenceEqual(tuple.desiredStation) && reservedTrain.price < tuple.price) || reservedTrains.Count == 0)
                                return true;
                        }
                        return false;
                    }))
                    {
                        var ssitecd = splitSubString[0];
                        var esitecd = splitSubString[1];
                        var trainno = splitSubString[2];
                        var seatno = splitSubString[4];
                        var trainnocd = splitSubString[7];

                        var postData = new Dictionary<string, string>
                        {
                            ["flag"] = "rsvtnForm",
                            ["proc_catg"] = "C",
                            ["i_trn_mvdt"] = trainInfo["p_mvdt"],
                            ["i_trn_dailymvmgtno"] = trainInfo["p_dailymvmgtno"],
                            ["i_trn_trainno"] = trainno,
                            ["i_trn_seatno"] = seatno,
                            ["i_trn_ssitecd"] = ssitecd,
                            ["i_trn_esitecd"] = esitecd,
                            ["i_trn_trainnocd"] = trainnocd
                        };

                        InitializeDefaultHeaders(_httpClient);
                        _httpClient.DefaultRequestHeaders.Add("Referer", "http://www.dtis.mil.kr/internet/dtis_rail/WSCWWMLPTEmbrktnAppMgtTF.public.jsp");
                        _httpClient.DefaultRequestHeaders.Add("Origin", "http://www.dtis.mil.kr");
                        _httpClient.DefaultRequestHeaders.TryAddWithoutValidation("Content-Type", "application/x-www-form-urlencoded");

                        await TryUntilSuccess(new Func<Task>(async () =>
                        {
                            result = await (await _httpClient.PostAsync("http://www.dtis.mil.kr/internet/dtis_rail/rail.process.public.jsp", new FormUrlEncodedContent(postData))).Content.ReadAsStringAsync();

                            if(result.Contains("정상적으로 예약하였습니다"))
                            {
                                reservedTrains.Add(tuple);
                                queryResults.Add($"[{trainInfo["operatingDate"]} {trainInfo["trainName"]}] {departureStation} ➡ {arrivalStation} 예약 완료");
                            }
                            else
                            {
                                Console.WriteLine(result);
                                File.WriteAllText(@"C:\Users\mnjihw\Desktop\로그.txt", result);
                                return;
                            }
                        }));
                    }
                } 
            }
        }

        private async Task TryUntilSuccess(Func<Task> action)
        {
            while (true)
            {
                try
                {
                    await action();
                    break;
                }
                catch (Exception)
                {
                    Console.WriteLine("시도중...");
                    await Task.Delay(5000);
                }
            }
        }

        private string EncryptString(string input)
        {
            var sb = new StringBuilder(64);

            for (int i = 0; i < input.Length; ++i)
                sb.Append(((int)input[i]).ToString("x2"));

            return sb.ToString();
        }

        private void InitializeDefaultHeaders(HttpClient httpClient)
        {
            httpClient.DefaultRequestHeaders.Clear();
            httpClient.DefaultRequestHeaders.Add("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3");
            httpClient.DefaultRequestHeaders.Add("Accept-Encoding", "gzip, deflate");
            httpClient.DefaultRequestHeaders.Add("Accept-Language", "ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7");
            httpClient.DefaultRequestHeaders.Add("Cache-Control", "max-age=0");
            httpClient.DefaultRequestHeaders.Connection.Add("keep-alive");
            httpClient.DefaultRequestHeaders.ExpectContinue = false;
            httpClient.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.89 Whale/1.6.81.16 Safari/537.36");
            httpClient.DefaultRequestHeaders.Add("Upgrade-Insecure-Requests", "1");
            httpClient.DefaultRequestHeaders.Add("Host", "www.dtis.mil.kr");
        }

        private string GetTagOfSelectedRadioButton(GroupBox groupBox)
        { 
            foreach(var child in (groupBox.Content as StackPanel).Children)
            {
                if(child is RadioButton radioButton && radioButton.IsChecked.Value)
                {
                    return radioButton.Tag.ToString();
                }
            }
            return default;
        }
    }
}

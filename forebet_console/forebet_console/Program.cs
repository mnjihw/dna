using HtmlAgilityPack;
using System;
using System.Net.Http;
using System.Threading.Tasks;
using System.Data;
using System.Linq;
using System.Globalization;
using System.Text.Json;
using System.Collections.Generic;
using System.IO;
using Telegram.Bot;
using System.Data.Common;

namespace forebet_console
{
    class Program
    {
        private static HttpClient HttpClient { get; } = new HttpClient();
        private static Dictionary<string, MatchData> TotalMatchDatas { get; } = new Dictionary<string, MatchData>();
        static async Task Main()
        {

            using var dd = new MyDbContext();
            

            var random = new Random();
            var matchDatasFromForebet = new List<MatchData>();
            
            var htmlDocument = new HtmlDocument();

#if a
            const string serverIP = "127.0.0.1";
            const int port = 3306;
            const string databaseName = "suggestion";
            const string userName = "suggestion";
            const string password = "SIsEQe85rekASOfar6mAB115qAf82u";


            using var db = DBConnection.Instance;
            if (!await db.ConnectAsync(serverIP, port, databaseName, userName, password))
            {
                Console.WriteLine("연결 실패!");
                return;
            }

            using var sqlExecutor = new SqlExecutor(db.SqlConnection);
#endif


            int blockedCount = 0;
            try
            {
                while (true)
                {
                    Console.WriteLine($"[{DateTime.Now.ToDatebaseDateTime()}] 크롤링 시작");
                    matchDatasFromForebet.Clear();

                    string result = default;
                    try
                    { 
                        result = await HttpClient.GetStringAsync("https://www.forebet.com/en/football-predictions");
                    }
                    catch(HttpRequestException)
                    {
                        await Task.Delay(TimeSpan.FromSeconds(10));
                        continue;
                    }
                    htmlDocument.LoadHtml(result);
                    var nodes = htmlDocument.DocumentNode.SelectNodes("//table[@class='schema tblen']/tr[starts-with(@class, 'tr')]");
                    if(nodes == null)
                    {
                        await Task.Delay(1000 + random.Next() % 2000);
                        continue;
                    }

                    foreach (var node in nodes)
                    {
                        var oddsNode = node.SelectSingleNode("td[@class='bigOnly odd']/div[@class='haodd']");
                        var data = new MatchData
                        {
                            HomeTeam = node.SelectSingleNode("td[@class='tnms']/div/a/span[@class='homeTeam']/span").InnerText,
                            AwayTeam = node.SelectSingleNode("td[@class='tnms']/div/a/span[@class='awayTeam']/span").InnerText,
                            StartDate = DateTime.Parse(node.SelectSingleNode("td[@class='tnms']/div/time[@itemprop='startDate']").InnerText, CultureInfo.GetCultureInfo("en-GB")),
                            HomeOdds = double.TryParse(oddsNode.SelectSingleNode("span[1]").InnerText, out var odds) ? odds : default,
                            DrawOdds = double.TryParse(oddsNode.SelectSingleNode("span[2]").InnerText, out odds) ? odds : default,
                            AwayOdds = double.TryParse(oddsNode.SelectSingleNode("span[3]").InnerText, out odds) ? odds : default
                        };
                        matchDatasFromForebet.Add(data);
                    }
                    int index = 11;
                    do
                    {
                        try
                        {
                            while (string.IsNullOrWhiteSpace(result = await HttpClient.GetStringAsync($"https://www.forebet.com/scripts/getrs.php?ln=en&tp=1x2&in={index}&ord=0")))
                            {
                                await Task.Delay(TimeSpan.FromMinutes(5)); //빈 문자열 리턴되면 5분 쉬고 계속 시도
                                Console.WriteLine($"[{DateTime.Now.ToDatebaseDateTime()}] {++blockedCount}번 차단됨");
                            }
                        }
                        catch(HttpRequestException)
                        {
                            await Task.Delay(TimeSpan.FromSeconds(10));
                            continue;
                        }

                        JsonDocument json = default;
                        for (int i = 0; i < 3; ++i)
                        {
                            try
                            {
                                json = JsonDocument.Parse(result);
                            }
                            catch (JsonException)
                            {
                                result = result.Remove(result.Length - 1);
                            }
                        }
                        if(json == null)
                        {
                            await Task.Delay(1000 + random.Next() % 2000);
                            continue;
                        }


                        foreach (var match in json.RootElement.EnumerateArray())
                        {
                            var data = new MatchData
                            {
                                HomeTeam = match.GetProperty("HOST_NAME").GetString(),
                                AwayTeam = match.GetProperty("GUEST_NAME").GetString(),
                                StartDate = DateTime.Parse(match.GetProperty("DATE_BAH").GetString()),
                                HomeOdds = double.TryParse(match.GetProperty("best_odd_1").GetString(), out var odds) ? odds : default,
                                DrawOdds = double.TryParse(match.GetProperty("best_odd_X").GetString(), out odds) ? odds : default,
                                AwayOdds = double.TryParse(match.GetProperty("best_odd_2").GetString(), out odds) ? odds : default
                            };
                            matchDatasFromForebet.Add(data);
                        }
                        await Task.Delay(1000 + random.Next() % 2000);

                        try
                        {
                            result = await HttpClient.GetStringAsync($"https://www.forebet.com/scripts/getrs.php?ln=en&tp=1x2&in={++index}&test=1");
                        }
                        catch(HttpRequestException)
                        {
                            await Task.Delay(TimeSpan.FromSeconds(10));
                            continue;
                        }
                        if (string.IsNullOrWhiteSpace(result))
                        {
                            await Task.Delay(TimeSpan.FromMinutes(5));
                            Console.WriteLine($"[{DateTime.Now.ToDatebaseDateTime()}] {++blockedCount}번 차단됨");
                        }
                        await Task.Delay(1000 + random.Next() % 2000);

                    } while (result == "1");
                    Console.WriteLine($"[{DateTime.Now.ToDatebaseDateTime()}] 크롤링 끝");

                    List<MatchData> matchDatasFromDatabase = default;
                    try
                    {
                        matchDatasFromDatabase = dd.MatchDatas.ToList();
                    }
                    catch(Exception e)
                    {
                        await Logger.SendTelegramMessageAsync(e.ToString());
                    }
#if a
                    var matchDatasFromDatabase = await sqlExecutor.GetMatchDatasAsync();
#endif

                    foreach (var matchData in matchDatasFromDatabase)
                        TotalMatchDatas[$"{matchData.HomeTeam}-{matchData.AwayTeam}"] = matchData; 
                    
                    
                    var intersectedDatas = matchDatasFromForebet.Intersect(matchDatasFromDatabase, new MatchDataComparer()); //first second의 element가 같으면 first의 element를 리턴함
                    int updatedCount = 0;

                   
                    var list = new List<MatchData>();

                    foreach (var intersectedData in intersectedDatas)
                    {
                        var title = $"{intersectedData.HomeTeam}-{intersectedData.AwayTeam}";
                        var oldData = TotalMatchDatas[title];

                        if (intersectedData.HomeOdds != oldData.HomeOdds || intersectedData.DrawOdds != oldData.DrawOdds || intersectedData.AwayOdds != oldData.AwayOdds) //데이터 변동 있으면 update
                        {
                            list.Add(intersectedData);
                            ++updatedCount;
                        }
                    }
                    try
                    {
                        //await dd.MatchDatas.AddRangeAsync(list);
                        //dd.MatchDatas.Where(matchData => matchData.) 아몰랑 여기 하다말음 밑에 인서트랑
                        await dd.SaveChangesAsync();
                    }
                    catch(Exception e)
                    {
                        
                    }
#if a
                    await sqlExecutor.UpdateMatchDatas(list);
#endif
                    

                    Console.WriteLine($"[{DateTime.Now.ToDatebaseDateTime()}] {updatedCount}개 UPDATE 완료");


                    var newDatas = matchDatasFromForebet.Except(matchDatasFromDatabase, new MatchDataComparer()).ToList();

#if a
                    await sqlExecutor.InsertMatchDatas(newDatas);
#endif
                    try
                    {
                        await dd.MatchDatas.AddRangeAsync(newDatas);
                        await dd.SaveChangesAsync();
                    }
                    catch (Exception e)
                    {
                        await Logger.SendTelegramMessageAsync(e.ToString());
                    }

                    int insertedCount = newDatas.Count;

                   
                    Console.WriteLine($"[{DateTime.Now.ToDatebaseDateTime()}] {insertedCount}개 INSERT 완료 3분 쉼");
                    
                    await Task.Delay(TimeSpan.FromMinutes(3));
                    
                }

            }
            catch(Exception e)
            {
                await Logger.SendTelegramMessageAsync(e.ToString());
            }
        }
    }
}

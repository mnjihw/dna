﻿using HtmlAgilityPack;
using System;
using System.Net.Http;
using System.Threading.Tasks;
using System.Web;
using MySql.Data.MySqlClient;
using System.Data;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Globalization;
using System.Text.Json;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Data.Common;

namespace forebet_console
{
    class Program
    {
        private static HttpClient HttpClient { get; } = new HttpClient();
        private static Dictionary<string, MatchData> Dict { get; } = new Dictionary<string, MatchData>();
        static async Task Main()
        {
            var random = new Random();
            var matchDataFromForebet = new List<MatchData>();
            var matchDataFromDatabase = new List<MatchData>();
            var htmlDocument = new HtmlDocument();
            using var connection = new MySqlConnection("Server=127.0.0.1;Port=3306;Database=test;Uid=root;Password=flema0288~"); //비밀번호 적혀있음 조심
            
            var command = connection.CreateCommand();
            await connection.OpenAsync();
            command.CommandTimeout = 500;


            int blockedCount = 0;
            try
            {
                while (true)
                {
                    Console.WriteLine($"{DateTime.Now.FormatAsDatebaseDateTime()} 크롤링 시작");
                    matchDataFromForebet.Clear();
                    matchDataFromDatabase.Clear();

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
                        matchDataFromForebet.Add(data);
                    }
                    int index = 11;
                    do
                    {

                        while (string.IsNullOrWhiteSpace(result = await HttpClient.GetStringAsync($"https://www.forebet.com/scripts/getrs.php?ln=en&tp=1x2&in={index}&ord=0")))
                        {
                            await Task.Delay(TimeSpan.FromMinutes(5)); //빈 문자열 리턴되면 5분 쉬고 계속 시도
                            Console.WriteLine($"{DateTime.Now.FormatAsDatebaseDateTime()} {++blockedCount}번 차단됨");
                        }

                        JsonDocument json = default;
                        try
                        {
                            json = JsonDocument.Parse(result);
                        }
                        catch (JsonException e) 
                        {
                            Console.WriteLine($"{DateTime.Now.FormatAsDatebaseDateTime()} {e}");
                            var a = result;
                            Console.WriteLine(a);
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
                            matchDataFromForebet.Add(data);
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
                            Console.WriteLine($"{DateTime.Now.FormatAsDatebaseDateTime()} {++blockedCount}번 차단됨");
                        }
                        await Task.Delay(1000 + random.Next() % 2000);

                    } while (result == "1");
                    Console.WriteLine($"{DateTime.Now.FormatAsDatebaseDateTime()} 크롤링 끝");


                    command.CommandText = $"SELECT home_title, away_title, home_odds, draw_odds, away_odds from sport_match";
                    using var reader = await command.ExecuteReaderAsync();
                   
                    while (await reader.ReadAsync())
                    {
                        var matchData = new MatchData
                        {
                            HomeTeam = reader.GetString("home_title"),
                            AwayTeam = reader.GetString("away_title"),
                            HomeOdds = double.Parse(reader.GetString("home_odds")),
                            DrawOdds = double.Parse(reader.GetString("draw_odds")),
                            AwayOdds = double.Parse(reader.GetString("away_odds")),
                            //StartDate = 필요없을듯
                        };

                        matchDataFromDatabase.Add(matchData);
                        Dict[$"{matchData.HomeTeam}-{matchData.AwayTeam}"] = matchData;
                    }
                    await reader.CloseAsync();
                    
                    var intersectedData = matchDataFromForebet.Intersect(matchDataFromDatabase, new MatchDataComparer()); //first second의 element가 같으면 first의 element를 리턴함
                    
                    command.Parameters.Clear();
                    command.Parameters.Add("@title", MySqlDbType.VarChar);
                    command.Parameters.Add("@homeOdds", MySqlDbType.VarChar);
                    command.Parameters.Add("@drawOdds", MySqlDbType.VarChar);
                    command.Parameters.Add("@awayOdds", MySqlDbType.VarChar);
                    command.Parameters.Add("@registrationDate", MySqlDbType.DateTime);

                    command.CommandText = "UPDATE sport_match SET home_odds = @homeOdds, draw_odds = @drawOdds, away_odds = @awayOdds, registration_date = @registrationDate WHERE title = @title";
                    await command.PrepareAsync();
                    var transaction = await connection.BeginTransactionAsync();
                    command.Transaction = transaction;


                    int updatedCount = 0;

                    foreach (var changedData in intersectedData)
                    {
                        var title = $"{changedData.HomeTeam}-{changedData.AwayTeam}";
                        var oldData = Dict[title];

                        if(changedData.HomeOdds != oldData.HomeOdds || changedData.DrawOdds != oldData.DrawOdds || changedData.AwayOdds != oldData.AwayOdds) //데이터 변동 있으면 update
                        {
                            command.Parameters["@title"].Value = title;
                            command.Parameters["@homeOdds"].Value = changedData.HomeOdds;
                            command.Parameters["@drawOdds"].Value = changedData.DrawOdds;
                            command.Parameters["@awayOdds"].Value = changedData.AwayOdds;
                            command.Parameters["@registrationDate"].Value = DateTime.Now.FormatAsDatebaseDateTime();
                            await command.ExecuteNonQueryAsync();
                            ++updatedCount;
                            Console.WriteLine($"{DateTime.Now.FormatAsDatebaseDateTime()} {oldData}->{changedData} UPDATE함");
                        }
                    }

                    await transaction.CommitAsync();
                    Console.WriteLine($"{DateTime.Now.FormatAsDatebaseDateTime()} {updatedCount}개 UPDATE 완료");


                    var newData = matchDataFromForebet.Except(matchDataFromDatabase, new MatchDataComparer());

                    command.Parameters.Clear();
                    command.Parameters.Add("@title", MySqlDbType.VarChar);
                    command.Parameters.Add("@homeTeam", MySqlDbType.VarChar);
                    command.Parameters.Add("@awayTeam", MySqlDbType.VarChar);
                    command.Parameters.Add("@homeOdds", MySqlDbType.VarChar);
                    command.Parameters.Add("@drawOdds", MySqlDbType.VarChar);
                    command.Parameters.Add("@awayOdds", MySqlDbType.VarChar);
                    command.Parameters.Add("@startDate", MySqlDbType.DateTime);

                    command.CommandText = $"INSERT INTO sport_match (title, status, home_title, home_odds, draw_odds, away_title, away_odds, date_start, source, registration_date, sport_srl) VALUES(@title, 'Wait', @homeTeam, @homeOdds, @drawOdds, @awayTeam, @awayOdds, @startDate, 'Forebet', '{DateTime.Now.FormatAsDatebaseDateTime()}', {1})";
                    await command.PrepareAsync();
                    transaction = await connection.BeginTransactionAsync();
                    command.Transaction = transaction;

                    int insertedCount = 0;
                    foreach(var data in newData)
                    {
                        
                        command.Parameters["@title"].Value = $"{data.HomeTeam}-{data.AwayTeam}";
                        command.Parameters["@homeTeam"].Value = data.HomeTeam;
                        command.Parameters["@awayTeam"].Value = data.AwayTeam;
                        command.Parameters["@homeOdds"].Value = data.HomeOdds;
                        command.Parameters["@drawOdds"].Value = data.DrawOdds;
                        command.Parameters["@awayOdds"].Value = data.AwayOdds;
                        command.Parameters["@startdate"].Value = data.StartDate.FormatAsDatebaseDateTime();


                        if (await command.ExecuteNonQueryAsync() == 0)
                        {
                            Console.WriteLine($"{DateTime.Now.FormatAsDatebaseDateTime()} {data} 머냐 인서트 실패");
                        }

                        ++insertedCount;
                    }
                    await transaction.CommitAsync();
                    Console.WriteLine($"{DateTime.Now.FormatAsDatebaseDateTime()} {insertedCount}개 INSERT 완료 3분 쉼");
                    
                    await Task.Delay(TimeSpan.FromMinutes(3));
                    
                }

            }
            catch(Exception e)
            {
                 Console.WriteLine(e);
            }
        }
    }
}

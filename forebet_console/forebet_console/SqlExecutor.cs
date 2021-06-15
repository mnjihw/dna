using MySqlConnector;
using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace forebet_console
{
    public class SqlExecutor : IDisposable
    {
        public MySqlCommand SqlCommand { get; set; }
        private bool DisposedValue { get; set; }

        public SqlExecutor(MySqlConnection sqlConnection)
        {
            SqlCommand = sqlConnection.CreateCommand();
            SqlCommand.Connection = sqlConnection;
            SqlCommand.CommandTimeout = 500;
        }


        private async Task BeginTransactionAsync()
        {
            SqlCommand.Transaction = await SqlCommand.Connection.BeginTransactionAsync();
        }

        private async Task CommitAsync()
        {
            await SqlCommand.Transaction.CommitAsync();
        }
        private async Task RollbackAsync()
        {
            await SqlCommand.Transaction.RollbackAsync();
        }

        private async Task DisposeTransactionAsync()
        {
            await SqlCommand.Transaction.DisposeAsync();
        }


        public async Task<List<MatchData>> GetMatchDatasAsync()
        {
            var list = new List<MatchData>();

            SqlCommand.CommandText = $"SELECT home_title, away_title, home_odds, draw_odds, away_odds FROM sport_match";

            try
            {
                await using var reader = await SqlCommand.ExecuteReaderAsync();
                while (await reader.ReadAsync())
                {
                    var matchData = new MatchData
                    {
                        HomeTeam = reader.GetString("home_title"),
                        AwayTeam = reader.GetString("away_title"),
                        HomeOdds = double.Parse(reader.GetString("home_odds")),
                        DrawOdds = double.Parse(reader.GetString("draw_odds")),
                        AwayOdds = double.Parse(reader.GetString("away_odds")),
                    };

                    list.Add(matchData);
                    
                }
            }
            catch (Exception e)
            {
                await Logger.SendTelegramMessageAsync(e.ToString());
            }
            return list;
        }

        public async Task<bool> UpdateMatchDatas(List<MatchData> list)
        {
            SqlCommand.Parameters.Clear();
            SqlCommand.Parameters.Add("@title", MySqlDbType.VarChar, 200);
            SqlCommand.Parameters.Add("@homeOdds", MySqlDbType.VarChar, 20);
            SqlCommand.Parameters.Add("@drawOdds", MySqlDbType.VarChar, 20);
            SqlCommand.Parameters.Add("@awayOdds", MySqlDbType.VarChar, 20);
            SqlCommand.Parameters.Add("@registrationDate", MySqlDbType.DateTime);

            SqlCommand.CommandText = "UPDATE sport_match SET home_odds = @homeOdds, draw_odds = @drawOdds, away_odds = @awayOdds, registration_date = @registrationDate WHERE title = @title";

            await SqlCommand.PrepareAsync();
            await BeginTransactionAsync();
        
            try
            {
                for (int i = 0; i < list.Count; ++i)
                {
                    SqlCommand.Parameters["@title"].Value = $"{list[i].HomeTeam}-{list[i].AwayTeam}";
                    SqlCommand.Parameters["@homeOdds"].Value = list[i].HomeOdds;
                    SqlCommand.Parameters["@drawOdds"].Value = list[i].DrawOdds;
                    SqlCommand.Parameters["@awayOdds"].Value = list[i].AwayOdds;
                    SqlCommand.Parameters["@registrationDate"].Value = DateTime.Now.ToDatebaseDateTime();

                    await SqlCommand.ExecuteNonQueryAsync();
                }
                await CommitAsync();
                await DisposeTransactionAsync();
            }
            catch (Exception e)
            {
                try
                {
                    await Logger.SendTelegramMessageAsync(e.ToString());
                    await RollbackAsync();
                }
                catch (Exception e2)
                {
                    await Logger.SendTelegramMessageAsync(e2.ToString());
                }
                return false;
            }
            return true;

        }

        public async Task<bool> InsertMatchDatas(List<MatchData> matchDatas)
        {
            SqlCommand.Parameters.Clear();
            SqlCommand.Parameters.Add("@title", MySqlDbType.VarChar, 200);
            SqlCommand.Parameters.Add("@homeTeam", MySqlDbType.VarChar, 200);
            SqlCommand.Parameters.Add("@awayTeam", MySqlDbType.VarChar, 200);
            SqlCommand.Parameters.Add("@homeOdds", MySqlDbType.VarChar, 20);
            SqlCommand.Parameters.Add("@drawOdds", MySqlDbType.VarChar, 20);
            SqlCommand.Parameters.Add("@awayOdds", MySqlDbType.VarChar, 20);
            SqlCommand.Parameters.Add("@startDate", MySqlDbType.DateTime);

            SqlCommand.CommandText = $"INSERT INTO sport_match (title, status, home_title, home_odds, draw_odds, away_title, away_odds, date_start, source, registration_date, sport_srl) VALUES (@title, 'Wait', @homeTeam, @homeOdds, @drawOdds, @awayTeam, @awayOdds, @startDate, 'Forebet', '{DateTime.Now.ToDatebaseDateTime()}', {1})";
            await SqlCommand.PrepareAsync();
            await BeginTransactionAsync();

            try
            {
                foreach (var matchData in matchDatas)
                {
                    SqlCommand.Parameters["@title"].Value = $"{matchData.HomeTeam}-{matchData.AwayTeam}";
                    SqlCommand.Parameters["@homeTeam"].Value = matchData.HomeTeam;
                    SqlCommand.Parameters["@awayTeam"].Value = matchData.AwayTeam;
                    SqlCommand.Parameters["@homeOdds"].Value = matchData.HomeOdds;
                    SqlCommand.Parameters["@drawOdds"].Value = matchData.DrawOdds;
                    SqlCommand.Parameters["@awayOdds"].Value = matchData.AwayOdds;
                    SqlCommand.Parameters["@startDate"].Value = matchData.StartDate.ToDatebaseDateTime();

                    await SqlCommand.ExecuteNonQueryAsync();
                }
                await CommitAsync();
                await DisposeTransactionAsync();
            }
            catch (Exception e)
            {
                try
                {
                    await Logger.SendTelegramMessageAsync(e.ToString());
                    await RollbackAsync();
                }
                catch (Exception e2)
                {
                    await Logger.SendTelegramMessageAsync(e2.ToString());
                }
                return false;
            }

            return true;
        }

        #region Dispose
        protected virtual void Dispose(bool disposing)
        {
            if (!DisposedValue)
            {
                if (disposing)
                {
                    // TODO: 관리형 상태(관리형 개체)를 삭제합니다.
                }
                SqlCommand.Dispose();

                // TODO: 비관리형 리소스(비관리형 개체)를 해제하고 종료자를 재정의합니다.
                // TODO: 큰 필드를 null로 설정합니다.
                DisposedValue = true;
            }
        }

        // // TODO: 비관리형 리소스를 해제하는 코드가 'Dispose(bool disposing)'에 포함된 경우에만 종료자를 재정의합니다.
        ~SqlExecutor()
        {
            // 이 코드를 변경하지 마세요. 'Dispose(bool disposing)' 메서드에 정리 코드를 입력합니다.
            Dispose(false);
        }

        public void Dispose()
        {
            // 이 코드를 변경하지 마세요. 'Dispose(bool disposing)' 메서드에 정리 코드를 입력합니다.
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }
        #endregion
    }
}

using System;


namespace forebet_console
{
    public class MatchData
    {
        public string HomeTeam { get; set; }
        public string AwayTeam { get; set; }
        public DateTime StartDate { get; set; }
        public double? HomeOdds { get; set; }
        public double? DrawOdds { get; set; }
        public double? AwayOdds { get; set; }

        public override string ToString() =>
            $"Team: {HomeTeam}-{AwayTeam} " +
            $"Home Odds: {(HomeOdds.HasValue ? HomeOdds.ToString() : "-")} " +
            $"Draw Odds: {(DrawOdds.HasValue ? DrawOdds.ToString() : "-")} " +
            $"Away Odds: {(AwayOdds.HasValue ? AwayOdds.ToString() : "-")} " +
            $"Start Date: {StartDate} ";
    }
}

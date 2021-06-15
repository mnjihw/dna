using System;
using System.ComponentModel.DataAnnotations.Schema;

namespace forebet_console
{
    [Table("sport_match")]
    public class MatchData
    {
        [Column("home_title")]
        public string HomeTeam { get; set; }
        [Column("away_title")]
        public string AwayTeam { get; set; }
        [Column("date_start")]
        public DateTime StartDate { get; set; }
        [Column("home_odds")]
        public double? HomeOdds { get; set; }
        [Column("draw_odds")]
        public double? DrawOdds { get; set; }
        [Column("away_odds")]
        public double? AwayOdds { get; set; }

        public override string ToString() =>
            $"Team: {HomeTeam}-{AwayTeam} " +
            $"Home Odds: {(HomeOdds.HasValue ? HomeOdds.ToString() : "-")} " +
            $"Draw Odds: {(DrawOdds.HasValue ? DrawOdds.ToString() : "-")} " +
            $"Away Odds: {(AwayOdds.HasValue ? AwayOdds.ToString() : "-")} " +
            $"Start Date: {StartDate} ";
    }
}

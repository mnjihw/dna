using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokerCommon
{
    public enum Ranking
    {
        Top, OnePair, TwoPair, ThreeOfAKind, Straight, BackStraight, Mountain, Flush, FullHouse, FourOfAKind, StraightFlush, BackStraightFlush, RoyalStraightFlush
    }

    public class HandRanking
    {
        public Ranking Ranking { get; set; } = Ranking.Top;
        public Rank Rank { get; set; }
        public Suit Suit { get; set; }

        public override string ToString()
        {
            return $"{Rank} {Suit} {Ranking}";
        }
    }
}

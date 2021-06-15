using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;


namespace forebet_console
{
    public class MatchDataComparer : IEqualityComparer<MatchData>
    {
        public bool Equals([AllowNull] MatchData x, [AllowNull] MatchData y) => x.HomeTeam == y.HomeTeam && x.AwayTeam == y.AwayTeam;

        public int GetHashCode([DisallowNull] MatchData obj) => (obj.HomeTeam, obj.AwayTeam).GetHashCode();
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokerCommon
{
    [Serializable]
    public class Player
    {
        public string Name { get; set; }
        public decimal Money { get; set; } = 1_000_000_000;
        public bool IsHost { get; set; }

        public Hand Hand { get; set; } = new Hand();

        public override string ToString()
        {
            return $"이름: {Name} 돈: {Money} 방장이니?: {IsHost}";
        }
    }
}
 
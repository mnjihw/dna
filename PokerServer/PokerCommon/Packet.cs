using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokerCommon
{
    public enum PacketHeader : ushort
    {
        SetName,
        PlayerEntered,
        StartGame,
        DiscardCard

    }
    [Serializable]
    public class Packet
    {
        public PacketHeader Header { get; set; }
        public string Data { get; set; }
    }
}

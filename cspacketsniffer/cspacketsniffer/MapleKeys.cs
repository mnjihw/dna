using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cspacketsniffer
{
    class MapleKeys
    {
        public static byte[] GetKey(string keyString)
        {
            byte[] realKey = new byte[8];
            int index = 0;

            for (int i = 0; i < 4 * 8 * 2; i += 4 * 2)
                realKey[index++] = byte.Parse(keyString[i] + "" + keyString[i + 1], System.Globalization.NumberStyles.HexNumber);

            byte[] expandedKey = new byte[32];

            for (int i = 0; i < 8; i++)
                expandedKey[i * 4] = realKey[i];

            return expandedKey;
        }
    }
}

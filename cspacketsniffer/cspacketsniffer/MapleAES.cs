using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace cspacketsniffer
{
    public sealed class MapleAES : IDisposable
    {
        
        private readonly static byte[] sShiftKey = new byte[] {
            0xEC, 0x3F, 0x77, 0xA4, 0x45, 0xD0, 0x71, 0xBF, 0xB7, 0x98, 0x20, 0xFC, 0x4B, 0xE9, 0xB3, 0xE1,
            0x5C, 0x22, 0xF7, 0x0C, 0x44, 0x1B, 0x81, 0xBD, 0x63, 0x8D, 0xD4, 0xC3, 0xF2, 0x10, 0x19, 0xE0,
            0xFB, 0xA1, 0x6E, 0x66, 0xEA, 0xAE, 0xD6, 0xCE, 0x06, 0x18, 0x4E, 0xEB, 0x78, 0x95, 0xDB, 0xBA,
            0xB6, 0x42, 0x7A, 0x2A, 0x83, 0x0B, 0x54, 0x67, 0x6D, 0xE8, 0x65, 0xE7, 0x2F, 0x07, 0xF3, 0xAA,
            0x27, 0x7B, 0x85, 0xB0, 0x26, 0xFD, 0x8B, 0xA9, 0xFA, 0xBE, 0xA8, 0xD7, 0xCB, 0xCC, 0x92, 0xDA,
            0xF9, 0x93, 0x60, 0x2D, 0xDD, 0xD2, 0xA2, 0x9B, 0x39, 0x5F, 0x82, 0x21, 0x4C, 0x69, 0xF8, 0x31,
            0x87, 0xEE, 0x8E, 0xAD, 0x8C, 0x6A, 0xBC, 0xB5, 0x6B, 0x59, 0x13, 0xF1, 0x04, 0x00, 0xF6, 0x5A,
            0x35, 0x79, 0x48, 0x8F, 0x15, 0xCD, 0x97, 0x57, 0x12, 0x3E, 0x37, 0xFF, 0x9D, 0x4F, 0x51, 0xF5,
            0xA3, 0x70, 0xBB, 0x14, 0x75, 0xC2, 0xB8, 0x72, 0xC0, 0xED, 0x7D, 0x68, 0xC9, 0x2E, 0x0D, 0x62,
            0x46, 0x17, 0x11, 0x4D, 0x6C, 0xC4, 0x7E, 0x53, 0xC1, 0x25, 0xC7, 0x9A, 0x1C, 0x88, 0x58, 0x2C,
            0x89, 0xDC, 0x02, 0x64, 0x40, 0x01, 0x5D, 0x38, 0xA5, 0xE2, 0xAF, 0x55, 0xD5, 0xEF, 0x1A, 0x7C,
            0xA7, 0x5B, 0xA6, 0x6F, 0x86, 0x9F, 0x73, 0xE6, 0x0A, 0xDE, 0x2B, 0x99, 0x4A, 0x47, 0x9C, 0xDF,
            0x09, 0x76, 0x9E, 0x30, 0x0E, 0xE4, 0xB2, 0x94, 0xA0, 0x3B, 0x34, 0x1D, 0x28, 0x0F, 0x36, 0xE3,
            0x23, 0xB4, 0x03, 0xD8, 0x90, 0xC8, 0x3C, 0xFE, 0x5E, 0x32, 0x24, 0x50, 0x1F, 0x3A, 0x43, 0x8A,
            0x96, 0x41, 0x74, 0xAC, 0x52, 0x33, 0xF0, 0xD9, 0x29, 0x80, 0xB1, 0x16, 0xD3, 0xAB, 0x91, 0xB9,
            0x84, 0x7F, 0x61, 0x1E, 0xCF, 0xC5, 0xD1, 0x56, 0x3D, 0xCA, 0xF4, 0x05, 0xC6, 0xE5, 0x08, 0x49
        };

        private ushort Build { get; set; } = 0;
        private RijndaelManaged AES { get; set; } = new RijndaelManaged();
        private ICryptoTransform Transformer { get; set; } = null;
        static private readonly string keyString = "CABE7D9F898A411BFDB84F68F6727B1499CDD30DF0443AB4A66653330BCBA110";
        public byte[] IV { get; private set; }
        static private byte[] Key { get; set; }
        static MapleAES()
        {
            Key = MapleKeys.GetKey(keyString);
        }

        internal MapleAES(ushort build, byte[] iv)
        {
            Build = build;

            AES.Key = Key;
            AES.Mode = CipherMode.ECB;
            AES.Padding = PaddingMode.PKCS7;
            Transformer = AES.CreateEncryptor();
            IV = iv;
        }

        bool disposed = false;
        public void Dispose()
        {
            if (disposed)
                return;
            Transformer?.Dispose();
            disposed = true;
        }


        public bool ConfirmHeader(byte[] pBuffer, int pStart)
        {
            bool b = (pBuffer[pStart] ^ IV[2]) == (Build & 0xFF) &&
                   (pBuffer[pStart + 1] ^ IV[3]) == ((Build >> 8) & 0xFF);
            return b;
        }

        public static int GetHeaderLength(byte[] pBuffer, bool pOldHeader)
        {
            if (pOldHeader)
                return 4;

            ushort ivBytes = (ushort)(pBuffer[0] | pBuffer[1] << 8);
            ushort xorredSize = (ushort)(pBuffer[2] | pBuffer[3] << 8);
            ushort length = (ushort)(xorredSize ^ ivBytes);

            if (length == 0xFF00)
                return 8;
            else
                return 4;
        }

        public static int GetPacketLength(byte[] pBuffer, int pBytesAvailable, bool pOldHeader)
        {
            if (pBytesAvailable < 4)
                return pBytesAvailable - 4;

            if (pOldHeader)
                return BitConverter.ToUInt16(pBuffer, 2);

            ushort ivBytes = (ushort)(pBuffer[0] | pBuffer[1] << 8);
            ushort xorredSize = (ushort)(pBuffer[2] | pBuffer[3] << 8);

            ushort length = (ushort)(xorredSize ^ ivBytes);

            if (length == 0xFF00)
            {
                if (pBytesAvailable < 8)
                    return pBytesAvailable - 8;
                return BitConverter.ToInt32(pBuffer, 4) ^ ivBytes;
            }
            return length;
        }


        const int AES_XOR_TABLE_BLOCKS = 92; // maximum amount of blocks; this is for 1472 bytes
        private byte[] AES_XOR_TABLE = new byte[AES_XOR_TABLE_BLOCKS * 16];

        public void TransformAES(byte[] pData)
        {
            int dataSize = pData.Length;

            byte[] freshIVBlock = new byte[16] {
                IV[0], IV[1], IV[2], IV[3],
                IV[0], IV[1], IV[2], IV[3],
                IV[0], IV[1], IV[2], IV[3],
                IV[0], IV[1], IV[2], IV[3]
            };

            int requiredBlocks = Math.Min((dataSize / 16) + 1, AES_XOR_TABLE_BLOCKS);

            // Transform block 1 (from the fresh IV block) to index 0
            Transformer.TransformBlock(freshIVBlock, 0, 16, AES_XOR_TABLE, 0);

            int i;
            for (i = 0; i < (requiredBlocks - 1); i++)
            {
                // Transform N to N + 1
                Transformer.TransformBlock(AES_XOR_TABLE, (i * 16), 16, AES_XOR_TABLE, ((i + 1) * 16));
            }

            int startOffset = 1456;
            if (dataSize >= 0xFF00)
                startOffset -= 4; // Substract 4 bytes (big header size)

            int blockSize = Math.Min(startOffset, dataSize);

            // How many bytes do we process per iteration?
            const int bigChunkSize = 8;
            unsafe
            {
                fixed (byte* dataPtr = pData)
                fixed (byte* xorPtr = AES_XOR_TABLE)
                {
                    byte* currentInputByte = dataPtr;

                    for (int start = 0; start < dataSize;)
                    {
                        byte* currentXorByte = xorPtr;

                        i = 0;

                        int intBlocks = blockSize / bigChunkSize;
                        for (; i < intBlocks; ++i)
                        {
                            *(ulong*)currentInputByte ^= *(ulong*)currentXorByte;
                            currentInputByte += bigChunkSize;
                            currentXorByte += bigChunkSize;
                        }

                        i *= bigChunkSize;

                        for (; i < blockSize; i++)
                        {
                            *(currentInputByte++) ^= *(currentXorByte++);
                        }
                        start += blockSize;
                        blockSize = Math.Min(1460, dataSize - start);
                    }
                }
            }
        }

        public void ShiftIV(byte[] oldIv = null)
        {
            if (oldIv == null)
                oldIv = IV;

            byte[] newIV = new byte[] { 0xF2, 0x53, 0x50, 0xC6 };
            for (int i = 0; i < 4; ++i)
                Morph(oldIv[i], newIV);

            Buffer.BlockCopy(newIV, 0, IV, 0, IV.Length);
        }

        public static void Morph(byte value, byte[] iv)
        {
            byte input = value;
            byte tableInput = sShiftKey[input];
            iv[0] += (byte)(sShiftKey[iv[1]] - input);
            iv[1] -= (byte)(iv[2] ^ tableInput);
            iv[2] ^= (byte)(sShiftKey[iv[3]] + input);
            iv[3] -= (byte)(iv[0] - tableInput);

            uint val = (uint)(iv[0] | iv[1] << 8 | iv[2] << 16 | iv[3] << 24);
            val = (val >> 0x1D | val << 0x03); // ROL32(3)
            iv[0] = (byte)(val & 0xFF);
            iv[1] = (byte)((val >> 8) & 0xFF);
            iv[2] = (byte)((val >> 16) & 0xFF);
            iv[3] = (byte)((val >> 24) & 0xFF);
        }
    }
}

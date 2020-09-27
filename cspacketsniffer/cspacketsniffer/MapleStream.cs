using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace cspacketsniffer
{
    [Flags]
    public enum TransformMethod : int
    {
        AES = 1 << 1,
        MAPLE_CRYPTO = 1 << 2,
        OLD_KMS_CRYPTO = 1 << 3,
        KMS_CRYPTO = 1 << 4,

        SHIFT_IV = 1 << 5,
        SHIFT_IV_OLD = 1 << 6,
        NONE = 0
    }

    public sealed class MapleStream
    {

        private const int DEFAULT_SIZE = 4096;

        private bool IsOutbound { get; set; } = false;
        private MapleAES AES { get; set; } = null;
        private byte[] buffer = new byte[DEFAULT_SIZE];
        private int Cursor { get; set; } = 0;

        private int ExpectedDataSize { get; set; } = 4;
        private bool UsesOldHeader { get; set; } = false;

        public ushort Build { get; private set; }
        public byte Locale { get; private set; }


        public MapleStream(bool isOutbound, ushort build, byte locale, byte[] iv)
        {
            IsOutbound = isOutbound;
            Build = build;
            Locale = locale;


            if (IsOutbound)
                AES = new MapleAES(Build, iv);
            else
                AES = new MapleAES((ushort)(0xFFFF - Build), iv);

        }

        public void Append(byte[] pBuffer) => Append(pBuffer, 0, pBuffer.Length);
        public void Append(byte[] pBuffer, int pStart, int pLength)
        {
            if (buffer.Length - Cursor < pLength)
            {
                int newSize = buffer.Length * 2;
                while (newSize < Cursor + pLength)
                    newSize *= 2;
                Array.Resize(ref buffer, newSize);
            }
            Buffer.BlockCopy(pBuffer, pStart, buffer, Cursor, pLength);
            Cursor += pLength;
        }

        public MaplePacket Read()
        {
            if (Cursor < ExpectedDataSize)
                return null;
            if (!(!IsOutbound && Sniffer.IsInGame) && !AES.ConfirmHeader(buffer, 0))
            {
                throw new Exception("Failed to confirm packet header");
            }

            int headerLength = MapleAES.GetHeaderLength(buffer, UsesOldHeader);
            ExpectedDataSize = headerLength;
            if (Cursor < headerLength)
            {
                return null;
            }

            int packetSize = MapleAES.GetPacketLength(buffer, Cursor, UsesOldHeader);
            ExpectedDataSize = packetSize + headerLength;
            if (Cursor < (packetSize + headerLength))
            {
                return null;
            }

            byte[] packetBuffer = new byte[packetSize];
            Buffer.BlockCopy(buffer, headerLength, packetBuffer, 0, packetSize);


            

            if (!IsOutbound && Sniffer.IsInGame)
            {
                for (int i = 0; i < packetBuffer.Length; ++i)
                    packetBuffer[i] = (byte)(packetBuffer[i] - AES.IV[0]);
            }
            else
                Decrypt(packetBuffer, TransformMethod.AES | TransformMethod.SHIFT_IV);

            Cursor -= ExpectedDataSize;
            if (Cursor > 0)
                Buffer.BlockCopy(buffer, ExpectedDataSize, buffer, 0, Cursor);
            ushort opcode;

           
            
            opcode = (ushort)(packetBuffer[0] | (packetBuffer[1] << 8));
            //Buffer.BlockCopy(packetBuffer, 2, packetBuffer, 0, packetSize - 2);
            //Array.Resize(ref packetBuffer, packetSize - 2);

            //no need to do this


            ExpectedDataSize = 4;


            return new MaplePacket(IsOutbound, opcode, packetBuffer);
        }

        private void Decrypt(byte[] pBuffer, TransformMethod pTransformLocale)
        {
            if ((pTransformLocale & TransformMethod.AES) != 0)
                AES.TransformAES(pBuffer);

            if ((pTransformLocale & TransformMethod.MAPLE_CRYPTO) != 0)
            {
                for (int index1 = 1; index1 <= 6; ++index1)
                {
                    byte firstFeedback = 0;
                    byte secondFeedback;
                    byte length = (byte)(pBuffer.Length & 0xFF);
                    if ((index1 % 2) == 0)
                    {
                        for (int index2 = 0; index2 < pBuffer.Length; ++index2)
                        {
                            byte temp = pBuffer[index2];
                            temp -= 0x48;
                            temp = (byte)(~temp);
                            temp = RollLeft(temp, length & 0xFF);
                            secondFeedback = temp;
                            temp ^= firstFeedback;
                            firstFeedback = secondFeedback;
                            temp -= length;
                            temp = RollRight(temp, 3);
                            pBuffer[index2] = temp;
                            --length;
                        }
                    }
                    else
                    {
                        for (int index2 = pBuffer.Length - 1; index2 >= 0; --index2)
                        {
                            byte temp = pBuffer[index2];
                            temp = RollLeft(temp, 3);
                            temp ^= 0x13;
                            secondFeedback = temp;
                            temp ^= firstFeedback;
                            firstFeedback = secondFeedback;
                            temp -= length;
                            temp = RollRight(temp, 4);
                            pBuffer[index2] = temp;
                            --length;
                        }
                    }
                }
            }


            if ((pTransformLocale & TransformMethod.SHIFT_IV) != 0)
                AES.ShiftIV();
        }

        public static byte RollLeft(byte pThis, int pCount)
        {
            uint overflow = ((uint)pThis) << (pCount % 8);
            return (byte)((overflow & 0xFF) | (overflow >> 8));
        }

        public static byte RollRight(byte pThis, int pCount)
        {
            uint overflow = (((uint)pThis) << 8) >> (pCount % 8);
            return (byte)((overflow & 0xFF) | (overflow >> 8));
        }
    }
}

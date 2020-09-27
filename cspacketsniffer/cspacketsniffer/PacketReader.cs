using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cspacketsniffer
{
    public class PacketReader : IDisposable
    {
        protected MemoryStream _buffer;

        private readonly BinaryReader _binReader;

        public int Length
        {
            get { return (int)_buffer.Length; }
        }

        public PacketReader(byte[] arrayOfBytes)
        {
            _buffer = new MemoryStream(arrayOfBytes, false);
            _binReader = new BinaryReader(_buffer, Encoding.Default);
        }

        bool disposed = false;
        public void Dispose()
        {
            if (disposed) return;
            _binReader.Close();
            _buffer.Dispose();

            disposed = true;
        }

       
        public void Reset(int length)
        {
            _buffer.Seek(length, SeekOrigin.Begin);
        }

        public void Skip(int length)
        {
            _buffer.Position += length;
        }

        public int Position
        {
            get
            {
                return (int)_buffer.Position;
            }
        }
        public int Remaining
        {
            get
            {
                return Length - (int)_buffer.Position;
            }
        }

        public byte ReadByte()
        {
            return _binReader.ReadByte();
        }

        public byte[] ReadBytes(int count)
        {
            return _binReader.ReadBytes(count);
        }

        public bool ReadBool()
        {
            return _binReader.ReadBoolean();
        }

        
        public short ReadShort()
        {
            return _binReader.ReadInt16();
        }

        public ushort ReadUShort()
        {
            return _binReader.ReadUInt16();
        }

        public int ReadInt()
        {
            return _binReader.ReadInt32();
        }

        public uint ReadUInt()
        {
            return _binReader.ReadUInt32();
        }

        public long ReadLong()
        {
            return _binReader.ReadInt64();
        }

        
        public ulong ReadULong()
        {
            return _binReader.ReadUInt64();
        }

        public string ReadString(int length)
        {
            return Encoding.Default.GetString(ReadBytes(length));
        }

        public string ReadMapleString()
        {
            return ReadString(ReadShort());
        }

        public byte[] ToArray()
        {
            return _buffer.ToArray();
        }
    }
}

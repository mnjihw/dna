using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading.Tasks;

namespace PokerCommon
{
    public static class TcpClientExtensions
    {
        private static readonly BinaryFormatter binaryFormatter = new BinaryFormatter();
        public static async Task SendData(this TcpClient tcpClient, dynamic data, Action<TcpClient> cleanup)
        {
            using MemoryStream memoryStream = new MemoryStream();

            memoryStream.Seek(sizeof(int), SeekOrigin.Begin);
            binaryFormatter.Serialize(memoryStream, data);
            memoryStream.Seek(0, SeekOrigin.Begin);
            await memoryStream.WriteAsync(BitConverter.GetBytes(memoryStream.Length - sizeof(int)), 0, sizeof(int));
            memoryStream.Seek(0, SeekOrigin.Begin);

            try
            {    
                await memoryStream.CopyToAsync(tcpClient.GetStream());
            }
            catch(Exception e)
            {
                if(e is IOException || e is ObjectDisposedException)
                {
                    cleanup(tcpClient);
                    Console.WriteLine("접속 끊김 in SendData()");
                    return;
                }
                Console.WriteLine(e);
                throw;
            }
        }

        public static async Task<dynamic> ReceiveData(this TcpClient tcpClient, Action<TcpClient> cleanup)
        {
            dynamic receivedData;
            var sizeBuffer = new byte[sizeof(int)];
            using MemoryStream memoryStream = new MemoryStream();
            byte[] buf;

            try
            {
                await tcpClient.GetStream().ReadAsync(sizeBuffer, 0, sizeBuffer.Length);
                buf = new byte[BitConverter.ToUInt32(sizeBuffer, 0)];
                await tcpClient.GetStream().ReadAsync(buf, 0, buf.Length);
            }
            catch(Exception e)
            {
                if(e is IOException || e is ObjectDisposedException)
                {
                    cleanup(tcpClient);
                    Console.WriteLine("접속 끊김 in ReceiveData()");
                    return null;
                }
                Console.WriteLine(e);
                throw;
            }
            await memoryStream.WriteAsync(buf, 0, buf.Length);
            memoryStream.Seek(0, SeekOrigin.Begin);
            receivedData = binaryFormatter.Deserialize(memoryStream);


            return receivedData switch
            {
                Player player => player,
                Packet packet => packet,
                Hand hand => hand,
                _ => null,
            };
        }

        
    }
}

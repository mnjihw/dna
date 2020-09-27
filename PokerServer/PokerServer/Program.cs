using PokerCommon;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading.Tasks;

namespace PokerServer
{
    class Program
    {
        private static readonly List<(TcpClient tcpClient, Player player)> clients = new List<(TcpClient, Player)>();
        private static readonly Action<TcpClient> cleanup = tcpClient =>
        {
            tcpClient.Close();

            var client = FindClientWith(tcpClient);
            Console.WriteLine("클린업!");
            clients.Remove(client);
            
        };

        private static async Task BroadcastData(IEnumerable<(TcpClient tcpClient, Player player)> clients, dynamic data, Predicate<(TcpClient tcpClient, Player player)> condition, Action<TcpClient> cleanup)
        {
            foreach(var client in clients)
            {
                if(condition(client))
                    await client.tcpClient.SendData((object)data, cleanup);
            }
        }

        private static (TcpClient tcpClient, Player player) FindClientWith(TcpClient tcpClient)
        {
            foreach(var client in clients)
            {
                if (client.tcpClient == tcpClient)
                    return client;
            }
            return default;
        }
        private static async Task ProcessPacket(TcpClient tcpClient, Packet packet)
        {
            switch(packet.Header)
            {
                case PacketHeader.SetName:
                    Console.WriteLine("닉 생성 요청");
                    var name = packet.Data;
                    var player = new Player { Name = name, IsHost = clients.Count == 1 };//FIXME

                    if (clients.Any(client => client.player?.Name == name))
                    {
                        player.Name = null;
                        await tcpClient.SendData(player, cleanup);
                        return;
                    }
                    await tcpClient.SendData(player, cleanup);

                    await BroadcastData(clients, new Packet
                    {
                        Header = PacketHeader.PlayerEntered,
                        Data = player.Name
                    }, tuple => tuple.tcpClient != tcpClient, cleanup);
                    
                    for(int i = 0; i < clients.Count; ++i)
                    {
                        if(clients[i].tcpClient == tcpClient)
                        {
                            clients[i] = (tcpClient, player);
                            break;
                        }
                    }
                    break;
                case PacketHeader.StartGame:
                    for(int i = 0; i < clients.Count; ++i)
                    {
                        if(clients[i].tcpClient == tcpClient && clients[i].player.IsHost)
                        {
                            Console.WriteLine("게임을 시작할까용?");
                            await BroadcastData(clients, new Packet { Header = PacketHeader.StartGame }, t => true, cleanup);
                            
                            for(i = 0; i < clients.Count; ++i)
                            {
                                if (clients[i].player != null)
                                {
                                    var hand = clients[i].player.Hand;
                                    hand.TakeFromStub(4);
                                    await clients[i].tcpClient.SendData(hand, cleanup);
                                }
                            }

                            break;
                        }
                    }
                    break;
                case PacketHeader.DiscardCard:
                    var client = FindClientWith(tcpClient);
                    client.player.Hand.Remove(int.Parse(packet.Data));
                    client.player.Hand.Print();
                    break;
            }
        }



        static async Task Main()
        {
            TcpListener listener = new TcpListener(IPAddress.Loopback, 7000);

            listener.Start();

            while(true)
            {
                TcpClient tcpClient = await listener.AcceptTcpClientAsync();
                Console.WriteLine("누군가 접속함");
                clients.Add((tcpClient, null));

                _ = Task.Run(async () =>
                {
                    while (true)
                    {
                        var packet = await tcpClient.ReceiveData(cleanup);
                        if (packet == null)
                            return;
                        ProcessPacket(tcpClient, packet);
                        await Task.Delay(300);
                    }
                });
          
            }
        }
    }
}

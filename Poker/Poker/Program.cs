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

namespace Poker
{

    class Program
    {
        private static readonly Action<TcpClient> cleanup = tcpClient => tcpClient.Close();
        private static Player player;

        
        static async Task ProcessPacket(TcpClient tcpClient, Packet packet)
        {
            switch(packet.Header)
            {
                case PacketHeader.PlayerEntered:
                    Console.WriteLine($"{packet.Data}님이 들어왔습니다.");

                    if (player?.IsHost == true)
                    {
                        Console.WriteLine("시작하려면 엔터");
                        Console.ReadLine();
                        await tcpClient.SendData(new Packet { Header = PacketHeader.StartGame, Data = "" }, cleanup);
                    }
                    
                    break;
                case PacketHeader.StartGame:
                    Console.WriteLine("게임 시작됨");
                    break;
            }
        }

        static IEnumerable<int >Range(int n)
        {
            for(int i = 0; i< n; ++i)
            {
                yield return i;
            }
        }
        static async Task Main()
        {
            foreach(var i in Range(100))
                Console.WriteLine(i);
            return;
            using TcpClient tcpClient = new TcpClient();
            await tcpClient.ConnectAsync(IPAddress.Loopback, 7000);
            var stream = tcpClient.GetStream();
            dynamic receivedData;
            

            while(true)
            { 
                Console.Write("닉네임 입력: ");
                var name = Console.ReadLine();
                await tcpClient.SendData(new Packet
                {
                    Header = PacketHeader.SetName,
                    Data = name
                }, cleanup);



                while (true)
                {
                    receivedData = await tcpClient.ReceiveData(cleanup);

                    switch (receivedData)
                    {
                        case Player p:
                            if (p.Name == null)
                            {
                                Console.Clear();
                                Console.WriteLine("닉네임 중복! 다시 입력해주세요");
                                Console.Write("닉네임 입력: ");
                                name = Console.ReadLine();
                                await tcpClient.SendData(new Packet
                                {
                                    Header = PacketHeader.SetName,
                                    Data = name
                                }, cleanup);
                            }
                            player = p;
                            Console.WriteLine(player);
                            break;
                        case Packet packet:
                            await ProcessPacket(tcpClient, packet);
                            break;
                        case Hand hand:
                            player.Hand = hand;
                            hand.Print();
                            int index;
                            while (true)
                            {
                                Console.Write("버릴 카드 입력: ");
                                if (!int.TryParse(Console.ReadLine(), out index))
                                {
                                    Console.WriteLine("잘못 입력");
                                }
                                else if(index < 0 || index > hand.Count)
                                {
                                    Console.WriteLine("잘못된 범위 입력");
                                }
                                else
                                    break;
                            }
                            --index;
                            player.Hand.Remove(index);
                            await tcpClient.SendData(new Packet { Header = PacketHeader.DiscardCard, Data = index.ToString() }, cleanup);
                            
                            break;
                        case Card card:
                            player.Hand.Add(card);
                            player.Hand.Print();
                            break;
                        default:
                            Console.WriteLine("뭐야 몰라 이상해");
                            break;
                    }

                    
                    await Task.Delay(400);
                }
                

            }
        }
        
    }
}

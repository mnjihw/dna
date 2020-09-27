using PacketDotNet;
using SharpPcap;
using SharpPcap.LibPcap;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using System.Windows;

namespace cspacketsniffer
{
    class Sniffer
    {
        public enum Results
        {
            Show,
            Continue,
            Terminated
        }
        public static bool IsInGame { get; set; }
        private LibPcapLiveDevice device = null;
        private readonly Timer timer = new Timer() { Interval = 300 };
        public Sniffer()
        {
            if (LibPcapLiveDeviceList.Instance.Count == 0)
                throw new Exception("no device detected");
            SetupAdapter("이더넷");
            timer.Elapsed += ReceiveTimer;
            timer.Start();

        }
        private ushort LocalPort { get; set; }
        private ushort RemotePort { get; set; }
        private uint _outboundSequence;
        private uint _inboundSequence;
        public ushort Build { get; private set; }
        private MapleStream OutboundStream { get; set; }
        private MapleStream InboundStream { get; set; }
        private Dictionary<uint, byte[]> InboundBuffer { get; set; } = new Dictionary<uint, byte[]>();
        private Dictionary<uint, byte[]> OutboundBuffer { get; set; } = new Dictionary<uint, byte[]>();

  
        private void Dump(byte[] data, int size, bool isOutbound)
        {
            const string hex = "0123456789ABCDEF";
            uint i, off, fucked = 0;
            byte[] buffer = new byte[256];

            if(size > 256)
                size = 256;
            uint idx = 0;

            Console.WriteLine(isOutbound ? "[Outbound]" : "[Inbound]");

            for (off = 0; off < size; off += 16)
            {
                idx = 0;
                i = off;
                do
                {
                    buffer[idx] = (byte)hex[data[i] >> 4];
                    buffer[idx + 1] = (byte)hex[data[i] & 15];
                    buffer[idx + 2] = (byte)' ';
                    idx += 3;
                    if(++i >= size)
                    {
                        while(i < off + 16)
                        {
                            buffer[idx] = (byte)' ';
                            buffer[idx + 1] = (byte)' ';
                            buffer[idx + 2] = (byte)' ';
                            idx += 3;
                            ++i;
                        }
                        break;
                    }
                } while (i < off + 16);

                i = off;

                do
                {
                    
                    if (fucked != 0)
                        --fucked;
                    if (data[i] > 31 && data[i] < 127)
                        buffer[idx++] = data[i];
                    else if ((data[i] & 0x80) != 0 && i + 1 < size && ((data[i] == 0xA4 && data[i + 1] >= 0xA1 && data[i + 1] <= 0xFE) ||
(data[i] >= 0xB0u && data[i] <= 0xC8u && data[i + 1] >= 0xA1u && data[i + 1] <= 0xFEu) ||
(data[i] >= 0x81 && data[i] <= 0xA0 && ((data[i + 1] >= 0x41 && data[i + 1] <= 0x5A) || (data[i + 1] >= 0x61 && data[i + 1] <= 0x7A) || (data[i + 1] >= 0x81 && data[i + 1] <= 0xFE))) ||
(data[i] >= 0xA1 && data[i] <= 0xC5 && ((data[i + 1] >= 0x41 && data[i + 1] <= 0x5A) || (data[i + 1] >= 0x61 && data[i + 1] <= 0x7A) || (data[i + 1] >= 0x81 && data[i + 1] <= 0xA0))) ||
(data[i] == 0xC6 && data[i + 1] >= 0x41 && data[i + 1] <= 0x52)))
                    {
                        if ((i + 1) < off + 16)
                        {
                            if (fucked != 0)
                                buffer[idx++] = (byte)'.';
                            else
                            {
                                buffer[idx++] = data[i];
                                buffer[idx++] = data[++i];
                            }
                        }
                        else
                        {
                            buffer[idx++] = (byte)'.';
                            fucked = 2;
                        }
                    }
                    else
                        buffer[idx++] = (byte)'.';
                  
                    if (++i >= size)
                    {
                        while (i < off + 16)
                        {
                            buffer[idx++] = (byte)' ';
                            ++i;
                        }
                        break;
                    }
                } while (i < off + 16);

                Console.WriteLine(Encoding.Default.GetString(buffer));
            }
            
        }
        private bool NeedsFiltering(byte[] data)
        {
            //ushort[] filter = { 0x9B, 0x10FA, 0xFE, 0x17BF, 0xD38, 0x97A, 0x7E9, 0x75C, 0xE3C};
            //ushort[] filter = { 0x1ADF };
            return true;
            //return filter.Contains(BitConverter.ToUInt16(data, 0));
        }
        private void ProcessTCPPacket(TcpPacket tcpPacket, ref uint sequence, Dictionary<uint, byte[]> buffer, MapleStream stream)
        {
            if (tcpPacket.SequenceNumber > sequence)
            {
                while (buffer.TryGetValue(sequence, out byte[] data))
                {
                    buffer.Remove(sequence);
                    stream.Append(data);
                    sequence += (uint)data.Length;
                }
                if (tcpPacket.SequenceNumber > sequence)
                    buffer[tcpPacket.SequenceNumber] = tcpPacket.PayloadData;
            }
            if (tcpPacket.SequenceNumber < sequence)
            {
                int difference = (int)(sequence - tcpPacket.SequenceNumber);
                if (difference > 0)
                {
                    byte[] data = tcpPacket.PayloadData;
                    if (data.Length > difference)
                    {
                        stream.Append(data, difference, data.Length - difference);
                        sequence += (uint)(data.Length - difference);
                    }
                }
            }
            else if (tcpPacket.SequenceNumber == sequence)
            {
                byte[] data = tcpPacket.PayloadData;
                if (data != null)
                {
                    stream.Append(data);
                    sequence += (uint)data.Length; 
                }
            }

            try
            {
                MaplePacket packet;

                while ((packet = stream.Read()) != null)
                {
                    if(NeedsFiltering(packet.Buffer))
                    {
                        Dump(packet.Buffer, packet.Buffer.Length, packet.IsOutbound);
                        //System.Threading.Thread.Sleep(2000);
                    }
                   
                } 

            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
                return;
            }

        }

        internal Results BufferTCPPacket(TcpPacket tcpPacket)
        {
     
            if (tcpPacket.Fin || tcpPacket.Rst) 
            {
                Build = 0;
                Console.WriteLine("빌드 초기화");
                return Results.Terminated;
            }
            if (tcpPacket.Syn && !tcpPacket.Ack)//handshake from client to server
            {
                LocalPort = tcpPacket.SourcePort;
                RemotePort = tcpPacket.DestinationPort;
                _outboundSequence = tcpPacket.SequenceNumber + 1;
                Console.WriteLine("handshake from client to server");
                return Results.Continue;
            }
            if (tcpPacket.Syn && tcpPacket.Ack) //handshake from server to client
            {
                _inboundSequence = tcpPacket.SequenceNumber + 1;
                Console.WriteLine("handshake from server to client");
                return Results.Continue;
            }
            //어기사널뜸 
            if (tcpPacket.PayloadData?.Length == 0)
                return Results.Continue;
            if (Build == 0) //if not initialized
            {
                byte[] tcpData = tcpPacket.PayloadData;

                if (tcpPacket.SourcePort == LocalPort)
                    _outboundSequence += (uint)tcpData.Length;
                else
                    _inboundSequence += (uint)tcpData.Length;

                ushort length = (ushort)(BitConverter.ToUInt16(tcpData, 0) + 2);

                byte[] headerData = new byte[tcpData.Length];
                Buffer.BlockCopy(tcpData, 0, headerData, 0, tcpData.Length);


                PacketReader pr = new PacketReader(headerData);

                if (length != tcpData.Length || tcpData.Length < 13)
                {
                    Console.WriteLine("Connection did not have a MapleStory Handshake");
                    return Results.Terminated;
                }

                string patchLocation;
                byte[] localIV;
                byte[] remoteIV;

                if (length == 0x11)
                {
                    pr.Skip(4);
                    Build = pr.ReadUShort();
                    pr.Skip(2);
                    localIV = pr.ReadBytes(4);
                    remoteIV = pr.ReadBytes(4);
                    Console.WriteLine("new session");
                    IsInGame = true;
                }
                else
                { 
                    pr.Skip(4);
                    patchLocation = pr.ReadMapleString();
                    localIV = pr.ReadBytes(4);
                    remoteIV = pr.ReadBytes(4);

                    int test = int.Parse(patchLocation);
                    Build = (ushort)(test & 0x7FFF);
                    IsInGame = false;
                }



                OutboundStream = new MapleStream(true, Build, 1, localIV);
                InboundStream = new MapleStream(false, Build, 1, remoteIV);
                //2번째 스트림 만들면 기존거 지우든지 기존거에 iv새로세팅해서 쓰든하셈
               
                ProcessTCPPacket(tcpPacket, ref _inboundSequence, InboundBuffer, InboundStream);

                return Results.Show;
            }
            if (tcpPacket.SourcePort == LocalPort)
                ProcessTCPPacket(tcpPacket, ref _outboundSequence, OutboundBuffer, OutboundStream);
            else
                ProcessTCPPacket(tcpPacket, ref _inboundSequence, InboundBuffer, InboundStream);
            return Results.Continue;
        }
         

        private void ReceiveTimer(object sender, ElapsedEventArgs e)
        {
            try
            {
                RawCapture packet = null;
                timer.Enabled = false;

                while ((packet = device.GetNextPacket()) != null)
                {

                    TcpPacket tcpPacket = (TcpPacket)Packet.ParsePacket(packet.LinkLayerType, packet.Data).Extract(typeof(TcpPacket));
                    
                    try
                    {
                        //right after the handshake
                        //여기서는 if 빼도 될듯
                        if (tcpPacket.Syn && !tcpPacket.Ack && tcpPacket.DestinationPort >= 8484 && tcpPacket.DestinationPort <= 8589)
                        {
                            var res = BufferTCPPacket(tcpPacket);
                            if (res == Results.Continue) { } //new session has started
                        }
                        else
                        {
                            var res = BufferTCPPacket(tcpPacket);


                        }
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(ex.ToString());
                       //session ended
                    }
                }
                timer.Enabled = true;
            }
            catch (Exception)
            {
                if (!device.Opened)
                    device.Open(DeviceMode.Promiscuous, 1);
            }
        }

        private void SetupAdapter(string adapterName)
        {

            if (device != null)
            {
                device.Close();
            }

            foreach (var dev in LibPcapLiveDeviceList.Instance)
            {
                if (dev.Interface.FriendlyName == adapterName)
                {
                    device = dev;
                    break;
                }
                else if(dev == LibPcapLiveDeviceList.Instance[LibPcapLiveDeviceList.Instance.Count - 1])
                {
                    throw new Exception("no adapters matched");
                }
            }
            
            try
            {
                device.Open(DeviceMode.Promiscuous, 1);
            }
            catch
            {
                Console.WriteLine("Failed to set the device in Promiscuous mode! But that doesn't really matter lol.");
                device.Open();
            }
            device.Filter = $"tcp portrange 8484-8589";
        }
    }
}

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace 리듬_채팅
{
    public partial class MainWindow : Window
    {
        private bool IsConnected { get; set; }
        private bool IsServer { get; set; } = true;
        private TcpListener Listener { get; } = new TcpListener(IPAddress.Any, 8000);
        private TcpClient Client { get; set; }
        private TcpClient ClientServerUses { get; set; }

        private enum ConnectionState { ESTABLISHED, LOST};
        public MainWindow()
        {
            RenderOptions.ProcessRenderMode = System.Windows.Interop.RenderMode.SoftwareOnly;
            InitializeComponent();
        }

        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            DragMove();
        }

        private void DisplayConnectionState(ConnectionState connectionState)
        {
            string state = string.Empty;

            switch(connectionState)
            {
                case ConnectionState.ESTABLISHED:
                    state = "연결됨";
                    IsConnected = true;
                    break;
                case ConnectionState.LOST:
                    state = "연결 안 됨";
                    IsConnected = false;
                    break;
            }
            connectionStateTextBlock.Dispatcher.Invoke(new Action(() => connectionStateTextBlock.Text = $"상태: {state}"));
        }

        private void MinimizeButton_Click(object sender, RoutedEventArgs e) => WindowState = WindowState.Minimized;

        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            if (IsServer)
            {
                if (ClientServerUses != null)
                {

                    ClientServerUses.Close();
                    ClientServerUses.Dispose();
                }
            }
            else
            {
                Client.Client.Shutdown(SocketShutdown.Both);
                Client.Close();
                Client.Dispose();
            }


            Listener.Stop();
            Close();
        }

        
        private async Task StartServer()
        {
            IsServer = true;

            Listener.Server.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
            Listener.Start();
            byte[] buffer = new byte[1024];


            while (true)
            {

                acceptTask = Listener.AcceptTcpClientAsync();
                
                while (true)
                {
                    await Task.Delay(100);
                    //ClientServerUses.LingerState = new LingerOption(true, 0);

                    //ClientServerUses.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
                    if (acceptTask.IsCompleted && !IsServer)
                    {

                        if (MessageBox.Show("딴놈이 연결 신청함 연결할까요?", "알림", MessageBoxButton.YesNo) == MessageBoxResult.Yes)
                        {
                            Client.Client.Shutdown(SocketShutdown.Both);
                            Client.Close();
                            Client.Dispose();
                        }
                        else
                        {
                            ClientServerUses.Client.Disconnect(true);
                            return;
                        }

                    }
                    else
                    {
                        if(IsServer)
                        {
                            if (acceptTask.IsCompleted)
                                ClientServerUses = await acceptTask;
                            else
                                continue;
                        }
                        DisplayConnectionState(ConnectionState.ESTABLISHED);
                        NetworkStream stream = IsServer ? ClientServerUses.GetStream() : Client.GetStream();

                      
                        int read;

                        using MemoryStream ms = new MemoryStream();
                        try
                        {
                            do
                            {
                                read = await stream.ReadAsync(buffer, 0, buffer.Length);
                                await ms.WriteAsync(buffer, 0, read);
                            } while (stream.DataAvailable);
                            Console.WriteLine("read " + read);
                        }
                        catch (SocketException)
                        {
                            MessageBox.Show("w끈끊김");
                            DisplayConnectionState(ConnectionState.LOST);
                            TcpClient client = IsServer ? ClientServerUses : Client;

                            client.Client.Shutdown(SocketShutdown.Both);
                            client.Client.Disconnect(true);
                            client.Close();
                            client.Dispose();
                            IsServer = true;
                            break;

                        }
                        if (read != 0)
                            chatListBox.Dispatcher.Invoke(new Action(() => chatListBox.Items.Add(Encoding.UTF8.GetString(ms.ToArray(), 0, (int)ms.Length))));
                        else
                        {
                            Console.WriteLine("2끋ㅇ김");
                            DisplayConnectionState(ConnectionState.LOST);
                            TcpClient client = IsServer ? ClientServerUses : Client;

                            client.Client.Shutdown(SocketShutdown.Both);
                            client.Client.Disconnect(true);
                            client.Close();
                            client.Dispose();
                            IsServer = true;
                            break;

                        } 
                    }

                    
                }
            }
        }

        private Task<TcpClient> acceptTask;
       

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            ipInputTextBox.Text = "mnjihw.asuscomm.com";

            //Client.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.DontLinger, false);
            //Client.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
            //Client.LingerState = new LingerOption(false, 0);
            //Client.Client.ReceiveTimeout = 500;

            Task.Run(() => StartServer());



        }



        private async void ChatInputTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter && chatInputTextBox.Text.Length != 0 && IsConnected)
            {
                NetworkStream stream = IsServer ? ClientServerUses.GetStream() : Client.GetStream();
                byte[] data = Encoding.UTF8.GetBytes(chatInputTextBox.Text);

                try
                {
                    await stream.WriteAsync(data, 0, data.Length);
                }
                catch (SocketException)
                {
                    return;
                }

                chatListBox.Items.Add(chatInputTextBox.Text);
                chatInputTextBox.Text = string.Empty;


            }
        }

        private void IpInputTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
                connectToServerButton.RaiseEvent(new RoutedEventArgs(ButtonBase.ClickEvent));
        }

        private async Task<bool> ConnectToServer(string host, int port)
        {
            Client = new TcpClient();

            await Task.WhenAny(Client.ConnectAsync(Dns.GetHostAddresses(host)[0], port), Task.Delay(1500));


            return Client.Connected;
        }

        private async void ConnectToServerButton_Click(object sender, RoutedEventArgs e)
        {

            if (ipInputTextBox.Text.Length == 0)
                return;
            if (IsConnected)
            {
                if (MessageBox.Show("이미 상대방와 연결돼 있습니다. 그래도 새로 연결하시겠습니까?", "알림", MessageBoxButton.YesNo) == MessageBoxResult.No)
                    return;


                TcpClient client;

                if (IsServer)
                    client = ClientServerUses;
                else
                    client = Client;

                client.Client.Shutdown(SocketShutdown.Both);
                client.Client.Disconnect(true);
                client.Close();
                client.Dispose();
                DisplayConnectionState(ConnectionState.LOST);
            }

            if (await ConnectToServer(ipInputTextBox.Text, 8000))
            {
                DisplayConnectionState(ConnectionState.ESTABLISHED);
                IsServer = false;
            }
           
        }

        private void ChatInputTextBox_PreviewDragOver(object sender, DragEventArgs e)
        {
            e.Effects = DragDropEffects.Copy;
            e.Handled = true;
        }

        private void ChatInputTextBox_Drop(object sender, DragEventArgs e)
        {

            if (e.Data.GetData(DataFormats.FileDrop) is string[] files && files.Length > 0)
            {
                //(sender as TextBox).Text 6= files[0];
                Console.WriteLine( files[0].Substring(files.Length - 2, 3));
            }

        }
    }
}

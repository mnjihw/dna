using System.Windows;
using System.Windows.Controls;
using System.Windows.Markup;
using System.Globalization;
using MaterialDesignThemes.Wpf;
using System;
using System.Linq;
using System.Configuration;
using System.Windows.Input;
using System.Diagnostics;
using System.Text;
using System.ComponentModel.Design;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Runtime.CompilerServices;
using System.Threading;
using TwitchLib.Client.Extensions;
using TwitchLib.Client.Models;
using TwitchLib.Client;
using TwitchLib.Communication.Clients;
using TwitchLib.Communication.Models;
using System.Threading.Tasks;

namespace 예은아씨
{
    

    public partial class MainWindow : Window
    {
        private List<UserInfo> Users { get; set; } = new List<UserInfo>();
        private List<Timer> BanTimers { get; set; } = new List<Timer>();
        private List<Timer> UnbanTimers { get; set; } = new List<Timer>();
        private bool Running { get; set; }
        private Configuration Configuration { get; set; } = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
        //private static string MyId => "yeeunn";
        private static string MyId => "mnjihw0210";
        //private ConnectionCredentials Credentials { get; set; } = new ConnectionCredentials(MyId, "oauth:so26qfb3f3uj0himkr6w6uqxe2owcg"); //https://twitchapps.com/tmi/
        private ConnectionCredentials Credentials { get; set; } = new ConnectionCredentials(MyId, "oauth:vs865v6vvejfz54tqkyydwuzvpz5n8"); //https://twitchapps.com/tmi/
        //oauth:vs865v6vvejfz54tqkyydwuzvpz5n8 이거내꺼
        private static ClientOptions ClientOptions { get; set; } = new ClientOptions { MessagesAllowedInPeriod = 750};
        private static WebSocketClient WebSocketClient { get; set; } = new WebSocketClient(ClientOptions);
        private TwitchClient TwitchClient { get; set; } = new TwitchClient(WebSocketClient);

        private void CreateRow(string id = default, DateTime startTime = default, TimeSpan banDuration = default)
        {
            var innerStackPanel = new StackPanel { Orientation = Orientation.Horizontal, HorizontalAlignment = HorizontalAlignment.Center };
            var idTextBox = new TextBox { Width = 200, MaxLength = 25 , VerticalAlignment = VerticalAlignment.Bottom };
            var timePicker = new TimePicker { Width = 80, VerticalAlignment = VerticalAlignment.Bottom };
            var durationComboBox = new ComboBox { Width = 60, VerticalAlignment = VerticalAlignment.Bottom};
            var minusButton = new Button { Padding = new Thickness(), Width = 25, Height = 25, Content = new PackIcon { Kind = PackIconKind.Minus } };


            idTextBox.PreviewTextInput += TextBox_PreviewTextInput;
            idTextBox.TextChanged += TextBox_TextChanged;


            timePicker.Language = XmlLanguage.GetLanguage(CultureInfo.CurrentCulture.IetfLanguageTag);
            HintAssist.SetHint(idTextBox, "아이디");
            HintAssist.SetHint(timePicker, "시작 시간");
            HintAssist.SetHint(durationComboBox, "기간");
            HintAssist.SetIsFloating(idTextBox, true);
            HintAssist.SetIsFloating(timePicker, true);
            HintAssist.SetIsFloating(durationComboBox, true);


            for (int i = 1; i <= 10; ++i)
                durationComboBox.Items.Add(new ComboBoxItem {Content= $"{i}시간", Tag = i});
            

            if (id != default)
                idTextBox.Text = id;
            if (startTime != default)
                timePicker.SelectedTime = startTime;
            if (banDuration != default)
            {
                foreach (ComboBoxItem item in durationComboBox.Items)
                {
                    if (item.Content as string == $"{banDuration.Hours}시간")
                    {
                        durationComboBox.SelectedItem = item;
                        break;
                    }
                }
            }

            innerStackPanel.Children.Add(idTextBox);
            innerStackPanel.Children.Add(new Separator { Width = 5, Visibility = Visibility.Hidden });
            innerStackPanel.Children.Add(timePicker);
            innerStackPanel.Children.Add(new Separator { Width = 5, Visibility = Visibility.Hidden });
            innerStackPanel.Children.Add(durationComboBox);
            innerStackPanel.Children.Add(new Separator { Width = 5, Visibility = Visibility.Hidden });
            innerStackPanel.Children.Add(minusButton);

            minusButton.Click += (sender, e) =>
            {
                stackPanel.Children.Remove(innerStackPanel);
            };


            stackPanel.Children.Add(innerStackPanel); 
        }
      

        public MainWindow()
        {
            InitializeComponent();
        }

        private void BanTimerCallback(object state)
        {
            var user = state as UserInfo;

            TwitchClient.BanUser(new JoinedChannel(MyId), user.Id);
        }
        private void UnbanTimerCallback(object state)
        {
            var user = state as UserInfo;

            TwitchClient.UnbanUser(new JoinedChannel(MyId), user.Id);
        }


        private void TextBox_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (e.Text == ",")
                e.Handled = true;
        }
        private void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (Keyboard.IsKeyDown(Key.V) && Keyboard.Modifiers == ModifierKeys.Control)
                (sender as TextBox).Text = (sender as TextBox).Text.Replace(",", "");
        }
        private string RowContentsToString(UIElementCollection children)
        {
            StringBuilder sb = new StringBuilder();

            foreach (var child in children)
            {
                if (child is TextBox textBox)
                {
                    if (string.IsNullOrWhiteSpace(textBox.Text))
                        return null;
                    sb.Append(textBox.Text);
                    sb.Append(',');
                }
                else if (child is TimePicker timePicker)
                {
                    if (!timePicker.SelectedTime.HasValue)
                        return null;
                    sb.Append(timePicker.SelectedTime.Value.ToShortTimeString());
                    sb.Append(',');
                }
                else if (child is ComboBox comboBox)
                {
                    if (comboBox.SelectedIndex == -1)
                        return null;
                    sb.Append((comboBox.SelectedItem as ComboBoxItem).Tag);
                }
            }
            return sb.ToString();
        }
        private void SaveButton_Click(object sender, RoutedEventArgs e)
        {
            Configuration.AppSettings.Settings.Clear();

            foreach (StackPanel innerStackPanel in stackPanel.Children)
            {
                var rowString = RowContentsToString(innerStackPanel.Children);

                if(rowString == null)
                {
                    MessageBox.Show("빈 항목을 모두 입력해주세요.");
                    return;
                }
                
                Configuration.AppSettings.Settings.Add($"UserInfo{Configuration.AppSettings.Settings.Count + 1}", rowString);
                
            }
            Configuration.Save();
            MessageBox.Show(Configuration.FilePath);
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            TwitchClient.Disconnect();
        }

        private async void Window_Loaded(object sender, RoutedEventArgs e)
        {

            await Task.Run(() =>
            {
                Dispatcher.Invoke(() =>
                {
                    if (Configuration.HasFile)
                    {
                        foreach (KeyValueConfigurationElement setting in Configuration.AppSettings.Settings)
                        {
                            var splitTexts = setting.Value.Split(',');
                            var id = splitTexts[0];
                            var startTime = DateTime.Parse(splitTexts[1]);
                            int duration = int.Parse(splitTexts[2]);
                            var banDuration = TimeSpan.FromHours(duration);

                            CreateRow(id, startTime, banDuration);
                        }
                    }
                });
                

                TwitchClient.Initialize(Credentials, "ju10506");
                TwitchClient.Connect();

                TwitchClient.OnMessageReceived += (sender, e) =>
                {
                    Trace.WriteLine($"{e.ChatMessage.DisplayName}: {e.ChatMessage.Message}");
                };
            });
            



            plusButton.Click += (sender, e) =>
            {
                CreateRow();
            };

            startStopButton.Click += async (sender, e) =>
            {
                Running = !Running;

                if (Running)
                {
                    foreach (StackPanel innerStackPanel in stackPanel.Children)
                    {
                        var rowString = RowContentsToString(innerStackPanel.Children);

                        if (rowString == null)
                        {
                            MessageBox.Show("빈 항목을 모두 입력해주세요.");
                            Running = false;
                            return;
                        }

                        var splitTexts = rowString.Split(',');

                        var id = splitTexts[0];
                        var startTime = DateTime.Parse(splitTexts[1]);
                        int duration = int.Parse(splitTexts[2]);
                        var banDuration = TimeSpan.FromHours(duration);


                        Users.Add(new UserInfo { Id = id, StartTime = startTime, BanDuration = banDuration });
                    }

                    foreach (var user in Users)
                    {
                        Timer banTimer = default, unbanTimer = default;

                        if (DateTime.Now < user.StartTime)
                        {
                            banTimer = new Timer(BanTimerCallback, user, user.StartTime - DateTime.Now, TimeSpan.FromDays(1));
                            unbanTimer = new Timer(UnbanTimerCallback, user, user.StartTime + user.BanDuration - DateTime.Now, TimeSpan.FromDays(1));
                        }
                        else if (DateTime.Now >= user.StartTime && DateTime.Now < user.StartTime + user.BanDuration)
                        {
                            banTimer = new Timer(BanTimerCallback, user, user.StartTime.AddDays(1) - DateTime.Now, TimeSpan.FromDays(1));
                            unbanTimer = new Timer(UnbanTimerCallback, user, user.StartTime + user.BanDuration - DateTime.Now, TimeSpan.FromDays(1));

                            BanTimerCallback(user);
                        }
                        else if (DateTime.Now >= user.StartTime + user.BanDuration)
                        {
                            banTimer = new Timer(BanTimerCallback, user, user.StartTime.AddDays(1) - DateTime.Now, TimeSpan.FromDays(1));
                            unbanTimer = new Timer(UnbanTimerCallback, user, user.StartTime.AddDays(1) + user.BanDuration - DateTime.Now, TimeSpan.FromDays(1));
                        }

                        BanTimers.Add(banTimer);
                        UnbanTimers.Add(unbanTimer);
                    }

                    startStopButton.Content = new PackIcon { Kind = PackIconKind.Stop };
                    runningTextBlock.Visibility = Visibility.Visible;
                }
                else
                {
                    startStopButton.Content = new PackIcon { Kind = PackIconKind.Play };
                    runningTextBlock.Visibility = Visibility.Hidden;

                    for (int i = 0; i < BanTimers.Count; ++i)
                    {
                        await BanTimers[i].DisposeAsync();
                        await UnbanTimers[i].DisposeAsync();
                    }
                    BanTimers.Clear();
                    UnbanTimers.Clear();
                }
            };

        }
    }
}

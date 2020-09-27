using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Telegram.Bot;

namespace 리듬_키워드_알리미
{
    public partial class MainWindow : Window
    {
        private static CookieContainer CookieContainer { get; } = new CookieContainer();
        private static HttpClientHandler Handler { get; } = new HttpClientHandler { CookieContainer = CookieContainer };
        public static HttpClient Client { get; } = new HttpClient(Handler);
        private ObservableCollection<string> Keywords { get; } = new ObservableCollection<string>();
        public bool Running { get; set; }
        public TelegramBotClient Bot { get; } = new TelegramBotClient("631711537:AAG164NGqHcdoYUnV-0QzKqE0B39z4a0U_0");

        public MainWindow()
        {
            InitializeComponent();


            Client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.113 Safari/537.36 Edg/81.0.416.62");

            listBox.ItemsSource = Keywords;
            
        }


        private void ListBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Delete)
            {
                Keywords.Remove(listBox.SelectedItem.ToString());

            }
        }

        private void KeywordTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.Key == Key.Enter)
            {
                if (!Keywords.Contains(keywordTextBox.Text))
                {
                    Keywords.Add(keywordTextBox.Text);
                    keywordTextBox.Clear();
                }
            }
        }

        private async void StartStopButton_Click(object sender, RoutedEventArgs e)
        {
            if(Running)
            {
                startStopButton.Content = "중지";
                Running = false;
            }
            else
            {
                if(Keywords.Count == 0)
                {
                    var textBlock = new TextBlock { Text = "키워드를 입력해주세요.", HorizontalAlignment = HorizontalAlignment.Center};
                    stackPanel.Children.Insert(stackPanel.Children.Count - 1, textBlock);
                    await Task.Delay(TimeSpan.FromSeconds(1.5));
                    stackPanel.Children.Remove(textBlock);
                    return;
                }
                Running = true;
                startStopButton.Content = "중지";
                QuasarzoneKeywordAlert quasarzone;
                quasarzone = new QuasarzoneKeywordAlert(new ReadOnlyCollection<string>(Keywords));
                
                await quasarzone.Run();
            }
        }
    }
}

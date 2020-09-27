using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Net.Cache;
using System.Net.Security;
using System.Text;
using System.Threading;
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

namespace 빌런세척기
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Task task;
        private CancellationTokenSource CancellationTokenSource = new CancellationTokenSource();
        private readonly DCManager dc = DCManager.Instance;

        public MainWindow()
        {
            InitializeComponent();

            Dispatcher.UnhandledException += (sender, e) => MessageBox.Show(e.Exception.ToString());

            var configuration = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
            if (!configuration.HasFile)
            {
                configuration.AppSettings.Settings.Add("id", "");
                configuration.AppSettings.Settings.Add("password", "");
                configuration.AppSettings.Settings.Add("nicknames", "");
                configuration.Save();
            }

            var text = configuration.AppSettings.Settings["id"]?.Value;
            idTextBox.Text = text;
            text = configuration.AppSettings.Settings["password"]?.Value;
            passwordTextBox.Text = text;

            foreach (var nickname in configuration.AppSettings.Settings["nicknames"]?.Value.Split(','))
            {
                if(!string.IsNullOrWhiteSpace(nickname))
                    listBox.Items.Add(nickname);
            }
            
        }

        private void TextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.Key == Key.Enter)
            {
                var textBox = sender as TextBox;
                var nickName = textBox.Text;

                if(!(listBox.Items.Contains(nickName) || string.IsNullOrWhiteSpace(nickName)))
                {
                    listBox.Items.Add(nickName);
                    textBox.Clear();
                }
            }
        }

        private void ListBox_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.Key == Key.Delete)
            {
                var selectedIndex = listBox.SelectedIndex;
                var items = listBox.SelectedItems;
                for(int i = items.Count - 1; i >= 0; --i)
                    listBox.Items.Remove(items[i]);
                listBox.SelectedIndex = selectedIndex;
                listBox.Focus();  
            }
        }

        private void SaveButton_Click(object sender, RoutedEventArgs e)
        {
            var text = idTextBox.Text;
            Configuration configuration = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);

            if(!string.IsNullOrWhiteSpace(text))
                configuration.AppSettings.Settings.Add("id", text);
         
            text = passwordTextBox.Text;
            if (!string.IsNullOrWhiteSpace(text))
                configuration.AppSettings.Settings.Add("password", text);


            if (listBox.Items.Count != 0)
                text = listBox.Items.OfType<string>().Aggregate((a, b) => $"{a},{b}");
            else
                text = "";
            configuration.AppSettings.Settings.Remove("nicknames");
            configuration.AppSettings.Settings.Add("nicknames", text);
            configuration.Save();
        }

        private async void ToggleButton_Click(object sender, RoutedEventArgs e)
        { 
            var galleryId = (galleryComboBox.SelectedItem as ComboBoxItem)?.Tag?.ToString();
            var toggleButton = sender as ToggleButton;

            if (string.IsNullOrWhiteSpace(idTextBox.Text) || string.IsNullOrWhiteSpace(passwordTextBox.Text))
            {
                MessageBox.Show("아이디 및 비번을 쳐 주셈!");
                toggleButton.IsChecked = false;
                toggleButton.Content = "Off";
                return;
            }

            if (string.IsNullOrWhiteSpace(galleryId))
            {
                MessageBox.Show("갤러리 선택해주셈!");
                toggleButton.IsChecked = false;
                toggleButton.Content = "Off";
                return;
            }
     
            
            if (toggleButton.IsChecked.Value)
            {
                toggleButton.Content = "On";
                if(galleryComboBox.SelectedItem == null)
                {
                    if (stackPanel.Children[0] is TextBlock)
                        return;
                    var textBlock = new TextBlock { Text = "갤러리를   선택하셈!", TextWrapping = TextWrapping.Wrap };
                    stackPanel.Children.Insert(0, textBlock);
                    await Task.Delay(TimeSpan.FromSeconds(1.5));
                    stackPanel.Children.RemoveAt(0);
                    toggleButton.IsChecked = false;
                    toggleButton.Content = "Off";
                    return;
                }
                var nicknames = listBox.Items.OfType<string>().ToArray();
                task = Task.Run(async () =>
                {
                    if(!await Dispatcher.Invoke(async () =>
                    {
                        if (!await dc.Login(idTextBox.Text, passwordTextBox.Text))
                        {
                            var textBlock = new TextBlock { Text = "로그인 실패!" };
                            stackPanel.Children.Insert(0, textBlock);
                            await Task.Delay(TimeSpan.FromSeconds(1.5));
                            stackPanel.Children.RemoveAt(0);
                            toggleButton.IsChecked = false;
                            toggleButton.Content = "Off";
                            return false;
                        }

                        if (!await dc.HasPermission(galleryId, true))
                        {
                            MessageBox.Show("글삭 권한 없음!");
                            toggleButton.IsChecked = false;
                            toggleButton.Content = "Off";
                            return false; 
                        }
                        return true;

                    }))
                        return;
                    
   
                    while(!CancellationTokenSource.IsCancellationRequested)
                    {
                        var articleNumbers = await dc.GetSpamArticlesNumbers(galleryId, true, nicknames, 1);

                        if (articleNumbers.Count != 0)
                        {
                            MessageBox.Show("11");
                            await dc.DeleteArticles(galleryId, articleNumbers.ToArray());
                            MessageBox.Show("22");
                            Console.WriteLine(articleNumbers);
                        }

                        await Task.Delay(TimeSpan.FromSeconds(10));
                    }
                }, CancellationTokenSource.Token).ContinueWith((t) =>
                {
                    CancellationTokenSource.Dispose();
                    CancellationTokenSource = new CancellationTokenSource();
                });
            }
            else
            { 
                toggleButton.Content = "Off";
                CancellationTokenSource.Cancel();
            }
                
        }

    }
}

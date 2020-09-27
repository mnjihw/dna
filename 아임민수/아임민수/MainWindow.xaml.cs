using System;
using System.Diagnostics;
using System.Net.Http;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Interop;
using Newtonsoft.Json.Linq;

namespace 아임민수
{
    public partial class MainWindow : Window
    {
        [DllImport("user32.dll")]
        private static extern bool FlashWindow(IntPtr hwnd, bool bInvert);

        public MainWindow()
        {
            InitializeComponent();
        }

        private async void Window_Loaded(object sender, RoutedEventArgs e)
        {
            HttpClient httpClient = new HttpClient();
            int articleCount = 0;
            var beepAction = new Action(() => Console.Beep(2000, 800));
            var hwnd = new WindowInteropHelper(this).EnsureHandle();

            while (true)
            {
                var result = await httpClient.GetStringAsync("https://www.seek.com.au/api/chalice-search/search?siteKey=AU-Main&sourcesystem=houston&userqueryid=e919b1a1e63aa981eb5a2a17b78af1a0-7538250&userid=11b3bd571ff4c6bf04f50b019229d539&usersessionid=11b3bd571ff4c6bf04f50b019229d539&eventCaptureSessionId=11b3bd571ff4c6bf04f50b019229d539&where=All+Adelaide+SA&page=1&seekSelectAllPages=true&keywords=graduate&classification=1206,1209&subclassification=6027&graduatesearch=true&include=seodata&isDesktop=true&solId=aae38a2e-f7aa-451b-a325-e16b6a12b96b");
                var jArray = JObject.Parse(result)["data"] as JArray;


                if (articleCount != 0 && articleCount < jArray.Count)
                {
                    beepAction.BeginInvoke(null, null);
                    FlashWindow(hwnd, false);
                }

                articleCount = jArray.Count;
                listBox.Items.Clear();

                for (int i = 0; i < jArray.Count; ++i)
                {
                    var item = new ListBoxItem
                    {
                        Tag = $"https://www.seek.com.au/job/{jArray[i]["id"]}",
                        Content = $"{jArray[i]["title"]} - {jArray[i]["advertiser"]["description"]} ({(DateTime.Now - DateTime.Parse(jArray[i]["listingDate"].ToString())).Days}d ago)",
                        Height = 30
                    };
                    listBox.Items.Add(item);
                }
                await Task.Delay(TimeSpan.FromHours(1));
            }
        }

        private void ListBox_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if(e.LeftButton == MouseButtonState.Pressed)
            {
                var item = (sender as ListBox).SelectedItem as ListBoxItem;
                Process.Start(item.Tag as string);
            }
        }
    }
}

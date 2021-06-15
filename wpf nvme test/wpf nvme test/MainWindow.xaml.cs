using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Runtime.InteropServices;
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

namespace wpf_nvme_test
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {

        

        public MainWindow()
        {
            InitializeComponent();

            DataContext = new ViewModel.MainViewModel();
        }

        [DllImport("wininet.dll", CharSet = CharSet.Auto, SetLastError = true)]
        static extern bool InternetSetCookieEx(string lpszUrlName, string lpszCookieName, string lpszCookieData, uint dwFlags, IntPtr dwReserved);
        private async void Window_Loaded(object sender, RoutedEventArgs e)
        {

            var cookie = new CookieContainer();
            var handler = new HttpClientHandler { CookieContainer = cookie };
            var client = new HttpClient(handler);

            client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Safari/537.36 Edg/87.0.664.66");
            client.DefaultRequestHeaders.Add("Update-Insecure-Requests", "1");
            client.DefaultRequestHeaders.Add("Referer", "https://www.ppomppu.co.kr/zboard/login.php");
            client.DefaultRequestHeaders.Add("Origin", "https://www.ppomppu.co.kr");
            client.DefaultRequestHeaders.Add("Host", "www.ppomppu.co.kr");

            var postData = new Dictionary<string, string>
            {
                ["s_url"] = "%2F",
                ["user_id"] = "mnjihw",
                ["password"] = "flema0210"
            };
            Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);


            var result = await (await client.PostAsync("https://www.ppomppu.co.kr/zboard/login_check.php?s_url=%2F", new FormUrlEncodedContent(postData))).Content.ReadAsStringAsync();
            result = await client.GetStringAsync("http://www.ppomppu.co.kr/");
            
            Console.WriteLine(result.Contains("리듬"));

            var s = cookie.GetCookieHeader(new Uri("http://www.ppomppu.co.kr"));
            Application.SetCookie(new Uri("http://www.ppomppu.co.kr"), s);

          /*  var sb = new StringBuilder();


            foreach(Cookie c in cookie.GetCookies(new Uri("https://www.ppomppu.co.kr")))
            {
                InternetSetCookieEx("https://www.ppomppu.co.kr", c.Name, c.Value, 0x2000, IntPtr.Zero);
                sb.Append($"{c.Name}={c.Value}; ");
            }
*/
            browser.Navigate("https://www.ppomppu.co.kr");
            //browser.Navigate("https://www.ppomppu.co.kr", "_blank", null, "Cookie: " + sb.ToString());

        }
    }
   

}

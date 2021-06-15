using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
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

namespace instagram
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

      
        private async void Window_Loaded(object sender, RoutedEventArgs e)
        {

            var random = new Random();
            var igManager = InstagramManager.Instance;

            if (!await igManager.Login("larp.royal", "rB$i%QcXN*8Z4o1"))
            {
                Console.WriteLine("로그인 실패!");
                return;
            }
            var list = new List<(string, InstagramPhoto)>();
            for (int i = 0; ; ++i)
            {
                list.Clear();
                await igManager.ArchivePictures(i, list);
                
                //await Task.Delay(3000 + (random.Next() % 500) + 300);
                foreach(var item in list)
                {
                    var s = new StackPanel();
                    s.Children.Add(new Image { Source = new BitmapImage(new Uri(item.Item1)), Width = 200, Height = 200 });
                    s.Children.Add(new TextBlock {Text = item.Item2.Name + " " + item.Item2.Id });
                    wrap.Children.Add(s);

                    //wrap.Children.Add(new Image { Source = new BitmapImage(new Uri(item.Item1)), Width = 200, Height = 200});
                }
                await Task.Delay(3000);
            }
        }
    }
}

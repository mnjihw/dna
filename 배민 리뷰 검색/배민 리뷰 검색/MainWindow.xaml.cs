using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using System.Web;
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

namespace 배민_리뷰_검색
{
    
    public partial class MainWindow : Window
    {
        private static HttpClient Client { get; } = new HttpClient();
        private static double Latitude { get; } = 35.882825;
        private static double Longitude { get; } = 128.671131;
        
        public MainWindow()
        {
            InitializeComponent();
            restaurantTextBox.Focus();
        }

        private async void RestaurantSearchButton_Click(object sender, RoutedEventArgs e)
        {
            var restaurantName = restaurantTextBox.Text;

            if (string.IsNullOrWhiteSpace(restaurantName))
            {
                notificationTextBlock.Text = "음식점 이름을 입력해주세요!";
                notificationTextBlock.Visibility = Visibility.Visible;
                await Task.Delay(TimeSpan.FromSeconds(1.5));
                notificationTextBlock.Visibility = Visibility.Hidden;
                return;
            }
            restaurantComboBox.Items.Clear();
            var result = await Client.GetStringAsync($"http://shopdp-api.baemin.com/v1/SEARCH/shops?keyword={HttpUtility.UrlEncode(restaurantName)}&sort=SORT__DEFAULT&kind=DEFAULT&limit=25&latitude={Latitude}&longitude={Longitude}");
            var json = JsonDocument.Parse(result);

            foreach (var shop in json.RootElement.GetProperty("data").GetProperty("shops").EnumerateArray())
            {
                var shopInformation = shop.GetProperty("shopInfo");
                var name = shopInformation.GetProperty("shopName").GetString();
                var id = shopInformation.GetProperty("shopNumber").GetInt32();
                var restaurant = new Restaurant { Name = name, Id = id };
                restaurantComboBox.Items.Add(restaurant);
            }

            restaurantComboBox.IsDropDownOpen = true;

        

        }

        private async void ReviewSearchButton_Click(object sender, RoutedEventArgs e)
        {
            if(restaurantComboBox.SelectedItem == null)
            {
                notificationTextBlock.Text = "먼저 음식점을 검색 후 선택한 뒤 검색해주세요!";
                notificationTextBlock.Visibility = Visibility.Visible;
                await Task.Delay(TimeSpan.FromSeconds(1.5));
                notificationTextBlock.Visibility = Visibility.Hidden;
                return;
            }
            var keyword = menuTextBox.Text;

            if (string.IsNullOrWhiteSpace(keyword))
            {
                notificationTextBlock.Text = "메뉴를 입력해주세요!";
                notificationTextBlock.Visibility = Visibility.Visible;
                await Task.Delay(TimeSpan.FromSeconds(1.5));
                notificationTextBlock.Visibility = Visibility.Hidden;
                return;
            }



            Client.DefaultRequestHeaders.Clear();
            Client.DefaultRequestHeaders.Add("Authorization", "bearer guest");
            var restaurantId = (restaurantComboBox.SelectedItem as Restaurant).Id;
            const int limit = 50;

            
            var result = await Client.GetStringAsync($"http://review-api.baemin.com/v1/shops/{restaurantId}/reviews?sort=MOST_RECENT&filter=ALL&offset=0&limit={limit}&adid=NONE&osver=22&oscd=2");
            var json = JsonDocument.Parse(result);
            var reviews = json.RootElement.GetProperty("data").GetProperty("reviews");

            wrapPanel.Children.Clear();

            
            foreach (var review in reviews.EnumerateArray())
            {
                foreach (var menu in review.GetProperty("menus").EnumerateArray())
                {
                    var menuName = menu.GetProperty("name").GetString();

                    if (menuName.Contains(keyword) && review.TryGetProperty("images", out var images))
                    {
                        foreach (var image in images.EnumerateArray())
                        {
                            var url = image.GetProperty("url").GetString();
                            
                            var bitmapImage = new BitmapImage();
                            bitmapImage.BeginInit();
                            bitmapImage.UriSource = new Uri(url);
                            bitmapImage.EndInit();


                            var img = new Image { Source = bitmapImage, Width = 200, Height = 200 };
                            var textBlock = new TextBlock { Text = menuName};
                            var stackPanel = new StackPanel { };
                            stackPanel.Children.Add(img);
                            stackPanel.Children.Add(textBlock);
                            wrapPanel.Children.Add(stackPanel);
                            


                        }
                    }

                }



                /*if(review.TryGetProperty("contents", out var contents) && !string.IsNullOrWhiteSpace(contents.GetString()))
                {
                    Console.WriteLine("리뷰: " + contents.GetString());
                }*/

            }
        }

        private void RestaurantTextBox_KeyUp(object sender, KeyEventArgs e)
        {
            if(e.Key == Key.Enter)
                restaurantSearchButton.RaiseEvent(new RoutedEventArgs(ButtonBase.ClickEvent));

        }

        private void MenuTextBox_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
                reviewSearchButton.RaiseEvent(new RoutedEventArgs(ButtonBase.ClickEvent));

        }
    }
}

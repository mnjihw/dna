using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Data;
using System.Linq;
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
using System.ComponentModel;
using System.Diagnostics;
using System.Collections;
using System.Globalization;
using System.Timers;
using System.IO;

namespace 리듬_코인_트레이서
{
    public class MyData
    {
        public string Name { get; set; }
        public string Binance { get; set; }
        public string Bithumb { get; set; }
        public string Gateio { get; set; }
        public string Huobi { get; set; }
        public string Upbit { get; set; }

        public Brush ColorOfBinance { get; set; }
        public Brush ColorOfBithumb { get; set; }
        public Brush ColorOfGateio { get; set; }
        public Brush ColorOfHuobi { get; set; }
        public Brush ColorOfUpbit { get; set; }

    }

    public partial class MainWindow : Window
    {
        public string NameOfHeaderClicked { get; set; }
        public ListSortDirection LastDirection { get; set; } = ListSortDirection.Ascending;
        private GridViewColumnHeader LastHeaderClicked { get; set; }

        private readonly ObservableCollection<MyData> items = new ObservableCollection<MyData>();
        private readonly ConcurrentDictionary<string, PricesByExchange> pricesByExchange = new ConcurrentDictionary<string, PricesByExchange>();
        private ConcurrentDictionary<string, PricesByExchange> prices5MinutesAgoByExchange;
        private readonly Timer timer = new Timer();
        private string BaseExchange { get; set; } = "Binance";
        private double CurrentUSDToKRWExchangeRate { get; set; }
        private double CurrentUSDTToUSDExchangeRate { get; set; }

        public enum Currency
        {
            BTC,
            KRW,
            USDT
        }
        public MainWindow()
        {
            InitializeComponent();
            Dispatcher.UnhandledException += Dispatcher_UnhandledException;
        }

        private void Dispatcher_UnhandledException(object sender, System.Windows.Threading.DispatcherUnhandledExceptionEventArgs e)
        {
            MessageBox.Show(e.Exception.ToString());
        }

        private string GetPriceStringByCurrency(string exchangeName, KeyValuePair<string, PricesByExchange> prices)
        {
            double price = (double)typeof(PricesByExchange).GetProperty(exchangeName).GetValue(prices.Value);
            double basePrice = (double)typeof(PricesByExchange).GetProperty(BaseExchange).GetValue(prices.Value);
            double priceInKRW, basePriceInKRW = 0;
            Currency currency, baseCurrency;

            if (price == 0)
                return "-";            

            switch(exchangeName)
            {
                case "Bithumb":
                    currency = Currency.KRW;
                    break;
                default:
                    if (prices.Key.IndexOf('/') != -1)
                        currency = Currency.BTC;
                    else
                    {
                        if (exchangeName == "Upbit")
                            currency = Currency.KRW;
                        else
                            currency = Currency.USDT;
                    }
                    break;
            }
            if (currency == Currency.BTC)
                priceInKRW = price * pricesByExchange["BTC"].Upbit;
            else if (currency == Currency.USDT)
                priceInKRW = price * CurrentUSDTToUSDExchangeRate * CurrentUSDToKRWExchangeRate;
            else
                priceInKRW = price;

            if (basePrice != 0)
            {
                switch (BaseExchange)
                {
                    case "Bithumb":
                        baseCurrency = Currency.KRW;
                        break;
                    default:
                        if (prices.Key.IndexOf('/') != -1)
                            baseCurrency = Currency.BTC;
                        else
                        {
                            if(BaseExchange == "Upbit")
                                baseCurrency = Currency.KRW;
                            else
                                baseCurrency = Currency.USDT;
                        }
                        break;
                }

                if (baseCurrency == Currency.BTC)
                    basePriceInKRW = basePrice * pricesByExchange["BTC"].Upbit;
                else if (baseCurrency == Currency.USDT)
                    basePriceInKRW = basePrice * CurrentUSDTToUSDExchangeRate * CurrentUSDToKRWExchangeRate;
                else
                    basePriceInKRW = basePrice;
            }
            
            var format = "#,##0." + new string('#', currency == Currency.BTC ? 8 : ((int)price).ToString().Length > 5 ? 0 : 5 - ((int)price).ToString().Length);


            if (exchangeName == "Bithumb")
            {
                //Console.WriteLine($"빗썸 {price} {price.ToString(format)}");
            }

            switch (currency)
            {
                case Currency.BTC:
                    if (exchangeName == BaseExchange)
                        return $"₿ {price.ToString(format)} (₩ {price * pricesByExchange["BTC"].Upbit:#,##0.##})";
                    else
                        return $"₿ {price.ToString(format)} {(basePrice != 0 ? $"({(priceInKRW - basePriceInKRW) / basePriceInKRW:#0.##%}) " : "")}(₩ {priceInKRW:#,##0.##})";
                case Currency.KRW:
                    if (exchangeName == BaseExchange)
                        return $"$ {(price / (CurrentUSDToKRWExchangeRate * CurrentUSDTToUSDExchangeRate)).ToString(format)} (₩ {price:#,##0.##})";
                    else
                        return $"$ {(price / (CurrentUSDToKRWExchangeRate * CurrentUSDTToUSDExchangeRate)).ToString(format)} {(basePrice != 0 ? $"({(priceInKRW - basePriceInKRW) / basePriceInKRW:#0.##%}) " : "")}(₩ {priceInKRW:#,##0.##})";
                case Currency.USDT:
                    if (exchangeName == BaseExchange)
                        return $"$ {price.ToString(format)} (₩ {priceInKRW:#,##0.##})";
                    else
                        return $"$ {price.ToString(format)} {(basePrice != 0 ? $"({(priceInKRW - basePriceInKRW) / basePriceInKRW:#0.##%}) " : "")}(₩ {priceInKRW:#,##0.##})";
            }

            return null;
        }

        private async void Window_Loaded(object sender, RoutedEventArgs e)
        {
            LastHeaderClicked = nameHeader.Header as GridViewColumnHeader;
            bool sortedOnce = false;
            int requestedCount = 0;
            IExchange bithumb = Bithumb.Instance;
            IExchange binance = Binance.Instance;
            IExchange upbit = Upbit.Instance;
            IExchange gateio = Gateio.Instance;
            IExchange huobi = Huobi.Instance;
            DateTime datetime = DateTime.Now;

            Task<double> USDToKRWTask = null, USDTToUSDTask;
            DateTime after5Minutes = DateTime.Now;

            after5Minutes = after5Minutes.AddSeconds(-after5Minutes.Second).AddMilliseconds(-after5Minutes.Millisecond);

            do
                after5Minutes = after5Minutes.AddMinutes(1);
            while (after5Minutes.Minute % 5 != 0);

            timer.Interval = (after5Minutes - DateTime.Now).TotalMilliseconds;
            timer.Elapsed += (_sender, _e) =>
            {
                timer.Interval = TimeSpan.FromMinutes(5).TotalMilliseconds;
                prices5MinutesAgoByExchange = new ConcurrentDictionary<string, PricesByExchange>(pricesByExchange);
            };
            timer.Start();


            while (true)
            {
                await Task.WhenAll(new Task[]
                {
                    (requestedCount += 1) % (60 * 30) == 1 ? USDToKRWTask = ExchangeRate.GetUSDToKRWExchangeRate() : Task.Run(() => { }),
                    binance.GetCurrentPrices(pricesByExchange),
                    bithumb.GetCurrentPrices(pricesByExchange),
                    gateio.GetCurrentPrices(pricesByExchange),
                    huobi.GetCurrentPrices(pricesByExchange),
                    upbit.GetCurrentPrices(pricesByExchange),
                    USDTToUSDTask = Cryptowatch.GetUSDTToUSDExchangeRate()
                }) ;

                if (prices5MinutesAgoByExchange == null)
                    prices5MinutesAgoByExchange = new ConcurrentDictionary<string, PricesByExchange>(pricesByExchange);
                

                if(requestedCount == 1)
                    CurrentUSDToKRWExchangeRate = await USDToKRWTask;                
                CurrentUSDTToUSDExchangeRate = await USDTToUSDTask;

                int count = 0;

                var name = (listView.SelectedItem as MyData)?.Name;
                

                items.Clear();
                listView.ItemsSource = null;
                listView.ItemsSource = items;

                foreach (var prices in pricesByExchange)
                {
                    foreach (var prop in typeof(PricesByExchange).GetProperties())
                    {
                        if (prop.PropertyType == typeof(double) && (double)prop.GetValue(prices.Value) != 0)
                        {
                            
                            ++count;
                        }

                    }
                    if (count >= 2)
                    {

                        items.Add(new MyData
                        {
                            Name = prices.Key,
                            Binance = GetPriceStringByCurrency("Binance", prices),
                            Bithumb = GetPriceStringByCurrency("Bithumb", prices),
                            Gateio = GetPriceStringByCurrency("Gateio", prices),
                            Huobi = GetPriceStringByCurrency("Huobi", prices),
                            Upbit = GetPriceStringByCurrency("Upbit", prices),
                            ColorOfBinance = Brushes.LightBlue,
                            ColorOfBithumb = Brushes.LightGray,
                            ColorOfGateio = Brushes.LightGoldenrodYellow,
                            ColorOfHuobi = Brushes.LightPink,
                            ColorOfUpbit = Brushes.LightSeaGreen
                        });

                    }
                    count = 0;
                }
                if (name != null)
                    listView.SelectedItem = items.Single(item => item.Name == name);
                if (!sortedOnce)
                {
                    Sort("Name", ListSortDirection.Ascending);
                    sortedOnce = true;
                }
            
                await Task.Delay(1000);
                pricesByExchange.Clear();
            }

        }

        private void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            bool isFirstTime = true;
            MyData firstItem = null;

            listView.Items.Filter = listViewItem =>
            {
                var item = listViewItem as MyData;
                var result = item.Name.IndexOf((sender as TextBox).Text, StringComparison.OrdinalIgnoreCase) != -1;

                if (result && isFirstTime)
                {
                    isFirstTime = false;
                    firstItem = listViewItem as MyData;
                }
                return result;
            };

            listView.ScrollIntoView(firstItem);
            listView.SelectedItem = firstItem;
        }

        private void GridViewColumnHeader_Click(object sender, RoutedEventArgs e)
        {
            ListSortDirection direction;

            if (e.OriginalSource is GridViewColumnHeader headerClicked)
            {
                if (headerClicked.Role != GridViewColumnHeaderRole.Padding)
                {
                    string header = (headerClicked.Column.CellTemplate.LoadContent() as TextBlock).GetBindingExpression(TextBlock.TextProperty).ParentBinding.Path.Path;
                    NameOfHeaderClicked = header;

                    if (headerClicked != LastHeaderClicked)
                        direction = header == "Name" ? ListSortDirection.Ascending : ListSortDirection.Descending;
                    else
                    {
                        if (LastDirection == ListSortDirection.Ascending)
                            direction = ListSortDirection.Descending;
                        else
                            direction = ListSortDirection.Ascending;
                    }
                    LastHeaderClicked = headerClicked;
                    LastDirection = direction;

                    Sort(header, direction);
                }
            }
        }

        private void Sort(string sortBy, ListSortDirection direction)
        {
            ListCollectionView listCollectionView = (ListCollectionView)CollectionViewSource.GetDefaultView(listView.ItemsSource);

            if (sortBy == "Name")
            {
                listCollectionView.CustomSort = null;
                listCollectionView.SortDescriptions.Clear();
                listCollectionView.SortDescriptions.Add(new SortDescription(sortBy, direction));
            }
            else
                listCollectionView.CustomSort = new CustomComparer();

            listCollectionView.Refresh();
        }

        public class CustomComparer : IComparer
        {
            public int Compare(object x, object y)
            {
                var firstValue = typeof(MyData).GetProperty((Application.Current.MainWindow as MainWindow).NameOfHeaderClicked).GetValue(x).ToString().Replace("₩", "").Replace("$", "").Replace("₿", "");
                var secondValue = typeof(MyData).GetProperty((Application.Current.MainWindow as MainWindow).NameOfHeaderClicked).GetValue(y).ToString().Replace("₩", "").Replace("$", "").Replace("₿", "");
                var sign = (Application.Current.MainWindow as MainWindow).LastDirection == ListSortDirection.Ascending ? -1 : 1;
                
                if (firstValue == "-")
                    return 1;
                else if (secondValue == "-")
                    return -1;

                return (double.Parse(secondValue.Split(' ')[1]) - double.Parse(firstValue.Split(' ')[1]) > 0 ? 1 : -1) * sign;
            }
        }

        private void RadioButton_Click(object sender, RoutedEventArgs e)
        {
            BaseExchange = (sender as RadioButton).Tag as string;

            int count = 0;

            items.Clear();
            listView.ItemsSource = null;
            listView.ItemsSource = items;

            foreach (var prices in pricesByExchange)
            {
                foreach (var prop in typeof(PricesByExchange).GetProperties())
                {
                    if (prop.PropertyType == typeof(double) && (double)prop.GetValue(prices.Value) != 0)
                    {
                        ++count;
                    }

                }
                if (count >= 2)
                {
                    items.Add(new MyData
                    {
                        Name = prices.Key,
                        Binance = GetPriceStringByCurrency("Binance", prices),
                        Bithumb = GetPriceStringByCurrency("Bithumb", prices),
                        Gateio = GetPriceStringByCurrency("Gateio", prices),
                        Huobi = GetPriceStringByCurrency("Huobi", prices),
                        Upbit = GetPriceStringByCurrency("Upbit", prices),
                        ColorOfBinance = Brushes.LightBlue,
                        ColorOfBithumb = Brushes.LightGray,
                        ColorOfGateio = Brushes.LightGoldenrodYellow,
                        ColorOfHuobi = Brushes.LightPink,
                        ColorOfUpbit = Brushes.LightSeaGreen
                    });

                }
                count = 0;
            }

            
        }
    }
}

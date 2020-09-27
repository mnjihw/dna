using System;
using System.Windows;
using System.Windows.Controls;
using Telegram.Bot.Types.Enums;
using Telegram.Bot.Types;
using System.Threading.Tasks;
using System.IO;
using System.Collections.Generic;


namespace 리듬_군전세객차_알리미
{
    public partial class MainWindow : Window
    {   
        private readonly Telegram.Bot.TelegramBotClient bot = new Telegram.Bot.TelegramBotClient("631711537:AAG164NGqHcdoYUnV-0QzKqE0B39z4a0U_0");
        private readonly string chatId = "581720134";//나
        //private readonly string chatId = "1041568091";//김동규하사
        private readonly HashSet<string> notifiedQueryResults = new HashSet<string>();
        private readonly Random random = new Random();

        public MainWindow()
        {
            InitializeComponent();
            Dispatcher.UnhandledException += Dispatcher_UnhandledException;
        }
        private async void Button_Click(object sender, RoutedEventArgs e)
        {
            DTISManager dtis = DTISManager.Instance;
            
            (sender as Button).IsEnabled = false;
            

            if (goingDatePicker.SelectedDate == null || (roundTripRadioButton.IsChecked.Value && comingDatePicker.SelectedDate == null))
            {
                MessageBox.Show("날짜를 제대로 선택해주세요.");
                (sender as Button).IsEnabled = true;
                return;
            }


            if(!await dtis.Login(DTISManager.MilitaryType.AirForce, idTextBox.Text, passwordTextBox.Text))
            {
                MessageBox.Show("로그인 실패! 아이디나 비밀번호를 확인해주세요.");
                (sender as Button).IsEnabled = true;
                return;
            }
            listBox.Items.Add($"[{DateTime.Now:HH:mm:ss}] 로그인 성공!");
            dtis.queryResults.Clear();

            var section = new HashSet<string> { (departureStationComboBox1.SelectedItem as ComboBoxItem)?.Content as string, (arrivalStationComboBox1.SelectedItem as ComboBoxItem)?.Content as string };
            var section2 = new HashSet<string> { (departureStationComboBox2.SelectedItem as ComboBoxItem)?.Content as string, (arrivalStationComboBox2.SelectedItem as ComboBoxItem)?.Content as string };

            dtis.desiredStationsDatas[dtis.desiredStationsDatas.Length - 1] = (new HashSet<string> { (departureStationComboBox3.SelectedItem as ComboBoxItem)?.Content as string ?? "빔", (arrivalStationComboBox3.SelectedItem as ComboBoxItem)?.Content as string ?? "빔2"}, 50000);
    
            foreach (var stationData in dtis.desiredStationsDatas)
            {
                if (stationData.desiredStation.SetEquals(section))
                    dtis.reservedTrains.Add((section, stationData.price));
                else if (stationData.desiredStation.SetEquals(section2))
                    dtis.reservedTrains.Add((section2, stationData.price));
            }

            var trainsInfo = await dtis.GetTrainsInformation(upTrainRadioButton.IsChecked.Value ? ("부산역", "서울역") : ("서울역", "부산역"), roundTripRadioButton.IsChecked.Value, groupBox, goingDatePicker.Text, comingDatePicker.Text);

            while (true)
            {
                foreach (var trainInfo in trainsInfo)
                    await dtis.CheckForAvailableSeats(trainInfo);
                
                foreach (var queryResult in dtis.queryResults)
                {
                    if(!notifiedQueryResults.Contains(queryResult))
                    {
                        await bot.SendTextMessageAsync(chatId, queryResult);
                        listBox.Items.Add(queryResult);
                        notifiedQueryResults.Add(queryResult);
                    }
                }
                //listBox.Items.Add($"[{DateTime.Now.ToString("HH:mm:ss")}] 끝");


                await Task.Delay(TimeSpan.FromSeconds(10 + random.Next(10)));
            }

        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            roundTripRadioButton.Checked += RadioButton_Checked;
            oneWayRadioButton.Checked += RadioButton_Checked;
            idTextBox.Text = "19-501297";
            passwordTextBox.Text = "flema0288!";
        }

        private void RadioButton_Checked(object sender, RoutedEventArgs e)
        {   
            if((sender as RadioButton).Name == "roundTripRadioButton")
            {
                comingDatePicker.Visibility = Visibility.Visible;
                upTrainRadioButton.IsChecked = true;
                upTrainRadioButton.IsEnabled = false;
                downTrainRadioButton.IsEnabled = false;
            }
            else
            {
                comingDatePicker.Visibility = Visibility.Hidden;
                upTrainRadioButton.IsEnabled = true;
                downTrainRadioButton.IsEnabled = true;
            }
        }
        private void Dispatcher_UnhandledException(object sender, System.Windows.Threading.DispatcherUnhandledExceptionEventArgs e) => MessageBox.Show(e.Exception.ToString());

        
    }
}

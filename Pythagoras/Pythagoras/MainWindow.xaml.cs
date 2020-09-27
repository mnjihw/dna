using System;
using System.Linq;
using System.Windows;
using System.Windows.Input;

namespace Pythagoras
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            hypotenuseTextBox.Focus();
        }
        private void TextBox_KeyUp(object sender, KeyEventArgs e)
        {
            if(e.Key == Key.Enter)
            {
                double[] arr = new double[5];
                /* 0 빗변
                 * 1 밑변
                 * 2 높이
                 * 3 각도1
                 * 4 각도2
                 */

                double.TryParse(hypotenuseTextBox.Text, out arr[0]);
                double.TryParse(baseTextBox.Text, out arr[1]);
                double.TryParse(heightTextBox.Text, out arr[2]);
                double.TryParse(angleTextBox.Text, out arr[3]);
                double.TryParse(angleTextBox2.Text, out arr[4]);


                if(arr[0] != 0) //빗변
                {
                    if (arr[1] != 0) //밑변
                    {
                        arr[2] = Math.Sqrt(Math.Pow(arr[0], 2) - Math.Pow(arr[1], 2)); //높이
                        arr[3] = Math.Acos(arr[1] / arr[0]) * (180 / Math.PI); //각도1
                        arr[4] = 90 - arr[3]; //각도2
                    }
                    else if (arr[2] != 0) //높이
                    {
                        arr[1] = Math.Sqrt(Math.Pow(arr[0], 2) - Math.Pow(arr[2], 2)); //밑변
                        arr[3] = Math.Asin(arr[2] / arr[0]) * (180 / Math.PI); //각도1
                        arr[4] = 90 - arr[3]; //각도2
                    }
                    else if (arr[3] != 0 || arr[4] != 0) //각도1, 2
                    {
                        if(arr[4] != 0) //각도2가 존재하면
                            arr[3] = 90 - arr[4]; //각도1

                        arr[1] = arr[0] * Math.Cos(arr[3] * Math.PI / 180); //밑변
                        arr[2] = arr[0] * Math.Sin(arr[3] * Math.PI / 180); //높이
                        arr[4] = 90 - arr[3]; //각도2
                    }
                    else
                    {
                        textBlock.Text = "두 개 이상의 값을 입력해주세요.";
                        return;
                    }
                }
                else if(arr[1] != 0) //밑변
                {
                    if(arr[2] != 0) //높이
                    {
                        arr[0] = Math.Sqrt(Math.Pow(arr[1], 2) + Math.Pow(arr[2], 2)); //빗변
                        arr[3] = Math.Atan(arr[2] / arr[1]) * (180 / Math.PI); //각도1
                        arr[4] = 90 - arr[3]; //각도2

                    }
                    else if(arr[3] != 0 || arr[4] != 0) //각도1, 2
                    {
                        if (arr[4] != 0)
                            arr[3] = 90 - arr[4]; //각도1

                        arr[0] = arr[1] / Math.Cos(arr[3] * Math.PI / 180); //빗변
                        arr[2] = arr[1] * Math.Tan(arr[3] * Math.PI / 180); //높이
                        arr[4] = 90 - arr[3]; //각도2
                    }
                    else
                    {
                        textBlock.Text = "두 개 이상의 값을 입력해주세요.";
                        return;
                    }
                }
                else if(arr[2] != 0) //높이
                {
                    if(arr[3] != 0 || arr[4] != 0) //각도1, 2
                    {
                        if (arr[4] != 0)
                            arr[3] = 90 - arr[4]; //각도1

                        arr[0] = arr[2] / Math.Sin(arr[3] * Math.PI / 180); //빗변
                        arr[1] = arr[2] / Math.Tan(arr[3] * Math.PI / 180); //밑변
                        arr[4] = 90 - arr[3]; //각도2
                    }
                    else
                    {
                        textBlock.Text = "두 개 이상의 값을 입력해주세요.";
                        return;
                    }
                }
                else if(arr[3] != 0 && arr[4] != 0) //각도만 입력된 경우
                {
                    textBlock.Text = $"각도만 주어질 경우 존재하는 삼각형의 개수는 무한하니{Environment.NewLine}제대로 입력해주세요.";
                    return;
                }
                else
                {
                    textBlock.Text = "두 개 이상의 값을 입력해주세요.";
                    return;
                }

                textBlock.Text = "";
                hypotenuseTextBox.Text = $"{arr[0]:0.##}";
                baseTextBox.Text = $"{arr[1]:0.##}";
                heightTextBox.Text = $"{arr[2]:0.##}";
                angleTextBox.Text = $"{arr[3]:0.#}";
                angleTextBox2.Text = $"{arr[4]:0.#}";

                
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            hypotenuseTextBox.Clear();
            baseTextBox.Clear();
            heightTextBox.Clear();
            angleTextBox.Clear();
            angleTextBox2.Clear();
        }
    }
}

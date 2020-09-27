using System;
using System.Collections.Generic;
using System.Linq;
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
using AxKHOpenAPILib;
using KHAxLib;


namespace 리듬_주식_트레이딩_봇
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            AxKHOpenAPI api = new KHAxControl().api;
            api.CommConnect();
            api.OnEventConnect += (_sender, _e) =>
            {
                if (_e.nErrCode == 0)
                {
                    Console.WriteLine("로그인 성공!");
                    var name = api.GetLoginInfo("USER_NAME");
                    nameTextBlock.Text = $"사용자 이름: {name}";
                }

            };

        }
         
       
    }
}

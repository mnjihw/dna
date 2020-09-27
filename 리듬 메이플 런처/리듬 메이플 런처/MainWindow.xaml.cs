using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
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

namespace 리듬_메이플_런처
{
    public partial class MainWindow : Window
    {   
        public MainWindow() 
        {
            InitializeComponent();

            var path = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Registry32).OpenSubKey(@"SOFTWARE\Wizet\MapleStory")?.GetValue("ExecPath") as string;

            if (path == null)
            {
                MessageBox.Show($"메이플이 설치돼 있지 않습니다.{Environment.NewLine}메이플을 설치한 후 다시 시도해주세요.");
                App.Current.Shutdown();
                return;
            }

            try
            {
                File.Delete($@"{path}\BlackCipher\BlackCall.log");
                File.Delete($@"{path}\BlackCipher\BlackCipher.log");
                File.Delete($@"{path}\BlackCipher\BlackXchg.log");
                File.Delete($@"{path}\BlackCipher\NGClient.log");
            }
            catch (Exception) { }

            if(Process.GetProcessesByName("MapleStory").Length == 0)
                Process.Start($@"{path}\MapleStory.exe", "GameLaunching");

            App.Current.Shutdown();
        }
    }
}

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
using PdfSharp.Pdf;
using PdfSharp.Drawing;
using System.IO;

namespace img2pdf
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {

        private PdfDocument document = new PdfDocument();

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Drop(object sender, DragEventArgs e)
        {
            if(e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                var files = e.Data.GetData(DataFormats.FileDrop) as string[];

                foreach(var file in files)
                {
                    var page = document.AddPage();
                    var width = 756;
                    var height = 1000;
                    page.Width = width;
                    page.Height = height;

                    XGraphics xgr = XGraphics.FromPdfPage(page);
                    XImage image = XImage.FromFile(file);

                    xgr.RotateTransform(90);
                    
                    xgr.TranslateTransform(0, -width); 
                    
                    xgr.DrawImage(image, 0, 0, height, width);
                    

                    textBlock.Text += $"{Environment.NewLine}{file}";
                }
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            document.Save($@"{ Environment.GetFolderPath(Environment.SpecialFolder.Desktop)}\gd.pdf");
            document.Close();

        }
    }
}

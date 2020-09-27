using System.IO;
using System.Windows;
using System.Security.Cryptography;
using System;
using System.Linq;
using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.WindowsAPICodePack.Dialogs;
using System.Windows.Controls;
using System.Diagnostics;

namespace image_hash_generator
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {

        private readonly string path = @"C:\Users\mnjihw\Documents\Visual Studio 2019\Projects\리듬 DC 콘솔 봇\리듬 DC 콘솔 봇\bin\Debug\이미지 블랙리스트.txt";
        private readonly List<(string, int)> imageBlacklist = new List<(string, int)>();

        public MainWindow()
        {
            InitializeComponent();

            var lines = File.ReadAllLines(path);
           
            foreach (var line in lines)
            {
                var result = line.Split(' ');

                imageBlacklist.Add((result[0], int.Parse(result[1])));
            }

            //button1.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
        }




        private void Window_Drop(object sender, DragEventArgs e)
        {
            string[] dropped = e.Data.GetData(DataFormats.FileDrop) as string[];

            foreach (var drop in dropped)
            {
                var fs = File.OpenRead(drop);
                int start = drop.LastIndexOf('.') + 1;
                string extension = drop.Substring(start, drop.Length - start);
                var imageExtensions = new[] { "jpg", "jpeg", "png", "bmp", "gif", "dib" };
                if (!imageExtensions.Any(str => str.Equals(extension, StringComparison.OrdinalIgnoreCase)))
                {
                    (sender as Window).Activate();
                    MessageBox.Show("이미지 파일 아닌 게 섞여있음");
                    return;
                }
                var tuple = (ComputeSHA256Hash(fs), (int)new FileInfo(drop).Length);
                Console.WriteLine(tuple);
                int index = imageBlacklist.BinarySearch(tuple, new MyComparer());
                if (index >= 0)
                {
                    Console.WriteLine("이미 있음");
                }
                else
                {
                    
                    imageBlacklist.Insert(~index, tuple);
                }
                fs.Close();
                File.Delete(drop);

            }
            File.WriteAllLines(path, imageBlacklist.Select(item => $"{item.Item1} {item.Item2}").ToArray());
            label1.Content = $"{dropped.Length}개 추가 완료";
        }


        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Stopwatch stopwatch = new Stopwatch();
            int count = 0;
            var dialog = new CommonOpenFileDialog
            {
                IsFolderPicker = true
            };

            if (dialog.ShowDialog() == CommonFileDialogResult.Ok)
            {
                Task.Run(() =>
                {
                    var imagesInfo = new List<(string imageName, string hash, int size)>();
                    var imageNames = Directory.GetFiles(dialog.FileName);
                    

                    for (int i = 0; i < imageNames.Length; ++i)
                    {
                        string imageName = imageNames[i];
                        string hash;
                        int index, size;
                        dynamic tuple;

                        stopwatch.Restart();
                        int start = imageName.LastIndexOf('.') + 1;
                        string extension = imageName.Substring(start, imageName.Length - start);
                        var imageExtensions = new[] { "jpg", "jpeg", "png", "bmp", "gif", "dib" };
                        if (!imageExtensions.Any(str => str.Equals(extension, StringComparison.OrdinalIgnoreCase)))
                            continue;

                        var fs = File.OpenRead(imageName);
                        hash = ComputeSHA256Hash(fs);
                        size = (int)fs.Length;
                        tuple = (imageName, hash, size);
                        index = imagesInfo.BinarySearch(tuple, new MyComparer2());
                        fs.Close();
                        
                        if (index >= 0)
                        {
                            File.Delete(imageName);
                            ++count;
                        }
                        else
                            imagesInfo.Insert(~index, tuple);

                        tuple = (hash, size);
                        index = imageBlacklist.BinarySearch(tuple, new MyComparer());

                        if (index >= 0)
                        {
                            tuple = (string.Empty, hash, size);
                            index = imagesInfo.BinarySearch(tuple, new MyComparer2());
                            if (index < 0)
                                MessageBox.Show("말안됨");

                            File.Delete(imagesInfo[index].imageName);
                            ++count;
                            imagesInfo.RemoveAt(index);

                        }
                        stopwatch.Stop();
                        Console.WriteLine($"{(int)((double)i / imageNames.Length * 100)}%({i})개 진행중 {count}개 삭제함 {stopwatch.ElapsedMilliseconds}ms");
                    }
                    MessageBox.Show(count + "개 삭제함");
                });
            }

        }

        private string ComputeSHA256Hash(FileStream fileStream)
        {
            using (SHA256 sha256 = SHA256.Create())
            {
                return Convert.ToBase64String(sha256.ComputeHash(fileStream));
            }
        }



        private class MyComparer : IComparer<(string Hash, int Size)>
        {
            public int Compare((string Hash, int Size) t1, (string Hash, int Size) t2)
            {
                if (t1.Size == t2.Size)
                    return t1.Hash.CompareTo(t2.Hash);

                return t1.Size.CompareTo(t2.Size);
            }
        }
        private class MyComparer2 : IComparer<(string, string Hash, int Size)>
        {
            public int Compare((string, string Hash, int Size) t1, (string, string Hash, int Size) t2)
            {
                if (t1.Size == t2.Size)
                    return t1.Hash.CompareTo(t2.Hash);

                return t1.Size.CompareTo(t2.Size);
            }
        }
    }
}

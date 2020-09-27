using MapleLib.WzLib.WzProperties;
using Microsoft.Win32;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using System.Xml;
using Microsoft.WindowsAPICodePack.Dialogs;
using Wz;
using System.Runtime.InteropServices;

namespace 리듬_메이플_BGM_플레이어
{
    public partial class MainWindow : Window
    {
        public static WzInformationManager InfoManager = new WzInformationManager();
        private WzMp3Streamer Player { get; }
        private DispatcherTimer SliderTimer { get; } = new DispatcherTimer();
        private PlayListClass PlayList { get; } = new PlayListClass();
        private bool PlaysRandomly { get; set; } = true;
        private readonly Random random = new Random();
        public bool RepeatsAll { get; set; } = true;
        private float LastVolume { get; set; }
        private bool IsLeftButtonPressed { get; set; }
        private Cursor MouseDownCursor { get; }
        private Cursor MouseUpCursor { get; }
        private Cursor MouseHoverDownCursor { get; }
        private Cursor MouseHoverUpCursor { get; }
        private bool IsCursorOverCommonControl { get; set; }
        

        private bool IsDragging { get; set; }
       
        public MainWindow()
        {
            RenderOptions.ProcessRenderMode = System.Windows.Interop.RenderMode.SoftwareOnly;
            InitializeComponent();
            Player = WzMp3Streamer.Instance;

            MouseDownCursor = CursorHelper.CreateCursor(new Bitmap(Application.GetResourceStream(new Uri("pack://application:,,,/Images/mousedown.png")).Stream), 1, 1);
            MouseUpCursor = CursorHelper.CreateCursor(new Bitmap(Application.GetResourceStream(new Uri("pack://application:,,,/Images/mouseup.png")).Stream), 1, 1);
            MouseHoverDownCursor = CursorHelper.CreateCursor(new Bitmap(Application.GetResourceStream(new Uri("pack://application:,,,/Images/mousehoverdown.png")).Stream), 1, 1);
            MouseHoverUpCursor = CursorHelper.CreateCursor(new Bitmap(Application.GetResourceStream(new Uri("pack://application:,,,/Images/mousehoverup.png")).Stream), 1, 1);


        }
        [DllImport("user32.dll", SetLastError = true)]
        static extern int MessageBoxTimeout(IntPtr hwnd, string text, string title, uint type, short languageId, int milliseconds);
        public void PlayFromBGMListItem(BGMListItem item)
        {
            PlayMusic(item);

            listView1.ScrollIntoView(item);
            listView1.SelectedItem = item;
        }

        private void SliderTimer_Tick(object sender, EventArgs e)
        {
            if (Player.PlaybackState == NAudio.Wave.PlaybackState.Playing)
            {
                if (!IsLeftButtonPressed)
                    playbackSlider.Value = Player.Position;

                TimeSpan playedTime = TimeSpan.FromSeconds(Player.Position);
                currentTimeTextBlock.Text = $"{playedTime.Minutes:D2}:{playedTime.Seconds:D2}";
            }
        }

        private void Thumb_MouseEnter(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed && e.MouseDevice.Captured == null)
            {
                ((VisualTreeHelper.GetParent(sender as DependencyObject) as Track).TemplatedParent as Slider).Cursor = MouseDownCursor;   
                MouseButtonEventArgs args = new MouseButtonEventArgs(e.MouseDevice, e.Timestamp, MouseButton.Left) { RoutedEvent = MouseLeftButtonDownEvent };
                (sender as Thumb).RaiseEvent(args);
            }
        }

        private async Task LoadAndDisplayBGMs(string path)
        {
            var wzLoadingTask = Task.Run(() =>
            {
                if (path == null)
                {
                    MessageBox.Show($"메이플이 설치돼 있지 않습니다.{Environment.NewLine}본 플레이어는 메이플의 Sound.wz 파일을 읽는 방식이어서 메이플이 설치돼 있어야 사용할 수 있습니다.{Environment.NewLine}해당 파일이 있으시면 선택해주세요.");
                    return;
                }
                WzFileManager wzManager = new WzFileManager(path, MapleLib.WzLib.WzMapleVersion.BMS);

                wzManager.LoadWzFile("sound");
                wzManager.ExtractSoundFile();
            });

            var pathes = new List<string>();
            var locations = new List<string>();
            var comments = new List<string>();

            using (XmlReader xmlReader = XmlReader.Create(Application.GetResourceStream(new Uri("pack://application:,,,/BGMMetadata.xml")).Stream))
            {
                while (xmlReader.Read())
                {
                    if (xmlReader.Name == "BGM")
                    {
                        pathes.Add(xmlReader["path"]);
                        locations.Add(xmlReader["loc."]);
                        comments.Add(xmlReader["cmt"]);
                    }
                }
            }
            await wzLoadingTask;

            var sortedBGMs = new List<string>();

            foreach (KeyValuePair<string, WzSoundProperty> bgm in InfoManager.BGMs)
                sortedBGMs.Add(bgm.Key);

            sortedBGMs.Sort();

            //sortedBGMs = sortedBGMs.OrderByDescending(s => s.Length).ToList();

            string title;

            listView1.Items.Clear();

            for (int i = 0, metadataIndex = 0; i < sortedBGMs.Count; ++i, ++metadataIndex)
            {
                title = sortedBGMs[i].Split('/')[1];

                if (sortedBGMs[i] == "Bgm48/Title")
                    title = "Title_Old";
                else if (sortedBGMs[i] == "Bgm26/FinalFight")
                    title += "2";
     
                ListViewItem item;
                if (pathes[metadataIndex] == sortedBGMs[i])
                {
                    item = new ListViewItem { Content = new BGMListItem { Number = i + 1, Title = title, Path = sortedBGMs[i], Location = locations[metadataIndex], Comment = comments[metadataIndex] } };
                    ToolTipService.SetToolTip(item, $"제목: {title}        설명: {comments[metadataIndex]}       위치: {locations[metadataIndex]}");
                }
                else
                {               
                    --metadataIndex;
                    item = new ListViewItem { Content = new BGMListItem { Number = i + 1, Title = title, Path = sortedBGMs[i], Location = "", Comment = "" } };
                    ToolTipService.SetToolTip(item, $"제목: {title}        설명: {""}       위치: {""}");
                }
                ToolTipService.SetVerticalOffset(item, 80);
                ToolTipService.SetPlacement(item, PlacementMode.Top);
                ToolTipService.SetPlacementTarget(item, this);
                listView1.Items.Add(item);
            }
        }

        private void SetFrameworkElementCursor(FrameworkElement element, Cursor cursor, MouseButtonEventHandler handler)
        {
            element.ForceCursor = true;
            element.Cursor = cursor;
            element.AddHandler(MouseLeftButtonDownEvent, handler, true);
            element.AddHandler(MouseLeftButtonUpEvent, handler, true);
        }

        private void Stopped(object sender, NAudio.Wave.StoppedEventArgs e)
        {
            int index;
            if (PlayList.Items.Count == 0 || Math.Floor(Player.Position) != Player.Length)
                return;
           if(!RepeatsAll)
            {
                Player.ResetPosition();
                Player.Play();
            }
            else
            {
                if(PlaysRandomly)
                {
                    do
                    {
                        index = random.Next(PlayList.Items.Count);
                    } while (index == PlayList.CurrentIndex);
                    PlayList.CurrentIndex = index;
                }
                else
                    PlayList.CurrentIndex = index = (PlayList.CurrentIndex + 1) % PlayList.Items.Count;
                playListListView.SelectedIndex = index;
                PlayMusic(PlayList[index]);
            }

            
        }

        private async void Window_LoadedAsync(object sender, RoutedEventArgs e)
        {

#region Set Cursor
            Cursor = MouseUpCursor;
            MouseLeftButtonDown += MainWindow_MouseLeftButtonDown;
            MouseLeftButtonUp += MainWindow_MouseLeftButtonUp;
            
            SetFrameworkElementCursor(menuToggleButton, MouseHoverUpCursor, SetCursorToMouseHover);
            SetFrameworkElementCursor(menuPopupBox, MouseHoverUpCursor, SetCursorToMouseHover);
            SetFrameworkElementCursor(minimizeButton, MouseHoverUpCursor, SetCursorToMouseHover);
            SetFrameworkElementCursor(closePopupBox, MouseHoverUpCursor, SetCursorToMouseHover);
            SetFrameworkElementCursor(previousButton, MouseHoverUpCursor, SetCursorToMouseHover);
            SetFrameworkElementCursor(playPauseButton, MouseHoverUpCursor, SetCursorToMouseHover);
            SetFrameworkElementCursor(nextButton, MouseHoverUpCursor, SetCursorToMouseHover);
            SetFrameworkElementCursor(addToPlayListButton, MouseHoverUpCursor, SetCursorToMouseHover);
            SetFrameworkElementCursor(downloadButton, MouseHoverUpCursor, SetCursorToMouseHover);
            SetFrameworkElementCursor(volumeMuteButton, MouseHoverUpCursor, SetCursorToMouseHover);
            SetFrameworkElementCursor(playbackLoopOptionImage, MouseHoverUpCursor, SetCursorToMouseHover);
            SetFrameworkElementCursor(randomOptionImage, MouseHoverUpCursor, SetCursorToMouseHover);
            SetFrameworkElementCursor(removeFromPlayListButton, MouseHoverUpCursor, SetCursorToMouseHover);
            SetFrameworkElementCursor(listView1, MouseUpCursor, SetCursorToOrdinaryMouse);
            SetFrameworkElementCursor(playListListView, MouseUpCursor, SetCursorToOrdinaryMouse);

            SetFrameworkElementCursor(playbackSlider, MouseUpCursor, SetCursorToOrdinaryMouse);
            SetFrameworkElementCursor(volumeSlider, MouseUpCursor, SetCursorToOrdinaryMouse);


            searchTextBox.Cursor = Cursors.IBeam;
            

#endregion

            playListListView.ItemsSource = PlayList.Items;

            //slider.ApplyTemplate(); 필요한지 의문
            (playbackSlider.Template.FindName("PART_Track", playbackSlider) as Track).Thumb.MouseEnter += Thumb_MouseEnter;
            (volumeSlider.Template.FindName("PART_Track", volumeSlider) as Track).Thumb.MouseEnter += Thumb_MouseEnter;


            playbackSlider.AddHandler(PreviewMouseLeftButtonDownEvent, new MouseButtonEventHandler(Slider_PreviewMouseLeftButtonDown), true);
            playbackSlider.AddHandler(PreviewMouseLeftButtonUpEvent, new MouseButtonEventHandler(Slider_PreviewMouseLeftButtonUp), true);


            SliderTimer.Interval = TimeSpan.FromMilliseconds(100);
            SliderTimer.Tick += SliderTimer_Tick;
            SliderTimer.Start();


            var path = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Registry32).OpenSubKey(@"SOFTWARE\Wizet\MapleStory")?.GetValue("ExecPath") as string;

            if (path[path.Length - 1] == '\\')
                path.Remove(path.Length - 1);
            await LoadAndDisplayBGMs(path);
        }


        private void AddToPlayList(BGMListItem item)
        {
            PlayList.Items.Insert(0, item.Clone() as BGMListItem);

            for (int i = 0; i < PlayList.Items.Count; ++i)
                PlayList[i].Number = i + 1;

            playListListView.ItemsSource = null;
            playListListView.ItemsSource = PlayList.Items;
        }

        private void AddToPlayList(IList items)
        {
            for (int i = 0; i < items.Count; ++i)
                PlayList.Items.Insert(i, ((items[i] as ListViewItem).Content as BGMListItem).Clone() as BGMListItem);

            for (int i = 0; i < PlayList.Items.Count; ++i)
                PlayList[i].Number = i + 1;

            playListListView.ItemsSource = null;
            playListListView.ItemsSource = PlayList.Items;
        }

        private void ListView1_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed && ((sender as ListView).SelectedItem as ListViewItem)?.Content is BGMListItem item)
            {
                AddToPlayList(item);
                PlayMusic(item);
            }
        }


        private void PlayMusic(BGMListItem item)
        {
            Player.Open(InfoManager.BGMs[item.Path]);
            Player.SetPlaybackStopped(Stopped);
            Player.Play();

            playPauseIcon.Kind = MaterialDesignThemes.Wpf.PackIconKind.Pause;
            playbackSlider.Value = 0;
            playbackSlider.Maximum = Player.Length;
            TimeSpan length = TimeSpan.FromSeconds(Player.Length);
            titleTextBlock.Text = $"{item.Title}({item.Comment})";
            totalLengthTextBlock.Text = length.ToString(@"mm\:ss");
        }


        private void PlayPauseButton_Click(object sender, RoutedEventArgs e)
        {
            //playmusic으로 통합했는데 이부분 고치기
            switch (Player.PlaybackState)
            {
                case NAudio.Wave.PlaybackState.Stopped:
                    MessageBox.Show("^^^^");
                    break;
                case NAudio.Wave.PlaybackState.Playing:
                    playPauseIcon.Kind = MaterialDesignThemes.Wpf.PackIconKind.Play;
                    Player.Pause();
                    break;
                case NAudio.Wave.PlaybackState.Paused:
                    playPauseIcon.Kind = MaterialDesignThemes.Wpf.PackIconKind.Pause;
                    Player.Resume();
                    break;
                case null:
                    if (PlayList.Items.Count == 0)
                        MessageBox.Show("no bgms in the playlist");
                    int index = 0;
                    if(PlaysRandomly)
                        index = random.Next(PlayList.Items.Count);
                    playListListView.SelectedIndex = index;
                    PlayMusic(PlayList[index]);
                    break;
            }
        }

        private void PreviousOrNextButton_Click(object sender, RoutedEventArgs e)
        {
            int index;

            if ((sender as Button).Name == "previousButton")
            {
                if (PlayList.CurrentIndex > 0)
                    index = --PlayList.CurrentIndex;
                else
                    index = PlayList.CurrentIndex = PlayList.Items.Count - 1;
            }
            else
            {
                if (PlayList.CurrentIndex < PlayList.Items.Count - 1)
                    index = ++PlayList.CurrentIndex;
                else
                    index = PlayList.CurrentIndex = 0;
            }
            playListListView.SelectedIndex = index;
            PlayMusic(PlayList[index]);
        }

        private void SearchTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            bool isFirstTime = true;
            ListViewItem firstItem = null;

            if (searchTextBox.Text.Length != 0 && searchTextBox.Text.All(char.IsDigit))
            {
                int index = int.Parse(searchTextBox.Text) - 1;
                if (index >= listView1.Items.Count)
                    return;

                firstItem = listView1.Items[index] as ListViewItem;
            }
            else
            {

                listView1.Items.Filter = listViewItem =>
                {
                    var item = (listViewItem as ListViewItem).Content as BGMListItem;
                    var result = new[] { item.Title, item.Comment, item.Location }.Any(value => value.IndexOf(searchTextBox.Text, StringComparison.OrdinalIgnoreCase) != -1);

                    if (result && isFirstTime)
                    {
                        isFirstTime = false;
                        firstItem = listViewItem as ListViewItem;
                    }
                    return result;
                };
            }

            listView1.ScrollIntoView(firstItem);
            listView1.SelectedItem = firstItem;
        }


        private void ListView1_KeyDown(object sender, KeyEventArgs e)
        {
            if (Keyboard.Modifiers == ModifierKeys.Control && e.Key == Key.C && ((sender as ListView).SelectedItem as ListViewItem).Content is BGMListItem item)
                Clipboard.SetText(item.Title);
        }


        private void SetCursorToMouseHover(object sender, MouseButtonEventArgs e)
        {
            if(e.ButtonState == MouseButtonState.Pressed)
                (sender as FrameworkElement).Cursor = MouseHoverDownCursor;
            else
                (sender as FrameworkElement).Cursor = MouseHoverUpCursor;
        }

        private void SetCursorToOrdinaryMouse(object sender, MouseButtonEventArgs e)
        {
            if (e.ButtonState == MouseButtonState.Pressed)
                (sender as FrameworkElement).Cursor = MouseDownCursor;
            else
                (sender as FrameworkElement).Cursor = MouseUpCursor;

        }

        private void PlaybackLoopOptionImage_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            RepeatsAll = !RepeatsAll;

            if(RepeatsAll)
            {
                playbackLoopOptionImage.Source = new ImageSourceConverter().ConvertFromString("pack://application:,,,/Images/repeatall.png") as ImageSource;

                playbackLoopOptionImage.Width = 25;
                playbackLoopOptionImage.Height = 25;
            }
            else
            {
                playbackLoopOptionImage.Source = new ImageSourceConverter().ConvertFromString("pack://application:,,,/Images/repeatone.png") as ImageSource;
                playbackLoopOptionImage.Width = 25;
                playbackLoopOptionImage.Height = 25;
            }
        }

        private void MainWindow_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Cursor = MouseDownCursor;

            if (Mouse.DirectlyOver?.GetType() == typeof(Grid) || Mouse.DirectlyOver == titleBarTextBlock)
            {
                DragMove();
                RaiseEvent(new MouseButtonEventArgs(e.MouseDevice, e.Timestamp, MouseButton.Left) { RoutedEvent = MouseLeftButtonUpEvent });
            }
            
        }

        

        private void RandomOptionImage_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            PlaysRandomly = !PlaysRandomly;

            if (PlaysRandomly)
                randomOptionImage.Source = new ImageSourceConverter().ConvertFromString("pack://application:,,,/Images/shuffle.png") as ImageSource;
            else
                randomOptionImage.Source = new ImageSourceConverter().ConvertFromString("pack://application:,,,/Images/noshuffle.png") as ImageSource;
        }


        private async void WzOpenButton_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog
            {
                DefaultExt = ".wz",
                Filter = "wz파일 (*.wz)|*.wz"
            };
            bool? result = dlg.ShowDialog();

            if (result.HasValue && result.Value)
            {
                if (dlg.FileName.IndexOf("Sound.wz", StringComparison.OrdinalIgnoreCase) == -1)
                {
                    MessageBox.Show("Sound.wz만 선택 가능합니다.");
                    return;
                }

                await LoadAndDisplayBGMs(dlg.FileName.Substring(0, dlg.FileName.LastIndexOf('\\')));
            }
        }

        private void TopMostToggleButton_Click(object sender, RoutedEventArgs e) => Topmost = !Topmost;

        private void TopMostButton_Click(object sender, RoutedEventArgs e)
        {
            topMostToggleButton.IsChecked = !topMostToggleButton.IsChecked;
            topMostToggleButton.RaiseEvent(new RoutedEventArgs(ButtonBase.ClickEvent));
        }

        private void MinimizeButton_Click(object sender, RoutedEventArgs e) => WindowState = WindowState.Minimized;


        private void CloseButton_Click(object sender, RoutedEventArgs e) => Close();


        private void SearchTextBox_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                if ((listView1.SelectedItem as ListViewItem).Content is BGMListItem item)
                {
                    AddToPlayList(item);
                    PlayMusic(item);
                }
            }
        }


        private void Slider_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            IsLeftButtonPressed = true;
        }
        private void Slider_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (Player != null)
            {
                Player.Position = playbackSlider.Value;
                TimeSpan playedTime = TimeSpan.FromSeconds(Player.Position);
                currentTimeTextBlock.Text = $"{playedTime.Minutes:D2}:{playedTime.Seconds:D2}";
            }
            IsLeftButtonPressed = false;
        }


        private void RemoveFromPlayListButton_Click(object sender, RoutedEventArgs e)
        {
            var items = playListListView.SelectedItems;

            if (items != null)
            {
                for (int i = items.Count - 1; i >= 0; --i)
                    PlayList.Items.Remove(items[i] as BGMListItem);

                for (int i = 0; i < PlayList.Items.Count; ++i)
                    PlayList[i].Number = i + 1;

                playListListView.ItemsSource = null;
                playListListView.ItemsSource = PlayList.Items;
            }
        }

        private void PlayListListView_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if ((sender as ListView).SelectedItem is BGMListItem item)
            {
                PlayList.CurrentIndex = (sender as ListView).SelectedIndex;
                PlayMusic(item);
            }
        }

        private void VolumeMuteButton_Click(object sender, RoutedEventArgs e)
        {
            if (volumeMuteIcon.Kind == MaterialDesignThemes.Wpf.PackIconKind.VolumeMute)
            {
                volumeSlider.Value = LastVolume;
                volumeMuteIcon.Kind = MaterialDesignThemes.Wpf.PackIconKind.VolumeHigh;
            }
            else
            {
                LastVolume = Player.Volume;// ?? (float)volumeSlider.Value;
                volumeMuteIcon.Kind = MaterialDesignThemes.Wpf.PackIconKind.VolumeMute;
                volumeSlider.Value = 0;
            }
        }

        private void VolumeSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (e.NewValue == 0)
                volumeMuteIcon.Kind = MaterialDesignThemes.Wpf.PackIconKind.VolumeMute;
            else if (e.OldValue == 0)
                volumeMuteIcon.Kind = MaterialDesignThemes.Wpf.PackIconKind.VolumeHigh;

            if (Player != null)
                Player.Volume = (float)e.NewValue;
        }

        private async void DownloadButton_ClickAsync(object sender, RoutedEventArgs e)
        {
            if (listView1.SelectedItem != null)
            {
                var dialog = new CommonOpenFileDialog() { IsFolderPicker = true, EnsurePathExists = true, InitialDirectory = Directory.GetCurrentDirectory() };

                if (dialog.ShowDialog() == CommonFileDialogResult.Ok)
                {
                    var items = new List<BGMListItem>();

                    foreach (ListViewItem item in listView1.SelectedItems)
                        items.Add(item.Content as BGMListItem);
                    await Task.Run(() =>
                    {
                        Parallel.ForEach(items, item => InfoManager.BGMs[item.Path].SaveToFile($@"{dialog.FileName}\{item.Title}.mp3"));
                        MessageBoxTimeout(IntPtr.Zero, "다운로드 완료!", "알림", 0, 0, 1000);
                    });
                }
            }
            else
                MessageBox.Show("저장할 BGM을 선택해주세요.");


        }

        private void PlayListListView_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            if ((sender as ListView).SelectedItem is BGMListItem selectedItem)
            {
                foreach (ListViewItem item in listView1.Items)
                {
                    if ((item.Content as BGMListItem).Path == selectedItem.Path)
                    {
                        listView1.ScrollIntoView(item);
                        listView1.SelectedItem = item;
                        break;
                    }
                }
            }
        }

        private void AddToPlayListButton_Click(object sender, RoutedEventArgs e)
        {
            var items = listView1.SelectedItems;

            if (items != null)
                AddToPlayList(items);
        }

        private void AddAllToPlayListButton_Click(object sender, RoutedEventArgs e)
        {

        }

    
        private void MainWindow_MouseLeftButtonUp(object sender, MouseEventArgs e) => Cursor = MouseUpCursor;


        private void PlayListListView_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Delete)
                removeFromPlayListButton.RaiseEvent(new RoutedEventArgs(ButtonBase.ClickEvent));
        }

        private void MainWindow_Closed(object sender, EventArgs e)
        {
            Player.Dispose();
        }

        private T FindAncestor<T>(DependencyObject obj) where T : DependencyObject
        {
            do
            {
                if (obj is T)
                {
                    return (T)obj;
                }
                obj = VisualTreeHelper.GetParent(obj);
            }
            while (obj != null);
            return null;
        }


        private void PlayListListView_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            var ancestor = FindAncestor<ListViewItem>(playListListView.InputHitTest(e.GetPosition(playListListView)) as DependencyObject);

            if (ancestor != null)
                IsDragging = true;
        }
      


        private void PlayListListView_PreviewMouseMove(object sender, MouseEventArgs e)
        {
            if((e.LeftButton == MouseButtonState.Pressed) && IsDragging)
            {
                ListViewItem listViewItem = FindAncestor<ListViewItem>(e.OriginalSource as DependencyObject);
                if (listViewItem == null)
                    return;



                if (DragDrop.DoDragDrop(listViewItem, new DataObject("BGMListItem", playListListView.Items.IndexOf(listViewItem.Content)), DragDropEffects.Move | DragDropEffects.Copy) == DragDropEffects.None)
                    Mouse.OverrideCursor = MouseUpCursor;

            }
        }

        private void PlayListListView_Drop(object sender, DragEventArgs e)
        {
            if(e.Data.GetDataPresent("BGMListItem"))
            {
                var ancestor = FindAncestor<ListViewItem>((sender as ListView).InputHitTest(e.GetPosition(sender as ListView)) as DependencyObject);
                int index;

                if (ancestor == null)
                {
                    var firstItemY = ((sender as ListView).ItemContainerGenerator.ContainerFromIndex(0) as ListViewItem)?.TranslatePoint(new System.Windows.Point(), sender as ListView).Y;
                       
                    if (firstItemY.HasValue && e.GetPosition(sender as ListView).Y <  firstItemY)
                        index = 0;
                    else
                        index = (sender as ListView).Items.Count - 1;
                }
                else
                    index = (sender as ListView).Items.IndexOf(ancestor.Content);

                PlayList.Items.Move((int)e.Data.GetData("BGMListItem", false), index);

                for (int i = 0; i < PlayList.Items.Count; ++i)
                    PlayList[i].Number = i + 1;

                (sender as ListView).ItemsSource = null;
                (sender as ListView).ItemsSource = PlayList.Items;
                
                IsDragging = false;
                Mouse.OverrideCursor = MouseUpCursor;
            }
        }

        public DependencyObject FindChild(DependencyObject o, Type childType)
        {
            DependencyObject foundChild = null;
            if (o != null)
            {
                int childrenCount = VisualTreeHelper.GetChildrenCount(o);
                for (int i = 0; i < childrenCount; i++)
                {
                    var child = VisualTreeHelper.GetChild(o, i);
                    if (child.GetType() != childType)
                    {
                        foundChild = FindChild(child, childType);
                    }
                    else
                    {
                        foundChild = child;
                        break;
                    }
                }
            }
            return foundChild;
        }

        private void PlayListListView_DragOver(object sender, DragEventArgs e)
        {
            ListView listView = sender as ListView;
            ScrollViewer scrollviewer = (ScrollViewer)FindChild(listView, typeof(ScrollViewer));
            System.Windows.Point mouseposition = e.GetPosition(listView);

            if (mouseposition.Y < 10)
                scrollviewer.ScrollToVerticalOffset(scrollviewer.VerticalOffset - 1);
            if (mouseposition.Y > listView.RenderSize.Height - 10)
                scrollviewer.ScrollToVerticalOffset(scrollviewer.VerticalOffset + 1);
        }
    }
}
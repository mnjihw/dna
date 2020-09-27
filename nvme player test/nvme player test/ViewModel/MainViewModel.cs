using nvme_player_test.Model;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;
using Wz;
using MapleLib.WzLib.WzProperties;
using System.Windows.Input;
using nvme_player_test.Command;

namespace nvme_player_test.ViewModel
{
    public partial class MainViewModel : INotifyPropertyChanged
    {
        public ObservableCollection<BGMListItem> Items { get; } = new ObservableCollection<BGMListItem>();
        public static WzInformationManager InfoManager = new WzInformationManager();

        public MainViewModel()
        {            
            WzFileManager wzManager = new WzFileManager(@"C:\Nexon\Maple", MapleLib.WzLib.WzMapleVersion.BMS);
            wzManager.LoadWzFile("sound");
            wzManager.ExtractSoundFile(InfoManager);

            var sortedBGMs = new List<string>();

            foreach (KeyValuePair<string, WzSoundProperty> bgm in InfoManager.BGMs)
                sortedBGMs.Add(bgm.Key);

            sortedBGMs.Sort();

            for (int i = 0; i < sortedBGMs.Count; ++i)
            {
                var title = sortedBGMs[i].Split('/')[1];

                var item =  new BGMListItem { Number = i + 1, Title = title, Path = sortedBGMs[i], Location = "", Comment = "" };

                Items.Add(item);
            }

        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));

        private ICommand command;
        public ICommand Command
        {
            get => command ??= new RelayCommand<object>(x => Console.WriteLine("GD"));
        }


    }
}

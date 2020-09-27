using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace wpf_nvme_test.ViewModel
{
    public partial class MainViewModel : INotifyPropertyChanged
    {
        private int number;
        public int Number
        {
            get => number;
            set
            {
                number = value;
                OnPropertyChanged(nameof(Number));
                OnPropertyChanged(nameof(PlusEnable));
                OnPropertyChanged(nameof(MinusEnable));

                PageContent = $"{number} 페이지를 보고 있어요.";
            }
        }

        public MainViewModel()
        {
            Number = 1;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));

        private ICommand minusCommand;
        public ICommand MinusCommand => minusCommand ??= new DelegateCommand(Minus);
        
        private void Minus() => --Number;

        public bool MinusEnable => Number > 1 ? true : false;


        private ICommand plusCommand;
        public ICommand PlusCommand => plusCommand ??= new DelegateCommand(Plus);

        private void Plus() => ++Number;

        public bool PlusEnable => Number < 10 ? true : false;

        private string pageContent;
        public string PageContent
        {
            get => pageContent;
            set
            {
                pageContent = value;
                OnPropertyChanged(nameof(PageContent));
            }

        }


    }
}

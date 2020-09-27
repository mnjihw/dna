using System;
using System.Windows.Input;

namespace wpf_nvme_test.ViewModel
{
    public partial class MainViewModel
    {
        public class DelegateCommand : ICommand
        {
            private readonly Func<bool> canExecute;
            private readonly Action execute;

            public DelegateCommand(Action execute) : this(execute, null)
            {

            }

            public DelegateCommand(Action execute, Func<bool> canExecute)
            {
                this.execute = execute;
                this.canExecute = canExecute;
            }

            public event EventHandler CanExecuteChanged;

            public bool CanExecute(object o)
            {
                if (canExecute == null)
                    return true;
                return canExecute();
            }

            public void Execute(object o) => execute();
            public void RaiseCanExecuteChanged()
            {
                CanExecuteChanged?.Invoke(this, EventArgs.Empty);
            }
        }
    }
   
}

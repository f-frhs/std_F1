using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace std_Fujita
{
    /// <summary>
    /// ViewModelの基底クラス
    /// INotifyPropertyChanged と IDataErrorInfoを実装する
    /// </summary>
    public class ViewModelBase : INotifyPropertyChanged, IDataErrorInfo
    {
        /// <summary> INotifyPropertyChanged と IDataErrorInfoの実装 </summary>        
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary> INotifyPropertyChangedイベントを発生させる </summary>
        /// <param name="propertyName"></param>
        protected virtual void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        /// <summary> IDataErrorInfo.Errorの実装 </summary>
        string IDataErrorInfo.Error => (errorMessages.Count > 0) ? "Has Error" : null;

        /// <summary> IDataErrorInfo.Itemの実装 </summary>
        /// <param name="columnName"></param>
        string IDataErrorInfo.this[string columnName]
            => errorMessages.ContainsKey(columnName) ? errorMessages[columnName] : null; 

        /// <summary> IDataErrorInfo用のエラーメッセージを保持する辞書 </summary>
        private readonly Dictionary<string, string> errorMessages = new Dictionary<string, string>();

        /// <summary> エラーメッセージのセット </summary>
        /// <param name="propertyName"></param>
        /// <param name="errorMessage"></param>
        protected void SetError(string propertyName, string errorMessage)
        {
            errorMessages[propertyName] = errorMessage;
        }

        /// <summary> エラーメッセージのクリア </summary>
        /// <param name="propertyName"></param>
        protected void ClearError(string propertyName)
        {
            if (errorMessages.ContainsKey(propertyName))
                errorMessages.Remove(propertyName) ;
        }

        /// <summary> ICommand実装用のヘルパークラス </summary>
        private class DelegateCommand : ICommand
        {
            private Action<object> command; //コマンド本体
            private Func<object, bool> canExecute; //実行可否

            /// <summary> コンストラクタ </summary>
            /// <param name="command"></param>
            /// <param name="canExecute"></param>
            public DelegateCommand(Action<object> command, Func<object, bool> canExecute)
            {
                if (command == null)throw new ArgumentNullException();

                this.command = command;
                this.canExecute = canExecute;
            }

            /// <summary> ICommand.Executeの実装 </summary>
            /// <param name="parameter"></param>
            void ICommand.Execute(object parameter) => command(parameter);

            /// <summary> ICommand.CanExecuteの実装 </summary>
            /// <param name="parameter"></param>
            /// <returns></returns>
            bool ICommand.CanExecute(object parameter) => canExecute == null || canExecute(parameter);

            /// <summary> ICommand.CanExecuteChangedの実装 </summary>
            event EventHandler ICommand.CanExecuteChanged
            {
                add { CommandManager.RequerySuggested += value; }
                remove { CommandManager.RequerySuggested -= value; }
            }
        }

        /// <summary> コマンドの生成 </summary>
        /// <param name="command"></param>
        /// <param name="canExecute"></param>
        /// <returns></returns>
        protected ICommand CreateCommand(Action<object> command, Func<object, bool> canExecute)
        {
            return new DelegateCommand(command, canExecute);
        }

    }
}
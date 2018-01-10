using System;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Documents;
using System.Windows.Media.Imaging;
using System.Xml.Serialization;
using Microsoft.Win32;

namespace std_Fujita
{
    /// <summary> MainWindow.xaml の相互作用ロジック  </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            //BaseContainer実装
            CamParams.BaseContainer.DataContext = new CamViewAreaDVModel();           
        }

        private void MainWindow_OnLoaded(object sender, RoutedEventArgs e)
        {
            //諸設定
            //名前付きパイプ生成
            //サーバの接続要請
            //サーバの接続可否確認
            //サーバの接続完了信号受信
            //接続カメラの番号送信
            //接続カメラの接続信号受信
            //接続カメラの情報リクエスト送信
            //接続カメラの情報受信
        }

        private void Button_CamCapture_Click(object sender, RoutedEventArgs e)
        {
            //TODO
            throw new NotImplementedException();
        }

        private void Button_SaveCamera_Click(object sender, RoutedEventArgs e)
        {
            //TODO
            throw new NotImplementedException();
        }

        private void Button_SaveXml_Click(object sender, RoutedEventArgs e)
        {
            //TODO
            throw new NotImplementedException();
        }

        private void Button_LoadXml_Click(object sender, RoutedEventArgs e)
        {
            //TODO
            throw new NotImplementedException();
        }
    }
}

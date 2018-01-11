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
    [Serializable]
    public class TcpSettings
    {
        /// <summary> IPアドレス </summary>
        [XmlElement]
        public string IP { set; get; }

        /// <summary> ポート番号 </summary>
        [XmlElement]
        public int Port { set; get; }

        /// <summary> 受信タイムアウト </summary>
        [XmlElement]
        public int RecvTimeout { set; get; }

        /// <summary> 送信タイムアウト </summary>
        [XmlElement]
        public int SendTimeout { set; get; }

        /// <summary> ファイルへ設定情報を保存する </summary>
        public void SaveToFile(string fname)
        {
            FileSaveLoadBase<TcpSettings>.SaveToFile(fname, this);
        }

        /// <summary> ファイルから設定情報を復元する </summary>
        public TcpSettings LoadFromFile(string fname)
        {
            var temp = FileSaveLoadBase<TcpSettings>.LoadFromFile(fname);
            return temp;
        }
    }

    /// <summary>  クラスをファイルに保存、及びファイルから復元する  </summary>
    /// <typeparam name="K"></typeparam>
    [Serializable]
    public static class FileSaveLoadBase<K>
    {
        /// <summary> XMLファイル形式のファイルから、クラスを復元する </summary>
        /// <param name="fname"></param>
        /// <returns></returns>
        public static K LoadFromFile(string fname)
        {
            //書き出したファイルを読み込んでクラスを再設定
            var serializer2 = new XmlSerializer(typeof(K));
            using (var sr = new StreamReader(fname, new UTF8Encoding(false)))
            {
                var ans = (K) serializer2.Deserialize(sr);

                return ans;
            }
        }

        /// <summary> クラス情報をXMLファイルに保存する </summary>
        /// <param name="fname"></param>
        /// <param name="saveObj"></param>
        public static void SaveToFile(string fname, K saveObj)
        {
            //XMLファイルに保存する
            var serializer1 = new XmlSerializer(typeof(K));
            var sw = new StreamWriter(fname, false, new UTF8Encoding(false));
            serializer1.Serialize(sw, saveObj);
            sw.Close();
        }
    }

    /// <summary> MainWindow.xaml の相互作用ロジック  </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            //BaseContainer実装
            CamParams.BaseContainer.DataContext = new CamViewAreaDVModel();           
        }

        /// <summary>  </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void MainWindow_OnLoaded(object sender, RoutedEventArgs e)
        {
            //諸設定
            //Path設定
            var settingPath = @"\setting"; //setting
            var logPth = settingPath + @"\log\"; //log出力
            var paramHangar = settingPath + @"\paramHanger\"; //各撮影位置番号毎のパラメータ保管

            //実行ファイルのパスを取得
            var exePath = Environment.GetCommandLineArgs()[0];
            var exeFullPath = Path.GetFullPath(exePath);
            var startupPath = Path.GetDirectoryName(exeFullPath);

            //ディレクトリの確保（無ければ作成）
            if (!Directory.Exists(startupPath + settingPath))
                Directory.CreateDirectory(startupPath + settingPath); //setting
            if (!Directory.Exists(startupPath + logPth)) Directory.CreateDirectory(startupPath + logPth); //log出力
            if (!Directory.Exists(startupPath + paramHangar))
                Directory.CreateDirectory(startupPath + paramHangar); //各撮影位置番号毎のパラメータ保管

            Trace.WriteLine($"スタートアップパス={startupPath}", "Debug");

            //ログファイルの出力設定
            var fname = DateTime.Now.ToString("yyyy_MMdd_HHmmss") + "AssyLog";
            var traceListener = new TextWriterTraceListener(startupPath + logPth + fname);
            Trace.AutoFlush = true;
            Trace.Listeners.Add(traceListener);

            Trace.WriteLine($"画像解析解析・表示プログラムスタート", "Debug");
            Trace.WriteLine($"カメラ使用の可否チェック", "Debug");

            //カメラを使用して画像解析を行うかを確認
            var checkUseIdsCamera = MessageBox.Show("IDSカメラは接続されていますか？\n保存してある画像を解析する場合はNoを押してください", "IDSカメラ使用の可否",
                                        MessageBoxButton.YesNo, MessageBoxImage.Question) != MessageBoxResult.No;

            //名前付きパイプ生成
            NamedPipeClientStream namedPipe = null;
            try
            {
                namedPipe = new NamedPipeClientStream(@".", "TestPipe1", PipeDirection.InOut);
            }
            catch (ArgumentNullException ane)
            {
                Trace.WriteLine($"パイプ設定中にエラーが発生.Nullのチェックをしてください\nMessage={ane.Message}\nStack={ane.StackTrace}","Error");
            }
            catch (ArgumentException ae)
            {
                Trace.WriteLine($"パイプ設定中にエラーが発生.\nMessage={ae.Message}\nStack={ae.StackTrace}", "Error");
            }

            //タイムアウト設定
            if (namedPipe.CanTimeout)
            {
                namedPipe.WriteTimeout = 1000;
                namedPipe.ReadTimeout = 1000;
            }
            else
            {
                Trace.WriteLine($"タイムアウトが設定できません", "Debug");
            }

            //サーバの接続可否確認
            var connectToServer = await Task.Run(() =>{namedPipe.Connect(1000);return true;});

            do
            {
                if (connectToServer)
                {
                    Trace.WriteLine($"パイプ接続完了={connectToServer}", "Debug");
                }
                else
                {
                    Trace.WriteLine($"パイプ接続失敗={connectToServer},カメラプログラムの起動・設定を確認してください", "Debug");
                    MessageBox.Show($"カメラプログラム接続に失敗しました.\nカメラプログラムの起動を確認してください.", "カメラプログラムエラー", MessageBoxButton.OK,
                        MessageBoxImage.Error);
                }
            } while (!connectToServer);

            //サーバの接続完了信号受信
            var recvBuf = new byte[namedPipe.InBufferSize];
            var cnt = 0;
            try
            {
                cnt = namedPipe.Read(recvBuf, 0, recvBuf.Length);
            }
            catch (ArgumentNullException ane)
            {
                Trace.WriteLine($"パイプ接続中にエラーが発生.Nullチェックをしてください\nMessage={ane.Message}\nStack={ane.StackTrace}", "Error");
            }
            catch (ArgumentOutOfRangeException aoore)
            {
                Trace.WriteLine($"パイプ接続中にエラーが発生.レンジの確認をしてください\nMessage={aoore.Message}\nStack={aoore.StackTrace}","Error");
            }
            catch (ArgumentException ae)
            {
                Trace.WriteLine($"パイプ接続中にエラーが発生.レンジの確認をしてください\nMessage={ae.Message}\nStack={ae.StackTrace}","Error");
            }

            var rStr = Encoding.UTF8.GetString(recvBuf, 0, cnt);

            Trace.WriteLine($"パイプ接続完了文字列={rStr}", "Debug");

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

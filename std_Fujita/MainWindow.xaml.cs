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

            //設定ファイル読み込み関係
            var settingFname = startupPath + settingPath + @"\" + "tcpini.xml";
            if (!File.Exists(settingFname))
            {
                //設定ファイルが存在しないので、標準的な設定ファイルを作成して終了
                var tmpSettings = new TcpSettings();
                tmpSettings.ServerName = @".";
                tmpSettings.PipeNmae = "TestPipe1";
                tmpSettings.PipeDirection = PipeDirection.InOut;
                tmpSettings.RecvTimeout = 1000;
                tmpSettings.SendTimeout = 1000;

                tmpSettings.SaveToFile(settingFname);

                MessageBox.Show($"設定ファイルを\n{settingFname}\nに保存しました.編集してください.", "File Missing Error", MessageBoxButton.OK,
                    MessageBoxImage.Error);

                return;
            }

            //ログファイルの出力設定
            var fname = DateTime.Now.ToString("yyyy_MMdd_HHmmss") + "AssyLog";
            var traceListener = new TextWriterTraceListener(startupPath + logPth + fname);
            Trace.AutoFlush = true;
            Trace.Listeners.Add(traceListener);

            Trace.WriteLine($"画像解析解析・表示プログラムスタート", "Debug");
            Trace.WriteLine($"カメラ使用の可否チェック", "Debug");

            //設定ファイルの復元
            var settingTcp = TcpSettings.LoadFromFile(settingFname);

            //TCPの作成
            //名前付きパイプ生成
            NamedPipeClientStream namedPipe = null;
            try
            {
                namedPipe = new NamedPipeClientStream(settingTcp.ServerName, settingTcp.PipeNmae, settingTcp.PipeDirection);
            }
            catch (ArgumentNullException ane)
            {
                Trace.WriteLine($"パイプ設定中にエラーが発生.Nullのチェックをしてください\nMessage={ane.Message}\nStack={ane.StackTrace}",
                    "Error");
            }
            catch (ArgumentException ae)
            {
                Trace.WriteLine($"パイプ設定中にエラーが発生.\nMessage={ae.Message}\nStack={ae.StackTrace}", "Error");
            }

            //var namedPipe = new NamedPipeClientStream(@".", "TestPipe1", PipeDirection.InOut);

            //タイムアウト設定
            if (namedPipe.CanTimeout)
            {
                namedPipe.WriteTimeout = settingTcp.SendTimeout;
                namedPipe.ReadTimeout = settingTcp.RecvTimeout;
            }


            //カメラを使用して画像解析を行うかを確認
            var checkUseIdsCamera =
                MessageBox.Show("IDSカメラは接続されていますか？\n保存してある画像を解析する場合はNoを押してください", "IDSカメラ使用の可否", MessageBoxButton.YesNo,
                    MessageBoxImage.Question) != MessageBoxResult.No;
            if (!checkUseIdsCamera)CheckBox_CamUse.IsChecked = true;
            
            //サーバの接続可否確認
            var connectToServer = await Task.Run(() =>
            {
                namedPipe.Connect(1000);
                return true;
            });

            do
            {
                if (connectToServer)
                {
                    Trace.WriteLine($"パイプ接続完了={connectToServer}", "Debug");
                }
                else
                {
                    Trace.WriteLine($"パイプ接続失敗={connectToServer},カメラプログラムの起動・設定を確認してください", "Error");
                    MessageBox.Show($"カメラプログラム接続に失敗しました.\nカメラプログラムの起動を確認してください.", "カメラプログラムエラー", MessageBoxButton.OK,
                        MessageBoxImage.Error);
                }
            } while (!connectToServer);

            //サーバの接続完了信号受信
            var recvBuf = new byte[namedPipe.InBufferSize];
            var cnt = namedPipe.Read(recvBuf, 0, recvBuf.Length);
            var rStr = Encoding.UTF8.GetString(recvBuf, 0, cnt);

            Trace.WriteLine($"パイプ接続完了文字列={rStr}", "Debug");

            //接続カメラの番号送信
            var connectCam = "ConnectCam";
            var camId = CheckBox_CamUse.IsChecked == true ? 999 : 001; //カメラ不使用の場合はID999を送信
            SendData(namedPipe, connectCam + camId);

            //接続カメラの接続信号受信
            if (RecvData(namedPipe)[0] != "Cam1Connect")
            {
                Trace.WriteLine($"カメラ接続に失敗しました = {RecvData(namedPipe)[0]}", "Error");
                MessageBox.Show($"カメラ接続に失敗しました = {RecvData(namedPipe)[0]}", "カメラ接続エラー", MessageBoxButton.OK, MessageBoxImage.Error);
                Environment.Exit(0);
            }

            Trace.WriteLine($"カメラ接続に成功しました", "Debug");

            //接続カメラの情報リクエスト送信
            var infoCam = "GetCaminfo";
            SendData(namedPipe, infoCam+camId);

            //接続カメラの情報受信
            //TODO:現在C++側にNoCameraとFailedGetCameraInfoしかない(カメラ情報取得の関係は後回しとなったため)
            if (RecvData(namedPipe)[0] == "FailedGetCameraInfo")
            {
                Trace.WriteLine($"カメラ接続に失敗しました = {RecvData(namedPipe)[0]}", "Error");
                MessageBox.Show($"カメラ接続に失敗しました = {RecvData(namedPipe)[0]}", "カメラ接続エラー", MessageBoxButton.OK, MessageBoxImage.Error);
                Environment.Exit(1);
            }

            Trace.WriteLine($"カメラ情報取得に成功しました", "Debug");
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

        /// <summary>データ送信 </summary>
        /// <param name="pipe">送信するパイプ</param>
        /// <param name="cmd">送信するコマンド</param>
        /// <returns></returns>
        private void SendData(NamedPipeClientStream pipe, string cmd)
        {
            //送受信するバイト配列
            var cmdBytes = Encoding.UTF8.GetBytes(cmd);
            var recvBuf = new byte[pipe.InBufferSize];

            //送信関係デバッグ出力
            Trace.WriteLine($"送信文字列\n{cmd}", "Debug");
            var sb = new StringBuilder();
            sb.AppendLine("送信バイト配列\n");
            foreach (var cmdByte in cmdBytes)
            {
                sb.Append($"{cmdByte:X2} ");
            }
            Trace.WriteLine($"{sb.ToString()}", "Debug");
            pipe.Write(cmdBytes, 0, cmdBytes.Length);
        }

        /// <summary> データ受信  </summary>
        /// <param name="pipe">受信するパイプ</param>
        /// <returns></returns>
        private string[] RecvData(NamedPipeClientStream pipe)
        {
            var recvBuf = new byte[pipe.InBufferSize];
            var cnt = pipe.Read(recvBuf, 0, recvBuf.Length);
            var tmpStr = Encoding.UTF8.GetString(recvBuf, 0, cnt).Split(new[] {','});
            return tmpStr;
        }
    }


    //-------------------------------------------------------------------------------------------------------




    //--------------------------------------------------------------------------------------------------------

    /// <summary> 送信データの構造体（C++のバイト数に合わせ方を設定）</summary>
    public class SetImageStruct
    {
        /// <summary>歪み補正の可否。チェック時は 「TRUE」 </summary>
        [MarshalAs(UnmanagedType.Bool)] public bool HasToUndistort;

        /// <summary>出力画像の幅 </summary>
        [MarshalAs(UnmanagedType.I4)] public int OutputWidth;

        /// <summary>出力画像の高さ </summary>
        [MarshalAs(UnmanagedType.I4)] public int OutputHeight;

        /// <summary>二値化の閾値１：threshold2/threshold1 = 2~3 が好ましい </summary>
        [MarshalAs(UnmanagedType.R8)] public double Threshold1;

        /// <summary>二値化の閾値２：threshold2/threshold1 = 2~3 が好ましい </summary>
        [MarshalAs(UnmanagedType.R8)] public double Threshold2;

        /// <summary>極座標で表す場合の半径の分解能 </summary>
        [MarshalAs(UnmanagedType.R8)] public double RhoResolution;

        /// <summary>極座標で表す場合の角度の分解能 </summary>
        [MarshalAs(UnmanagedType.R8)] public double ThetaResolution;

        /// <summary>閾値 </summary>
        [MarshalAs(UnmanagedType.I4)] public int Threshold;

        /// <summary>指定数値未満の長さの線分は、線分とはみなさない </summary>
        [MarshalAs(UnmanagedType.R8)] public double MinLineLength;

        /// <summary>指定数値以下のギャップは、同一線分とみなす </summary>
        [MarshalAs(UnmanagedType.R8)] public double MaxLineGap;

        /// <summary>指定数値だけ線分を伸長する。その際交点が発生したものは「交差している」とみなす </summary>
        [MarshalAs(UnmanagedType.I4)] public int Torelance;

        /// <summary>交差した際の角度が指定角度未満の場合、「交差しない（平行）」とみなす </summary>
        [MarshalAs(UnmanagedType.R8)] public double LowerThetaLimit;

        /// <summary>矩形領域内にある点のみを抽出。チェック時はTRUE </summary>
        [MarshalAs(UnmanagedType.Bool)] public bool HasToMask;

        /// <summary>矩形領域左上の点(x1,y1)のx座標 </summary>
        [MarshalAs(UnmanagedType.I4)] public int X1;

        /// <summary>矩形領域左上の点(x1,y1)のy座標 </summary>
        [MarshalAs(UnmanagedType.I4)] public int Y1;

        /// <summary>矩形領域の幅 </summary>
        [MarshalAs(UnmanagedType.I4)] public int RecWidth;

        /// <summary>矩形領域の高さ </summary>
        [MarshalAs(UnmanagedType.I4)] public int RecHeight;

        /// <summary>ガウシアンフィルタのカーネルサイズ(x方向)[pixel]</summary>
        [MarshalAs(UnmanagedType.I4)] public int KSizeX;

        /// <summary>ガウシアンフィルタのカーネルサイズ(y方向)[pixel]</summary>
        [MarshalAs(UnmanagedType.I4)] public int KSizeY;

        /// <summary>ガウシアンフィルタのカーネルの標準偏差(x方向)[pixel]</summary>
        [MarshalAs(UnmanagedType.R8)] public double SigmaX;

        /// <summary>ガウシアンフィルタのカーネルの標準偏差(y方向)[pixel]</summary>
        [MarshalAs(UnmanagedType.R8)] public double SigmaY;

        /// <summary>2直線の(符号付き)角度の差の絶対値が slopeAngleResolution 未満であれば、2直線の傾きを等しいとみなす。単位は[deg]</summary>
        [MarshalAs(UnmanagedType.R8)] public double SlopeAngleResolution;

        /// <summary>2直線の(符号付き)y切片の差の絶対値が intersectResolution 未満であれば、2直線のy切片を等しいとみなす。単位は[pixel]</summary>
        [MarshalAs(UnmanagedType.R8)] public double IntersectResolution;

        /// <summary> 一枚目貼り付け時の直線検出用 </summary>
        [MarshalAs(UnmanagedType.I4)] public int Point1X;

        /// <summary> 一枚目貼り付け時の直線検出用 </summary>
        [MarshalAs(UnmanagedType.I4)] public int Point1Y;

        /// <summary> 一枚目貼り付け時の直線検出用 </summary>
        [MarshalAs(UnmanagedType.I4)] public int Point2X;

        /// <summary> 一枚目貼り付け時の直線検出用 </summary>
        [MarshalAs(UnmanagedType.I4)] public int Point2Y;

        /// <summary> 板　バージョンのレファレンスポイントX </summary>
        [MarshalAs(UnmanagedType.I4)] public int RefCornerPointX;

        /// <summary> 板　バージョンのレファレンスポイントY </summary>
        [MarshalAs(UnmanagedType.I4)] public int RefCornerPointY;

        /// <summary> 板　バージョンのリファレンス角度 </summary>
        [MarshalAs(UnmanagedType.I4)] public int RefCornerAngle;

        /// <summary> 板　バージョンのレファレンスポイントX2 </summary>
        [MarshalAs(UnmanagedType.I4)] public int RefCornerPointX2;

        /// <summary> 板　バージョンのレファレンスポイントY2  </summary>
        [MarshalAs(UnmanagedType.I4)] public int RefCornerPointY2;

        /// <summary> バイラテラルフィルタ用の係数 </summary>
        [MarshalAs(UnmanagedType.I4)] public int BirateralKernel;

        /// <summary> バイラテラルフィルタ用の係数 </summary>
        [MarshalAs(UnmanagedType.R8)] public double SigmaSpace;

        /// <summary> バイラテラルフィルタ用の係数 </summary>
        [MarshalAs(UnmanagedType.R8)] public double SigmaColer;

        /// <summary> コーナー検索用のサイズ </summary>
        [MarshalAs(UnmanagedType.I4)] public int SearchLength;

        /// <summary> コーナー検索用のサイズ </summary>
        [MarshalAs(UnmanagedType.I4)] public int SearchLine;

        /// <summary> チェスボード関係の設定 </summary>
        [MarshalAs(UnmanagedType.I4)] public int ChessCrossWidth;

        /// <summary> チェスボード関係の設定 </summary>
        [MarshalAs(UnmanagedType.I4)] public int ChessCrossHeigth;

        /// <summary> チェスボード関係の設定 </summary>
        [MarshalAs(UnmanagedType.R8)] public double ChessSquareSize;

        /// <summary> チェスボード関係の設定 </summary>
        [MarshalAs(UnmanagedType.I4)] public int OffsetX;

        /// <summary> チェスボード関係の設定 </summary>
        [MarshalAs(UnmanagedType.I4)] public int OffsetY;

        /// <summary> ViewModelの値を構造体に代入 </summary>
        public SetImageStruct GetSettingStruct(CamViewAreaDVModel data)
        {
            var setStruct = new SetImageStruct
            {
                HasToUndistort = data.HasToUndistort,
                OutputWidth = data.OutputWidth,
                OutputHeight = data.OutputHeight,
                Threshold1 = data.Threshold1,
                Threshold2 = data.Threshold2,
                Threshold = data.Threshold,
                RhoResolution = data.RhoResolution,
                ThetaResolution = data.ThetaResolution,
                MinLineLength = data.MinLineLength,
                MaxLineGap = data.MaxLineGap,
                Torelance = data.Torelance,
                LowerThetaLimit = data.LowerThetaLimit,
                HasToMask = data.HasToMask,
                X1 = data.X1,
                Y1 = data.Y1,
                RecWidth = data.RecWidth,
                RecHeight = data.RecHeight,
                KSizeX = data.KSizeX,
                KSizeY = data.KSizeY,
                SigmaX = data.SigmaX,
                SigmaY = data.SigmaY,
            };
            return setStruct;
        }

        /// <summary> 送信用のバイト配列を取得する </summary>
        /// <param name="judgStr">ヘッダー判定子</param>
        /// <param name="sStruct">構造体</param>
        /// <returns></returns>
        public byte[] StructToByte(string judgStr, SetImageStruct sStruct)
        {
            //ヘッダー：判定部
            var headJudgBt = Encoding.UTF8.GetBytes(judgStr);

            //ヘッダー：サイズ
            var headSizeInt = Marshal.SizeOf(typeof(SetImageStruct));

            //構造体をbyteに変更
            var strBytes = new byte[headSizeInt];
            var ptr = Marshal.AllocHGlobal(headSizeInt);
            Marshal.StructureToPtr(sStruct, ptr, true);
            Marshal.Copy(ptr, strBytes, 0, headSizeInt);
            Marshal.FreeHGlobal(ptr);

            var sendStrAllSize = Encoding.UTF8.GetBytes(headSizeInt.ToString() + ",");
            var tempBt = headJudgBt.Concat(sendStrAllSize).Concat(strBytes).ToArray();

            return tempBt;
        }
    }


    //----------------------------------------------------------------------------------------------------------

    /// <summary> XMLへの設定保存用クラス </summary>
    [Serializable]
    [XmlRoot("cereal")]
    public class SettingsClass
    {
        [XmlElement("Setting_Params")]
        public SettingClassInfo Info { set; get; }
    }

    /// <summary> XMLへの保存用クラス </summary>
    [Serializable]
    public class SettingClassInfo
    {
        [XmlElement("hasToUndistort")]
        public bool HasToUndistort { get; set; }

        [XmlElement("outputWidth")]
        public int OutputWidth { get; set; }

        [XmlElement("outputHeight")]
        public int OutputHeight { get; set; }

        [XmlElement("threshold1")]
        public double Threshold1 { get; set; }

        [XmlElement("threshold2")]
        public double Threshold2 { get; set; }

        [XmlElement("rhoResolution")]
        public double RhoResolution { get; set; }

        [XmlElement("thetaResolution")]
        public double ThetaResolution { get; set; }

        [XmlElement("threshold")]
        public int Threshold { get; set; }

        [XmlElement("minLineLength")]
        public double MinLineLength { get; set; }

        [XmlElement("maxLineGap")]
        public double MaxLineGap { get; set; }

        [XmlElement("torelance")]
        public int Torelance { get; set; }

        [XmlElement("lowerThetaLimit")]
        public double LowerThetaLimit { get; set; }

        [XmlElement("hasToMask")]
        public bool HasToMask { get; set; }

        [XmlElement("x1")]
        public int X1 { get; set; }

        [XmlElement("y1")]
        public int Y1 { get; set; }

        [XmlElement("recWidth")]
        public int RecWidth { get; set; }

        [XmlElement("recHeight")]
        public int RecHeight { get; set; }

        [XmlElement("filterX")]
        public int KSizeX { get; set; }

        [XmlElement("filterY")]
        public int KSizeY { get; set; }

        [XmlElement("xSigma")]
        public double SigmaX { get; set; }

        [XmlElement("ySigma")]
        public double SigmaY { get; set; }

		[XmlElement("slopeAngleResolution")]
		public double SlopeAngleResolution { get; set; }

		[XmlElement("iIntersectResolution")]
		public double IntersectResolution { get; set; }

		[XmlElement("point1X")]
		public int Point1X { get; set; }

		[XmlElement("point1Y")]
		public int Point1Y { get; set; }

		[XmlElement("point2X")]
		public int Point2X { get; set; }

		[XmlElement("point2Y")]
		public int Point2Y { get; set; }

		[XmlElement("refCornerPointX")]
		public int RefCornerPointX { get; set; }

		[XmlElement("refCornerPointY")]
		public int RefCornerPointY { get; set; }

		[XmlElement("refCornerAngle")]
		public int RefCornerAngle { get; set; }

		[XmlElement("refCornerPointX2")]
		public int RefCornerPointX2 { get; set; }

		[XmlElement("refCornerPointY2")]
		public int RefCornerPointY2 { get; set; }

		[XmlElement("birateralKernel")]
		public int BirateralKernel { get; set; }

		[XmlElement("sigmaSpace")]
		public double SigmaSpace { get; set; }

		[XmlElement("sigmaColer")]
		public double SigmaColer { get; set; }

		[XmlElement("searchLength")]
		public int SearchLength { get; set; }

		[XmlElement("searchLine")]
		public int SearchLine { get; set; }

		[XmlElement("chessCrossWidth")]
		public int ChessCrossWidth { get; set; }

		[XmlElement("chessCrossHeigth")]
		public int ChessCrossHeigth { get; set; }

		[XmlElement("chessSquareSize")]
		public double ChessSquareSize { get; set; }

		[XmlElement("offsetX")]
		public int OffsetX { get; set; }

		[XmlElement("offsetY")]
		public int OffsetY { get; set; }

        /// <summary> ファイル保存用のクラスを生成する </summary>
        public static SettingsClass GetStructSettings(CamViewAreaDVModel data)
        {
            var tmp = new SettingsClass()
            {
                Info = new SettingClassInfo()
                {
                    HasToUndistort = data.HasToUndistort,
                    OutputWidth = data.OutputWidth,
                    OutputHeight = data.OutputHeight,
                    Threshold1 = data.Threshold1,
                    Threshold2 = data.Threshold2,
                    Threshold = data.Threshold,
                    RhoResolution = data.RhoResolution,
                    ThetaResolution = data.ThetaResolution,
                    MinLineLength = data.MinLineLength,
                    MaxLineGap = data.MaxLineGap,
                    Torelance = data.Torelance,
                    LowerThetaLimit = data.LowerThetaLimit,
                    HasToMask = data.HasToMask,
                    X1 = data.X1,
                    Y1 = data.Y1,
                    RecWidth = data.RecWidth,
                    RecHeight = data.RecHeight,
                    KSizeX = data.KSizeX,
                    KSizeY = data.KSizeY,
                    SigmaX = data.SigmaX,
                    SigmaY = data.SigmaY,
                    SlopeAngleResolution = data.SlopeAngleResolution,
                    IntersectResolution = data.IntersectResolution,
                    Point1X = data.Point1X,
                    Point1Y = data.Point1Y,
                    Point2X = data.Point2X,
                    Point2Y = data.Point2Y,
                    RefCornerPointX = data.RefCornerPointX,
                    RefCornerPointY = data.RefCornerPointY,
                    RefCornerAngle = data.RefCornerAngle,
                    RefCornerPointX2 = data.RefCornerPointX2,
                    RefCornerPointY2 = data.RefCornerPointY2,
                    BirateralKernel = data.BirateralKernel,
                    SigmaSpace = data.SigmaSpace,
                    SigmaColer = data.SigmaColer,
                    SearchLength = data.SearchLength,
                    SearchLine = data.SearchLine,
                    ChessCrossWidth = data.ChessCrossWidth,
                    ChessCrossHeigth = data.ChessCrossHeigth,
                    ChessSquareSize = data.ChessSquareSize,
                    OffsetX = data.OffsetX,
                    OffsetY = data.OffsetY
                }
            };
            return tmp;
        }
    }

    //-------------------------------------------------------------------------------------

    /// <summary> TCP設定 </summary>
    [Serializable]
    public class TcpSettings
    {
        /// <summary> サーバー名 </summary>
        [XmlElement]
        public string ServerName { set; get; }

        /// <summary> パイプ名 </summary>
        [XmlElement]
        public string PipeNmae { set; get; }

        /// <summary> パイプの方向性 </summary>
        [XmlElement]
        public PipeDirection PipeDirection { set; get; }

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
        public static TcpSettings LoadFromFile(string fname)
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

    //-----------------------------------------------------------------------------------------------


}

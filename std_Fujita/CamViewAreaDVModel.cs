using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Serialization;


namespace std_Fujita
{
    /// <summary> ViewMode用クラス </summary>
    [Serializable]
    public class CamViewAreaDVModel : ViewModelBase, INotifyDataErrorInfo
    {
        /// <summary> コンストラクタ </summary>
        public CamViewAreaDVModel()
        {
        }

        public IEnumerable GetErrors(string propertyName)
        {
            IEnumerable error = null;
            this.errors.TryGetValue(propertyName, out error);
            return error;
        }

        public bool HasErrors { get; }
        public event EventHandler<DataErrorsChangedEventArgs> ErrorsChanged;

        private Dictionary<string, IEnumerable> errors = new Dictionary<string, IEnumerable>();
        
		/// <summary> 歪み補正をするか否か bool </summary>
        private bool hasToUndistort;

		/// <summary> 歪み補正をするか否か string </summary>
        public static string HasToUndistortStr { set; get; } = "歪み補正の可否。チェック時は 「TRUE」";

        /// <summary> 歪み補正をするか否か </summary>
        public bool HasToUndistort
        {
            set
            {
                hasToUndistort = value;
                RaisePropertyChanged(nameof(HasToUndistort));
            }
            get { return hasToUndistort; }
        }

		/// <summary> 出力する画像の幅[pixel] int </summary>
        private int outputWidth = 1280;

		/// <summary> 出力する画像の幅 string </summary>
        public static string OutputWidthStr { set; get; } = "出力画像の幅";

        /// <summary> 出力する画像の幅[pixel] </summary>
        public int OutputWidth
        {
            set
            {
                outputWidth = value;
                RaisePropertyChanged(nameof(OutputWidth));
            }
            get { return outputWidth; }
        }

		/// <summary> 出力する画像の高さ[pixel] int </summary>
        private int outputHeight =1024;

		/// <summary> 出力する画像の高さ string </summary>
        public static string OutputHeightStr { set; get; } = "出力画像の高さ";

		/// <summary> 出力する画像の高さ[pixel] </summary>
        public int OutputHeight
        {
            set
            {
                outputHeight = value;
                RaisePropertyChanged(nameof(OutputHeight));
            }
            get { return outputHeight; }
        }

		/// <summary> 二値化の閾値１ </summary>
        private double threshold1 =8;

		/// <summary> 二値化の閾値１ </summary>
        public static string Threshold1Str { set; get; } = "二値化の閾値１：threshold2/threshold1 = 2~3 が好ましい";

        /// <summary> 二値化の閾値１ </summary>
        public double Threshold1
        {
            set
            {
                threshold1 = value;
                RaisePropertyChanged(nameof(Threshold1));
            }
            get { return threshold1; }
        }

		/// <summary> 二値化の閾値２ </summary>
        private double threshold2 = 28;

		/// <summary> 二値化の閾値２ </summary>
		public static string Threshold2Str { set; get; } = "二値化の閾値２：threshold2/threshold1 = 2~3 が好ましい";

		/// <summary> 二値化の閾値２ </summary>
        public double Threshold2
        {
            set
            {
                threshold2 = value;
                RaisePropertyChanged(nameof(Threshold2));
            }
            get { return threshold2; }
        }

		/// <summary> 極座標で表す場合の半径の分解能 </summary>
        private double rhoResolution=1;

		/// <summary> 極座標で表す場合の半径の分解能 </summary>
        public static string RhoResolutionStr { set; get; } = "極座標で表す場合の半径の分解能";

		/// <summary> 極座標で表す場合の半径の分解能 </summary>
        public double RhoResolution
        {
            set
            {
                rhoResolution = value;
                RaisePropertyChanged(nameof(RhoResolution));
            }
            get { return rhoResolution; }
        }

        /// <summary> 極座標で表す場合の角度の分解能 </summary>
		private double thetaResolution = 1;

        /// <summary> 極座標で表す場合の角度の分解能 </summary>
        public static string ThetaResolutionStr { set; get; } = "極座標で表す場合の角度の分解能";

        /// <summary> 極座標で表す場合の角度の分解能 </summary>
        public double ThetaResolution
        {
            set
            {
                thetaResolution = value;
                RaisePropertyChanged(nameof(ThetaResolution));
            }
            get { return thetaResolution; }
        }

		/// <summary> 閾値 </summary>
        private int threshold = 78;

		/// <summary> 閾値 </summary>
		public static string ThresholdStr { set; get; } = "閾値";

		/// <summary> 閾値 </summary>
        public int Threshold
        {
            set
            {
                threshold = value;
                RaisePropertyChanged(nameof(Threshold));
            }
            get { return threshold; }
        }

		/// <summary> 指定数値未満の長さの線分は、線分とはみなさない[pixel]  </summary>
		private double minLineLength = 94;

		/// <summary> 指定数値未満の長さの線分は、線分とはみなさない  </summary>
        public static string MinLineLengthStr { set; get; } = "指定数値未満の長さの線分は、線分とはみなさない";

        /// <summary> 指定数値未満の長さの線分は、線分とはみなさない  </summary>
        public double MinLineLength
        {
            set
            {
                minLineLength = value;
                RaisePropertyChanged(nameof(MinLineLength));
            }
            get { return minLineLength; }
        }

		/// <summary> 指定数値以下のギャップは、同一線分とみなす[pixel] </summary>
        private double maxLineGap = 29;

		/// <summary> 指定数値以下のギャップは、同一線分とみなす </summary>
        public static string MaxLineGapStr { set; get; } = "指定数値以下のギャップは、同一線分とみなす";

		/// <summary> 指定数値以下のギャップは、同一線分とみなす </summary>
        public double MaxLineGap
        {
            set
            {
                maxLineGap = value;
                RaisePropertyChanged(nameof(MaxLineGap));
            }
            get { return maxLineGap; }
        }

		/// <summary> 指定数値だけ線分を伸長する。その際交点が発生したものは「交差している」とみなす[pixel] </summary>
		private int torelance = 30;

		/// <summary> 指定数値だけ線分を伸長する。その際交点が発生したものは「交差している」とみなす </summary>
        public static string TorelanceStr { set; get; } = "指定数値だけ線分を伸長する。その際交点が発生したものは「交差している」とみなす";

		/// <summary> 指定数値だけ線分を伸長する。その際交点が発生したものは「交差している」とみなす </summary>
        public int Torelance
        {
            set
            {
                torelance = value;
                RaisePropertyChanged(nameof(Torelance));
            }
            get { return torelance; }
        }

		/// <summary> 交差した際の角度が指定角度未満の場合、「交差しない（平行）」とみなす[pixel] </summary>
		private double lowerThetaLimit = 30.0;

		/// <summary> 交差した際の角度が指定角度未満の場合、「交差しない（平行）」とみなす </summary>
        public static string LowerThetaLimitStr { set; get; } = "交差した際の角度が指定角度未満の場合、「交差しない（平行）」とみなす";

		/// <summary> 交差した際の角度が指定角度未満の場合、「交差しない（平行）」とみなす </summary>
        public double LowerThetaLimit
        {
            set
            {
                lowerThetaLimit = value;
                RaisePropertyChanged(nameof(LowerThetaLimit));
            }
            get { return lowerThetaLimit; }
        }

		/// <summary> 矩形領域内にある点のみを抽出。チェック時はTRUE </summary>
        private bool hasToMask;

		/// <summary> 矩形領域内にある点のみを抽出。チェック時はTRUE </summary>
        public static string HasToMaskStr { set; get; } = "矩形領域内にある点のみを抽出。チェック時はTRUE";

		/// <summary> 矩形領域内にある点のみを抽出。チェック時はTRUE </summary>
        public bool HasToMask
        {
            set
            {
                hasToMask = value;
                RaisePropertyChanged(nameof(HasToMask));
            }
            get { return hasToMask; }
        }

		/// <summary> 矩形領域左上の点(x1,y1)のx座標[pixcel] </summary>
        private int x1=500;

		/// <summary> 矩形領域左上の点(x1,y1)のx座標 </summary>
        public static string X1Str { set; get; } = "矩形領域左上の点(x1,y1)のx座標";

		/// <summary> 矩形領域左上の点(x1,y1)のx座標[pixcel] </summary>
        public int X1
        {
            set
            {
                x1 = value;
                RaisePropertyChanged(nameof(X1));
            }
            get { return x1; }
        }

		/// <summary> 矩形領域左上の点(x1,y1)のy座標 [pixcel]</summary>
        private int y1=120;

		/// <summary> 矩形領域左上の点(x1,y1)のy座標 </summary>
        public static string Y1Str { set; get; } = "矩形領域左上の点(x1,y1)のy座標";

		/// <summary> 矩形領域左上の点(x1,y1)のy座標 </summary>
        public int Y1
        {
            set
            {
                y1 = value;
                RaisePropertyChanged(nameof(Y1));
            }
            get { return y1; }
        }

		/// <summary> 矩形領域の幅 [pixcel]</summary>
        private int recWidth=300;

		/// <summary> 矩形領域の幅 </summary>
        public static string RecWidthStr { set; get; } = "矩形領域の幅";

		/// <summary> 矩形領域の幅 </summary>
        public int RecWidth
        {
            set
            {
                recWidth = value;
                RaisePropertyChanged(nameof(recWidth));
            }
            get { return recWidth; }
        }

		/// <summary> 矩形領域の高さ[pixcel] </summary>
		private int recHeight=300;

		/// <summary> 矩形領域の高さ </summary>
        public static string RecHeightStr { set; get; } = "矩形領域の高さ";

		/// <summary> 矩形領域の高さ </summary>
        public int RecHeight
        {
            set
            {
                recHeight = value;
                RaisePropertyChanged(nameof(RecHeight));
            }
            get { return recHeight; }
        }

		/// <summary> ガウシアンフィルタのカーネルサイズ(x方向)[pixel] </summary>
        private int kSizeX = 9;

		/// <summary> ガウシアンフィルタのカーネルサイズ(x方向) </summary>
        public static string FilterXStr { set; get; } = "ガウシアンフィルタのカーネルサイズ(x方向)[pixel]";

		/// <summary> ガウシアンフィルタのカーネルサイズ(x方向) </summary>
        public int KSizeX
        {
            set
            {
                kSizeX = value;
                RaisePropertyChanged(nameof(KSizeX));
            }
            get { return kSizeX; }
        }

		/// <summary> ガウシアンフィルタのカーネルサイズ(y方向)[pixel] </summary>
        private int kSizeY = 9;

		/// <summary> ガウシアンフィルタのカーネルサイズ(y方向) </summary>
        public static string FilterYStr { set; get; } = "ガウシアンフィルタのカーネルサイズ(y方向)[pixel]";

		/// <summary> ガウシアンフィルタのカーネルサイズ(y方向) </summary>
        public int KSizeY
        {
            set
            {
                kSizeY = value;
                RaisePropertyChanged(nameof(KSizeY));
            }
            get { return kSizeY; }
        }

		/// <summary> ガウシアンフィルタのカーネルの標準偏差(x方向)[pixel] </summary>
		private double sigmaX = 0;

		/// <summary> ガウシアンフィルタのカーネルの標準偏差(x方向) </summary>
        public static string SigmaXStr { set; get; } = "ガウシアンフィルタのカーネルの標準偏差(x方向)[pixel]";

		/// <summary> ガウシアンフィルタのカーネルの標準偏差(x方向) </summary>
        public double SigmaX
        {
            set
            {
                sigmaX = value;
                RaisePropertyChanged(nameof(SigmaX));
            }
            get { return sigmaX; }
        }

		/// <summary> ガウシアンフィルタのカーネルの標準偏差(y方向)[pixel] </summary>
        private double sigmaY = 0;

		/// <summary> ガウシアンフィルタのカーネルの標準偏差(y方向) </summary>
        public static string SigmaYStr { set; get; } = "ガウシアンフィルタのカーネルの標準偏差(y方向)[pixel]";

		/// <summary> ガウシアンフィルタのカーネルの標準偏差(y方向) </summary>
        public double SigmaY
        {
            set
            {
                sigmaY = value;
                RaisePropertyChanged(nameof(SigmaY));
            }
            get { return sigmaY; }
        }

        /// <summary>2直線の(符号付き)角度の差の絶対値が slopeAngleResolution 未満であれば、2直線の傾きを等しいとみなす。単位は[deg]</summary>
        private double slopeAngleResolution  = 0;

        /// <summary>2直線の(符号付き)角度の差の絶対値が slopeAngleResolution 未満であれば、2直線の傾きを等しいとみなす。単位は[deg]</summary>
        public static string SlopeAngleResolutionStr { set; get; } = "";

        /// <summary>2直線の(符号付き)角度の差の絶対値が slopeAngleResolution 未満であれば、2直線の傾きを等しいとみなす。単位は[deg]</summary>
		public double SlopeAngleResolution
        {
            set
            {
                 slopeAngleResolution = value;
                RaisePropertyChanged(nameof(SlopeAngleResolution));
            }
            get { return slopeAngleResolution; }
        }
		
        /// <summary>2直線の(符号付き)y切片の差の絶対値が intersectResolution 未満であれば、2直線のy切片を等しいとみなす。単位は[pixel]</summary>
        private double intersectResolution = 0;

        /// <summary>2直線の(符号付き)y切片の差の絶対値が intersectResolution 未満であれば、2直線のy切片を等しいとみなす。単位は[pixel]</summary>
        public static string IntersectResolutionStr { set; get; } = "";

        /// <summary>2直線の(符号付き)y切片の差の絶対値が intersectResolution 未満であれば、2直線のy切片を等しいとみなす。単位は[pixel]</summary>
	    public double IntersectResolution
	    {
		    set
		    {
			    intersectResolution = value;
			    RaisePropertyChanged(nameof(IntersectResolution));
		    }
		    get { return intersectResolution; }
	    }

		/// <summary> 一枚目貼り付け時の直線検出用 X</summary>
        private int point1X = 0;

		/// <summary> 一枚目貼り付け時の直線検出用 X</summary>
        public static string Point1XStr { set; get; } = "";

		/// <summary> 一枚目貼り付け時の直線検出用 X</summary>
		public int Point1X
        {
            set
            {
                 point1X = value;
                RaisePropertyChanged(nameof(Point1X));
            }
            get { return point1X; }
        }

		/// <summary> 一枚目貼り付け時の直線検出用 Y</summary>
        private int point1Y = 0;

		/// <summary> 一枚目貼り付け時の直線検出用 Y</summary>
        public static string Point1YStr { set; get; } = "";

		/// <summary> 一枚目貼り付け時の直線検出用 Y</summary>
		public int Point1Y
        {
            set
            {
                 point1Y = value;
                RaisePropertyChanged(nameof(Point1Y));
            }
            get { return point1Y; }
        }

		/// <summary> 一枚目貼り付け時の直線検出用 X</summary>
        private int point2X = 0;

		/// <summary> 一枚目貼り付け時の直線検出用 X</summary>
        public static string Point2XStr { set; get; } = "";

		/// <summary> 一枚目貼り付け時の直線検出用 X</summary>
        public int Point2X
        {
            set
            {
                point2X = value;
                RaisePropertyChanged(nameof(Point2X));
            }
            get { return point2X; }
        }

		/// <summary> 一枚目貼り付け時の直線検出用 Y</summary>
        private int point2Y = 0;

		/// <summary> 一枚目貼り付け時の直線検出用 Y</summary>
        public static string Point2YStr { set; get; } = "";

		/// <summary> 一枚目貼り付け時の直線検出用 Y</summary>
        public int Point2Y
        {
            set
            {
                point2Y = value;
                RaisePropertyChanged(nameof(Point2Y));
            }
            get { return point2Y; }
        }

		/// <summary> 板　バージョンのレファレンスポイントX </summary>
        private int refCornerPointX = 0;

		/// <summary> 板　バージョンのレファレンスポイントX </summary>
        public static string RefCornerPointXStr { set; get; } = "";

		/// <summary> 板　バージョンのレファレンスポイントX </summary>
		public int RefCornerPointX
        {
            set
            {
                 refCornerPointX = value;
                RaisePropertyChanged(nameof(RefCornerPointX));
            }
            get { return refCornerPointX; }
        }

		/// <summary> 板　バージョンのレファレンスポイントY </summary>
        private int refCornerPointY = 0;

		/// <summary> 板　バージョンのレファレンスポイントY </summary>
        public static string RefCornerPointYStr { set; get; } = "";

		/// <summary> 板　バージョンのレファレンスポイントY </summary>
		public int RefCornerPointY
        {
            set
            {
                 refCornerPointY = value;
                RaisePropertyChanged(nameof(RefCornerPointY));
            }
            get { return refCornerPointY; }
        }

		/// <summary> 板　バージョンのリファレンス角度 </summary>
        private int refCornerAngle = 0;

		/// <summary> 板　バージョンのリファレンス角度 </summary>
        public static string RefCornerAngleStr { set; get; } = "";

		/// <summary> 板　バージョンのリファレンス角度 </summary>
        public int RefCornerAngle
        {
            set
            {
                 refCornerAngle = value;
                RaisePropertyChanged(nameof(RefCornerAngle));
            }
            get { return refCornerAngle; }
        }

		/// <summary> 板　バージョンのレファレンスポイントX2 </summary>
        private int refCornerPointX2 = 0;

		/// <summary> 板　バージョンのレファレンスポイントX2 </summary>
        public static string RefCornerPointX2Str { set; get; } = "";

		/// <summary> 板　バージョンのレファレンスポイントX2 </summary>
        public int RefCornerPointX2
        {
            set
            {
                 refCornerPointX2 = value;
                RaisePropertyChanged(nameof(RefCornerPointX2));
            }
            get { return  refCornerPointX2; }
        }

		/// <summary> 板　バージョンのレファレンスポイントY2  </summary>
        private int refCornerPointY2 = 0;

		/// <summary> 板　バージョンのレファレンスポイントY2  </summary>
        public static string RefCornerPointY2Str { set; get; } = "";

		/// <summary> 板　バージョンのレファレンスポイントY2  </summary>
        public int RefCornerPointY2
        {
            set
            {
                 refCornerPointY2 = value;
                RaisePropertyChanged(nameof(RefCornerPointY2));
            }
            get { return refCornerPointY2; }
        }

		/// <summary> バイラテラルフィルタ用の係数 </summary>
        private int birateralKernel = 0;

		/// <summary> バイラテラルフィルタ用の係数 </summary>
        public static string BirateralKernelStr { set; get; } = "";

		/// <summary> バイラテラルフィルタ用の係数 </summary>
        public int BirateralKernel
        {
            set
            {
                 birateralKernel = value;
                RaisePropertyChanged(nameof(BirateralKernel));
            }
            get { return birateralKernel; }
        }

		/// <summary> バイラテラルフィルタ用の係数 </summary>
        private double sigmaSpace = 0;

		/// <summary> バイラテラルフィルタ用の係数 </summary>
        public static string SigmaSpaceStr { set; get; } = "";

		/// <summary> バイラテラルフィルタ用の係数 </summary>
        public double SigmaSpace
        {
            set
            {
                 sigmaSpace = value;
                RaisePropertyChanged(nameof(SigmaSpace));
            }
            get { return sigmaSpace; }
        }

		/// <summary> バイラテラルフィルタ用の係数 </summary>
        private double sigmaColer = 0;

		/// <summary> バイラテラルフィルタ用の係数 </summary>
        public static string SigmaColerStr { set; get; } = "";

		/// <summary> バイラテラルフィルタ用の係数 </summary>
		public double SigmaColer
        {
            set
            {
                sigmaColer = value;
                RaisePropertyChanged(nameof(SigmaColer));
            }
            get { return sigmaColer; }
        }

		/// <summary> コーナー検索用のサイズ </summary>
        private int searchLength = 0;

		/// <summary> コーナー検索用のサイズ </summary>
        public static string SearchLengthStr { set; get; } = "";

		/// <summary> コーナー検索用のサイズ </summary>
		public int SearchLength
        {
            set
            {
                searchLength = value;
                RaisePropertyChanged(nameof(SearchLength));
            }
            get { return searchLength; }
        }

		/// <summary> コーナー検索用のサイズ </summary>
        private int searchLine = 0;

		/// <summary> コーナー検索用のサイズ </summary>
        public static string SearchLineStr { set; get; } = "";

		/// <summary> コーナー検索用のサイズ </summary>
        public int SearchLine
        {
            set
            {
                searchLine = value;
                RaisePropertyChanged(nameof(SearchLine));
            }
            get { return searchLine; }
        }

        /// <summary> チェスボード関係の設定 </summary>
        private int chessCrossWidth = 0;

		/// <summary> チェスボード関係の設定 </summary>
        public static string ChessCrossWidthStr { set; get; } = "";

		/// <summary> チェスボード関係の設定 </summary>
        public int ChessCrossWidth
        {
            set
            {
                chessCrossWidth = value;
                RaisePropertyChanged(nameof(ChessCrossWidth));
            }
            get { return chessCrossWidth; }
        }

        /// <summary> チェスボード関係の設定 </summary>
        private int chessCrossHeigth = 0;

		/// <summary> チェスボード関係の設定 </summary>
        public static string ChessCrossHeigthStr { set; get; } = "";

		/// <summary> チェスボード関係の設定 </summary>
        public int ChessCrossHeigth
        {
            set
            {
                chessCrossHeigth = value;
                RaisePropertyChanged(nameof(ChessCrossHeigth));
            }
            get { return chessCrossHeigth ; }
        }

        /// <summary> チェスボード関係の設定 </summary>
        private double chessSquareSize = 0;

		/// <summary> チェスボード関係の設定 </summary>
        public static string ChessSquareSizeStr { set; get; } = "";

		/// <summary> チェスボード関係の設定 </summary>
        public double ChessSquareSize
        {
            set
            {
                chessSquareSize = value;
                RaisePropertyChanged(nameof(ChessSquareSize));
            }
            get { return chessSquareSize; }
        }

        /// <summary> チェスボード関係の設定 </summary>
        private int offsetX = 0;

		/// <summary> チェスボード関係の設定 </summary>
        public static string OffsetXStr { set; get; } = "";

		/// <summary> チェスボード関係の設定 </summary>
        public int OffsetX
        {
            set
            {
                offsetX = value;
                RaisePropertyChanged(nameof(OffsetX));
            }
            get { return offsetX; }
        }

        /// <summary> チェスボード関係の設定 </summary>
        private int offsetY = 0;

		/// <summary> チェスボード関係の設定 </summary>
        public static string OffsetYStr { set; get; } = "";

		/// <summary> チェスボード関係の設定 </summary>
        public int OffsetY
        {
            set
            {
                offsetY = value;
                RaisePropertyChanged(nameof(OffsetY));
            }
            get { return offsetY; }
        }

        /// <summary> XMLファイルを読み込んで設定更新  </summary>
        /// <param name="data"></param>
        /// <param name="fileName"></param>
        public static void ReadXmlToXaml(CamViewAreaDVModel data, string fileName)
        {
            var serializer = new XmlSerializer(typeof(SettingsClass));
            using (var sr = new StreamReader(fileName, new UTF8Encoding(false)))
            {
                var ans = (SettingsClass) serializer.Deserialize(sr);
                data.HasToUndistort = ans.Info.HasToUndistort;
                data.OutputWidth = ans.Info.OutputWidth;
                data.OutputHeight = ans.Info.OutputHeight;
                data.Threshold1 = ans.Info.Threshold1;
                data.Threshold2 = ans.Info.Threshold2;
                data.RhoResolution = ans.Info.RhoResolution;
                data.ThetaResolution = ans.Info.ThetaResolution;
                data.Threshold = ans.Info.Threshold;
                data.MinLineLength = ans.Info.MinLineLength;
                data.MaxLineGap = ans.Info.MaxLineGap;
                data.Torelance = ans.Info.Torelance;
                data.LowerThetaLimit = ans.Info.LowerThetaLimit;
                data.HasToMask = ans.Info.HasToMask;
                data.X1 = ans.Info.X1;
                data.Y1 = ans.Info.Y1;
                data.RecWidth = ans.Info.RecWidth;
                data.RecHeight = ans.Info.RecHeight;
                data.KSizeX = ans.Info.KSizeX;
                data.KSizeY = ans.Info.KSizeY;
                data.SigmaX = ans.Info.SigmaX;
                data.SigmaY = ans.Info.SigmaY;
                data.SlopeAngleResolution = ans.Info.SlopeAngleResolution;
                data.IntersectResolution = ans.Info.IntersectResolution;
                data.Point1X = ans.Info.Point1X;
                data.Point1Y = ans.Info.Point1Y;
                data.Point2X = ans.Info.Point2X;
                data.Point2Y = ans.Info.Point2Y;
                data.RefCornerPointX = ans.Info.RefCornerPointX;
                data.RefCornerPointY = ans.Info.RefCornerPointY;
                data.RefCornerAngle = ans.Info.RefCornerAngle;
                data.RefCornerPointX2 = ans.Info.RefCornerPointX2;
                data.RefCornerPointY2 = ans.Info.RefCornerPointY2;
                data.BirateralKernel = ans.Info.BirateralKernel;
                data.SigmaSpace = ans.Info.SigmaSpace;
                data.SigmaColer = ans.Info.SigmaColer;
                data.SearchLength = ans.Info.SearchLength;
                data.SearchLine = ans.Info.SearchLine;
                data.ChessCrossWidth = ans.Info.ChessCrossWidth;
                data.ChessCrossHeigth = ans.Info.ChessCrossHeigth;
                data.ChessSquareSize = ans.Info.ChessSquareSize;
                data.OffsetX = ans.Info.OffsetX;
                data.OffsetY = ans.Info.OffsetY;
            }
        }
    }
}
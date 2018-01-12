using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace std_Fujita
{
    ///// <summary> 送信データの構造体（C++のバイト数に合わせ方を設定）</summary>
    //public class SetImageStruct
    //{
    //    /// <summary>歪み補正の可否。チェック時は 「TRUE」 </summary>
    //    [MarshalAs(UnmanagedType.Bool)] public bool HasToUndistort;

    //    /// <summary>出力画像の幅 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int OutputWidth;

    //    /// <summary>出力画像の高さ </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int OutputHeight;

    //    /// <summary>二値化の閾値１：threshold2/threshold1 = 2~3 が好ましい </summary>
    //    [MarshalAs(UnmanagedType.R8)] public double Threshold1;

    //    /// <summary>二値化の閾値２：threshold2/threshold1 = 2~3 が好ましい </summary>
    //    [MarshalAs(UnmanagedType.R8)] public double Threshold2;

    //    /// <summary>極座標で表す場合の半径の分解能 </summary>
    //    [MarshalAs(UnmanagedType.R8)] public double RhoResolution;

    //    /// <summary>極座標で表す場合の角度の分解能 </summary>
    //    [MarshalAs(UnmanagedType.R8)] public double ThetaResolution;

    //    /// <summary>閾値 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int Threshold;

    //    /// <summary>指定数値未満の長さの線分は、線分とはみなさない </summary>
    //    [MarshalAs(UnmanagedType.R8)] public double MinLineLength;

    //    /// <summary>指定数値以下のギャップは、同一線分とみなす </summary>
    //    [MarshalAs(UnmanagedType.R8)] public double MaxLineGap;

    //    /// <summary>指定数値だけ線分を伸長する。その際交点が発生したものは「交差している」とみなす </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int Torelance;

    //    /// <summary>交差した際の角度が指定角度未満の場合、「交差しない（平行）」とみなす </summary>
    //    [MarshalAs(UnmanagedType.R8)] public double LowerThetaLimit;

    //    /// <summary>矩形領域内にある点のみを抽出。チェック時はTRUE </summary>
    //    [MarshalAs(UnmanagedType.Bool)] public bool HasToMask;

    //    /// <summary>矩形領域左上の点(x1,y1)のx座標 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int X1;

    //    /// <summary>矩形領域左上の点(x1,y1)のy座標 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int Y1;

    //    /// <summary>矩形領域の幅 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int RecWidth;

    //    /// <summary>矩形領域の高さ </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int RecHeight;

    //    /// <summary>ガウシアンフィルタのカーネルサイズ(x方向)[pixel]</summary>
    //    [MarshalAs(UnmanagedType.I4)] public int KSizeX;

    //    /// <summary>ガウシアンフィルタのカーネルサイズ(y方向)[pixel]</summary>
    //    [MarshalAs(UnmanagedType.I4)] public int KSizeY;

    //    /// <summary>ガウシアンフィルタのカーネルの標準偏差(x方向)[pixel]</summary>
    //    [MarshalAs(UnmanagedType.R8)] public double SigmaX;

    //    /// <summary>ガウシアンフィルタのカーネルの標準偏差(y方向)[pixel]</summary>
    //    [MarshalAs(UnmanagedType.R8)] public double SigmaY;

    //    /// <summary>2直線の(符号付き)角度の差の絶対値が slopeAngleResolution 未満であれば、2直線の傾きを等しいとみなす。単位は[deg]</summary>
    //    [MarshalAs(UnmanagedType.R8)] public double SlopeAngleResolution;

    //    /// <summary>2直線の(符号付き)y切片の差の絶対値が intersectResolution 未満であれば、2直線のy切片を等しいとみなす。単位は[pixel]</summary>
    //    [MarshalAs(UnmanagedType.R8)] public double IntersectResolution;

    //    /// <summary> 一枚目貼り付け時の直線検出用 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int Point1X;

    //    /// <summary> 一枚目貼り付け時の直線検出用 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int Point1Y;

    //    /// <summary> 一枚目貼り付け時の直線検出用 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int Point2X;

    //    /// <summary> 一枚目貼り付け時の直線検出用 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int Point2Y;

    //    /// <summary> 板　バージョンのレファレンスポイントX </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int RefCornerPointX;

    //    /// <summary> 板　バージョンのレファレンスポイントY </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int RefCornerPointY;

    //    /// <summary> 板　バージョンのリファレンス角度 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int RefCornerAngle;

    //    /// <summary> 板　バージョンのレファレンスポイントX2 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int RefCornerPointX2;

    //    /// <summary> 板　バージョンのレファレンスポイントY2  </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int RefCornerPointY2;

    //    /// <summary> バイラテラルフィルタ用の係数 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int BirateralKernel;

    //    /// <summary> バイラテラルフィルタ用の係数 </summary>
    //    [MarshalAs(UnmanagedType.R8)] public double SigmaSpace;

    //    /// <summary> バイラテラルフィルタ用の係数 </summary>
    //    [MarshalAs(UnmanagedType.R8)] public double SigmaColer;

    //    /// <summary> コーナー検索用のサイズ </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int SearchLength;

    //    /// <summary> コーナー検索用のサイズ </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int SearchLine;

    //    /// <summary> チェスボード関係の設定 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int ChessCrossWidth;

    //    /// <summary> チェスボード関係の設定 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int ChessCrossHeigth;

    //    /// <summary> チェスボード関係の設定 </summary>
    //    [MarshalAs(UnmanagedType.R8)] public double ChessSquareSize;

    //    /// <summary> チェスボード関係の設定 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int OffsetX;

    //    /// <summary> チェスボード関係の設定 </summary>
    //    [MarshalAs(UnmanagedType.I4)] public int OffsetY;

    //    /// <summary> ViewModelの値を構造体に代入 </summary>
    //    public SetImageStruct GetSettingStruct(CamViewAreaDVModel data)
    //    {
    //        var setStruct = new SetImageStruct
    //        {
    //            HasToUndistort = data.HasToUndistort,
    //            OutputWidth = data.OutputWidth,
    //            OutputHeight = data.OutputHeight,
    //            Threshold1 = data.Threshold1,
    //            Threshold2 = data.Threshold2,
    //            Threshold = data.Threshold,
    //            RhoResolution = data.RhoResolution,
    //            ThetaResolution = data.ThetaResolution,
    //            MinLineLength = data.MinLineLength,
    //            MaxLineGap = data.MaxLineGap,
    //            Torelance = data.Torelance,
    //            LowerThetaLimit = data.LowerThetaLimit,
    //            HasToMask = data.HasToMask,
    //            X1 = data.X1,
    //            Y1 = data.Y1,
    //            RecWidth = data.RecWidth,
    //            RecHeight = data.RecHeight,
    //            KSizeX = data.KSizeX,
    //            KSizeY = data.KSizeY,
    //            SigmaX = data.SigmaX,
    //            SigmaY = data.SigmaY,
    //        };
    //        return setStruct;
    //    }

    //    /// <summary> 送信用のバイト配列を取得する </summary>
    //    /// <param name="judgStr">ヘッダー判定子</param>
    //    /// <param name="sStruct">構造体</param>
    //    /// <returns></returns>
    //    public byte[] StructToByte(string judgStr, SetImageStruct sStruct)
    //    {
    //        //ヘッダー：判定部
    //        var headJudgBt = Encoding.UTF8.GetBytes(judgStr);

    //        //ヘッダー：サイズ
    //        var headSizeInt = Marshal.SizeOf(typeof(SetImageStruct));

    //        //構造体をbyteに変更
    //        var strBytes = new byte[headSizeInt];
    //        var ptr = Marshal.AllocHGlobal(headSizeInt);
    //        Marshal.StructureToPtr(sStruct, ptr, true);
    //        Marshal.Copy(ptr, strBytes, 0, headSizeInt);
    //        Marshal.FreeHGlobal(ptr);

    //        var sendStrAllSize = Encoding.UTF8.GetBytes(headSizeInt.ToString() + ",");
    //        var tempBt = headJudgBt.Concat(sendStrAllSize).Concat(strBytes).ToArray();

    //        return tempBt;
    //    }
    //}
}
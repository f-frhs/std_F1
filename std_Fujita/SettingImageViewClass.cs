using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Xml.Serialization;

namespace std_Fujita
{
  //  /// <summary> XMLへの設定保存用クラス </summary>
  //  [Serializable]
  //  [XmlRoot("cereal")]
  //  public class SettingsClass
  //  {
  //      [XmlElement("Setting_Params")]
  //      public SettingClassInfo Info { set; get; }
  //  }

  //  /// <summary> XMLへの保存用クラス </summary>
  //  [Serializable]
  //  public class SettingClassInfo
  //  {
  //      [XmlElement("hasToUndistort")]
  //      public bool HasToUndistort { get; set; }

  //      [XmlElement("outputWidth")]
  //      public int OutputWidth { get; set; }

  //      [XmlElement("outputHeight")]
  //      public int OutputHeight { get; set; }

  //      [XmlElement("threshold1")]
  //      public double Threshold1 { get; set; }

  //      [XmlElement("threshold2")]
  //      public double Threshold2 { get; set; }

  //      [XmlElement("rhoResolution")]
  //      public double RhoResolution { get; set; }

  //      [XmlElement("thetaResolution")]
  //      public double ThetaResolution { get; set; }

  //      [XmlElement("threshold")]
  //      public int Threshold { get; set; }

  //      [XmlElement("minLineLength")]
  //      public double MinLineLength { get; set; }

  //      [XmlElement("maxLineGap")]
  //      public double MaxLineGap { get; set; }

  //      [XmlElement("torelance")]
  //      public int Torelance { get; set; }

  //      [XmlElement("lowerThetaLimit")]
  //      public double LowerThetaLimit { get; set; }

  //      [XmlElement("hasToMask")]
  //      public bool HasToMask { get; set; }

  //      [XmlElement("x1")]
  //      public int X1 { get; set; }

  //      [XmlElement("y1")]
  //      public int Y1 { get; set; }

  //      [XmlElement("recWidth")]
  //      public int RecWidth { get; set; }

  //      [XmlElement("recHeight")]
  //      public int RecHeight { get; set; }

  //      [XmlElement("filterX")]
  //      public int KSizeX { get; set; }

  //      [XmlElement("filterY")]
  //      public int KSizeY { get; set; }

  //      [XmlElement("xSigma")]
  //      public double SigmaX { get; set; }

  //      [XmlElement("ySigma")]
  //      public double SigmaY { get; set; }

		//[XmlElement("slopeAngleResolution")]
		//public double SlopeAngleResolution { get; set; }

		//[XmlElement("iIntersectResolution")]
		//public double IntersectResolution { get; set; }

		//[XmlElement("point1X")]
		//public int Point1X { get; set; }

		//[XmlElement("point1Y")]
		//public int Point1Y { get; set; }

		//[XmlElement("point2X")]
		//public int Point2X { get; set; }

		//[XmlElement("point2Y")]
		//public int Point2Y { get; set; }

		//[XmlElement("refCornerPointX")]
		//public int RefCornerPointX { get; set; }

		//[XmlElement("refCornerPointY")]
		//public int RefCornerPointY { get; set; }

		//[XmlElement("refCornerAngle")]
		//public int RefCornerAngle { get; set; }

		//[XmlElement("refCornerPointX2")]
		//public int RefCornerPointX2 { get; set; }

		//[XmlElement("refCornerPointY2")]
		//public int RefCornerPointY2 { get; set; }

		//[XmlElement("birateralKernel")]
		//public int BirateralKernel { get; set; }

		//[XmlElement("sigmaSpace")]
		//public double SigmaSpace { get; set; }

		//[XmlElement("sigmaColer")]
		//public double SigmaColer { get; set; }

		//[XmlElement("searchLength")]
		//public int SearchLength { get; set; }

		//[XmlElement("searchLine")]
		//public int SearchLine { get; set; }

		//[XmlElement("chessCrossWidth")]
		//public int ChessCrossWidth { get; set; }

		//[XmlElement("chessCrossHeigth")]
		//public int ChessCrossHeigth { get; set; }

		//[XmlElement("chessSquareSize")]
		//public double ChessSquareSize { get; set; }

		//[XmlElement("offsetX")]
		//public int OffsetX { get; set; }

		//[XmlElement("offsetY")]
		//public int OffsetY { get; set; }

  //      /// <summary> ファイル保存用のクラスを生成する </summary>
  //      public static SettingsClass GetStructSettings(CamViewAreaDVModel data)
  //      {
  //          var tmp = new SettingsClass()
  //          {
  //              Info = new SettingClassInfo()
  //              {
  //                  HasToUndistort = data.HasToUndistort,
  //                  OutputWidth = data.OutputWidth,
  //                  OutputHeight = data.OutputHeight,
  //                  Threshold1 = data.Threshold1,
  //                  Threshold2 = data.Threshold2,
  //                  Threshold = data.Threshold,
  //                  RhoResolution = data.RhoResolution,
  //                  ThetaResolution = data.ThetaResolution,
  //                  MinLineLength = data.MinLineLength,
  //                  MaxLineGap = data.MaxLineGap,
  //                  Torelance = data.Torelance,
  //                  LowerThetaLimit = data.LowerThetaLimit,
  //                  HasToMask = data.HasToMask,
  //                  X1 = data.X1,
  //                  Y1 = data.Y1,
  //                  RecWidth = data.RecWidth,
  //                  RecHeight = data.RecHeight,
  //                  KSizeX = data.KSizeX,
  //                  KSizeY = data.KSizeY,
  //                  SigmaX = data.SigmaX,
  //                  SigmaY = data.SigmaY,
  //                  SlopeAngleResolution = data.SlopeAngleResolution,
  //                  IntersectResolution = data.IntersectResolution,
  //                  Point1X = data.Point1X,
  //                  Point1Y = data.Point1Y,
  //                  Point2X = data.Point2X,
  //                  Point2Y = data.Point2Y,
  //                  RefCornerPointX = data.RefCornerPointX,
  //                  RefCornerPointY = data.RefCornerPointY,
  //                  RefCornerAngle = data.RefCornerAngle,
  //                  RefCornerPointX2 = data.RefCornerPointX2,
  //                  RefCornerPointY2 = data.RefCornerPointY2,
  //                  BirateralKernel = data.BirateralKernel,
  //                  SigmaSpace = data.SigmaSpace,
  //                  SigmaColer = data.SigmaColer,
  //                  SearchLength = data.SearchLength,
  //                  SearchLine = data.SearchLine,
  //                  ChessCrossWidth = data.ChessCrossWidth,
  //                  ChessCrossHeigth = data.ChessCrossHeigth,
  //                  ChessSquareSize = data.ChessSquareSize,
  //                  OffsetX = data.OffsetX,
  //                  OffsetY = data.OffsetY
  //              }
  //          };
  //          return tmp;
  //      }
  //  }
}
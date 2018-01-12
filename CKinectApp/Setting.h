#ifndef SETTING_H
#define SETTING_H

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cereal/cereal.hpp>

//#include <cereal/archives/xml.hpp>

#define PRINT1(name) printer(#name, (name))
#define valName(x) #x

using std::cout;
using std::endl;
//using std::string;

#pragma pack(push, 4)
//計算条件の設定を格納する構造体
struct ImageProcessSetting
{
	//メンバは、C#側構造体の順番・アライメントと一致させること
	//また、メンバの追加・削除・修正の際は、下記4項目を連動させること
	//1. 構造体メンバ
	//2. Setting::hashIt()
	//3. Setting::setVal()
	//4. Setting::eValName 列挙体

	/// <summary>歪み補正をするか否か </summary>
	bool HasToUndistort;

	/// <summary>出力する画像の幅[pixel] </summary>
	int OutputWidth;

	/// <summary>出力する画像の高さ[pixel] </summary>
	int OutputHeight;

	/// <summary>二値化の閾値1 </summary>
	double Threshold1;

	/// <summary>二値化の閾値2 </summary>
	double Threshold2;

	/// <summary>極座標で表す場合の半径の分解能 </summary>
	double RhoResolution;

	/// <summary>極座標で表す場合の角度の分解能 </summary>
	double ThetaResolution;

	/// <summary>閾値 </summary>
	int Threshold; 

	/// <summary>これ未満の長さの線分は、線分とはみなさない </summary>
	double MinLineLength;

	/// <summary>この長さ以下ギャップがあっても、同一の線分とみなす </summary>
	double MaxLineGap;

	/// <summary>この長さだけ線分を伸長して交差した場合、「交差した」とみなす </summary>
	int Torelance;

	/// <summary>小さいほうの角度がこの角度未満で交差する場合、平行とみなして「交差しない」とみなす。 </summary>
	double LowerThetaLimit;

	/// <summary>矩形領域内にある点のみを抽出するか否か。する場合はTRUE。 </summary>
	bool HasToMask;

	/// <summary>矩形領域左上の点(x1,y1)のx座標[pixel] </summary>
	int X1;

	/// <summary>矩形領域左上の点(x1,y1)のy座標[pixel] </summary>
	int Y1;

	/// <summary>矩形領域の幅[pixel] </summary>
	int RecWidth;

	/// <summary>矩形領域の高さ[pixel] </summary>
	int RecHeight;


	/// <summary>ガウシアンフィルタのカーネルサイズ(x方向)[pixel]</summary>
	int KSizeX;

	/// <summary>ガウシアンフィルタのカーネルサイズ(y方向)[pixel]</summary>
	int KSizeY;

	/// <summary>ガウシアンフィルタのカーネルの標準偏差(x方向)[pixel]</summary>
	double SigmaX;

	/// <summary>ガウシアンフィルタのカーネルの標準偏差(y方向)[pixel]</summary>
	double SigmaY;

	/// <summary>2直線の(符号付き)角度の差の絶対値が slopeAngleResolution 未満であれば、2直線の傾きを等しいとみなす。単位は[deg]。</summary>
	double SlopeAngleResolution;

	/// <summary>2直線の(符号付き)y切片の差の絶対値が intersectResolution 未満であれば、2直線のy切片を等しいとみなす。単位は[pixel]。</summary>
	double IntersectResolution;

	//一枚目貼り付け時の直線検出用
	/// <summary> </summary>
	int Point1X;

	/// <summary> </summary>
	int Point1Y;

	/// <summary> </summary>
	int Point2X;

	/// <summary> </summary>
	int Point2Y;

	/// <summary>板　バージョンのレファレンスポイントX </summary>
	int RefCornerPointX;

	/// <summary>板　バージョンのレファレンスポイントY </summary>
	int RefCornerPointY;

	/// <summary>板　バージョンのリファレンス角度 </summary>
	double RefCornerAngle;

	/// <summary>板　バージョンのレファレンスポイントX2 </summary>
	int RefCornerPointX2;

	/// <summary>板　バージョンのレファレンスポイントY2 </summary>
	int RefCornerPointY2;

	/// <summary>バイラテラルフィルタ用の係数 </summary>
	int BirateralKernel;
	double SigmaSpace;
	double SigmaColor;

	/// <summary>コーナー検索用のサイズ </summary>
	int SearchLength;
	int SearchLine;

	/// <summary>チェスボード関係の設定</summary>
	int ChessCrossWidth;
	int CHessCrossHeight;
	double ChessSquareSize;
	int OffsetX;
	int OffsetY;


#pragma pack(pop)

	enum eValName
	{
		eInvalidValue = 0,
		eHasToUndistort,
		eOutputWidth,
		eOutputHeight,
		eThreshold1,
		eThreshold2,
		eRhoResolution,
		eThetaResolution,
		eThreshold,
		eMinLineLength,
		eMaxLineGap,
		eTorelance,
		eLowerThetaLimit,
		eHasToMask,
		eX1,
		eY1,
		eRecWidth,
		eRecHeight,
		eKSizeX,
		eKSizeY,
		eSigmaX,
		eSigmaY,
		eSlopeAngleResolution,
		eIntersectResolution,
		ePoint1X,
		ePoint1Y,
		ePoint2X,
		ePoint2Y,
		eRefCornerPointX,
		eRefCornerPointY,
		eRefCornerAngle,
		eRefCornerPointX2,
		eRefCornerPointY2,
		eBirateralKernel,
		eSigmaSpace,
		eSigmaColor,
		eSearchLength,
		eSearchLine,
		eChessCrossWidth,
		eChessCrossHeight,
		eChessSquareSize,
		eOffsetX,
		eOffsetY,
	};

	//文字列を受け取って、対応する eValName を返す。
	//C++では string に対して switch ができないため。
	eValName hashIt(std::string inString)
	{
		if (inString == "hasToUndistort")            return eValName::eHasToUndistort;
		else if (inString == "outputWidth")          return eValName::eOutputWidth;
		else if (inString == "outputHeight")         return eValName::eOutputHeight;
		else if (inString == "threshold1")           return eValName::eThreshold1;
		else if (inString == "threshold2")           return eValName::eThreshold2;
		else if (inString == "rhoResolution")        return eValName::eRhoResolution;
		else if (inString == "thetaResolution")      return eValName::eThetaResolution;
		else if (inString == "threshold")            return eValName::eThreshold;
		else if (inString == "minLineLength")        return eValName::eMinLineLength;
		else if (inString == "maxLineGap")           return eValName::eMaxLineGap;
		else if (inString == "torelance")            return eValName::eTorelance;
		else if (inString == "lowerThetaLimit")      return eValName::eLowerThetaLimit;
		else if (inString == "hasToMask")            return eValName::eHasToMask;
		else if (inString == "x1")                   return eValName::eX1;
		else if (inString == "y1")                   return eValName::eY1;
		else if (inString == "recWidth")             return eValName::eRecWidth;
		else if (inString == "recHeight")            return eValName::eRecHeight;
		else if (inString == "kSizeX")               return eValName::eKSizeX;
		else if (inString == "kSizeY")               return eValName::eKSizeY;
		else if (inString == "sigmaX")               return eValName::eSigmaX;
		else if (inString == "sigmaY")               return eValName::eSigmaY;
		else if (inString == "slopeAngleResolution") return eValName::eSlopeAngleResolution;
		else if (inString == "intersectResolution")  return eValName::eIntersectResolution;
		else if (inString == "Point1X")              return eValName::ePoint1X;
		else if (inString == "Point1Y")              return eValName::ePoint1Y;
		else if (inString == "Point2X")              return eValName::ePoint2X;
		else if (inString == "Point2Y")              return eValName::ePoint2Y;
		else if (inString == "RefCornerPointX")      return eValName::eRefCornerPointX;
		else if (inString == "RefCornerPointY")      return eValName::eRefCornerPointY;
		else if (inString == "RefCornerAngle")       return eValName::eRefCornerAngle;
		else if (inString == "RefCornerPointX2")     return eValName::eRefCornerPointX2;
		else if (inString == "RefCornerPointY2")     return eValName::eRefCornerPointY2;
		else if (inString == "BirateralKernel")      return eValName::eBirateralKernel;
		else if (inString == "sigmaSpace")           return eValName::eSigmaSpace;
		else if (inString == "sigmaColor")           return eValName::eSigmaColor;
		else if (inString == "searchLength")         return eValName::eSearchLength;
		else if (inString == "searchLine")           return eValName::eSearchLine;
		else if (inString == "ChessCrossWidth")      return eValName::eChessCrossWidth;
		else if (inString == "CHessCrossHeight")     return eValName::eChessCrossHeight;
		else if (inString == "ChessSquareSize")      return eValName::eChessSquareSize;
		else if (inString == "OffsetX")              return eValName::eOffsetX;
		else if (inString == "OffsetY")              return eValName::eOffsetY;
		else                                         return eValName::eInvalidValue;
	};

public:

	//設定の初期値を生成
	ImageProcessSetting()
	{
		HasToUndistort = false;
		OutputWidth = 1280;
		OutputHeight = 1024;
		Threshold1 = 8;
		Threshold2 = 28;
		RhoResolution = 1;
		ThetaResolution = 1 * M_PI / 180.0;
		Threshold = 78;
		MinLineLength = 94;
		MaxLineGap = 29;
		Torelance = 30;
		LowerThetaLimit = 30.0;
		HasToMask = true;
		X1 = 550;
		Y1 = 130;
		RecWidth = 500;
		RecHeight = 300;
		KSizeX = 9;
		KSizeY = 9;
		SigmaX = 0;
		SigmaY = 0;
		SlopeAngleResolution = 3.0;
		IntersectResolution = 4.0;
		Point1X = 0;
		Point1Y = 0;
		Point2X = 0;
		Point2Y = 0;
		RefCornerPointX = 0;
		RefCornerPointY = 0;
		RefCornerAngle = 0.0;
		RefCornerPointX2 = 0;
		RefCornerPointY2 = 0;
		BirateralKernel = 1;
		SigmaSpace = 1;
		SigmaColor = 1;
		SearchLength = 1;
		SearchLine = 1;
		ChessCrossWidth = 7;
		CHessCrossHeight = 11;
		ChessSquareSize = 24;
		OffsetX = 0;
		OffsetY = 0;
	};

	//構造体のメンバー名に、 対応する値 val をパースして代入する
	void setVal(std::string memberName, std::string val)
	{
		switch (hashIt(memberName))
		{
		case eValName::eHasToUndistort:
			//TODO: bool型にパースできるかを検査する。できなければ例外を投げる。
			HasToUndistort = String2<bool>(val);
			break;
		case eValName::eOutputWidth:
			OutputWidth = String2<int>(val);
			break;
		case eValName::eOutputHeight:
			OutputHeight = String2<int>(val);
			break;
		case eValName::eThreshold1:
			Threshold1 = String2<double>(val);
			break;
		case eValName::eThreshold2:
			Threshold2 = String2<double>(val);
			break;
		case eValName::eRhoResolution:
			RhoResolution = String2<double>(val);
			break;
		case eValName::eThetaResolution:
			ThetaResolution = String2<double>(val);
			break;
		case eValName::eThreshold:
			Threshold = String2<int>(val);
			break;
		case eValName::eMinLineLength:
			MinLineLength = String2<double>(val);
			break;
		case eValName::eMaxLineGap:
			MaxLineGap = String2<double>(val);
			break;
		case eValName::eTorelance:
			Torelance = String2<int>(val);
			break;
		case eValName::eLowerThetaLimit:
			LowerThetaLimit = String2<double>(val);
			break;
		case eValName::eHasToMask:
			HasToMask = String2<bool>(val);
			break;
		case eValName::eX1:
			X1 = String2<int>(val);
			break;
		case eValName::eY1:
			Y1 = String2<int>(val);
			break;
		case eValName::eRecWidth:
			RecWidth = String2<int>(val);
			break;
		case eValName::eRecHeight:
			RecHeight = String2<int>(val);
			break;
		case eValName::eKSizeX:
			KSizeX = String2<int>(val);
			break;
		case eValName::eKSizeY:
			KSizeY = String2<int>(val);
			break;
		case eValName::eSigmaX:
			SigmaX = String2<double>(val);
			break;
		case eValName::eSigmaY:
			SigmaY = String2<double>(val);
			break;
		case eSlopeAngleResolution:
			SlopeAngleResolution = String2<double>(val);
			break;
		case eIntersectResolution:
			IntersectResolution = String2<double>(val);
			break;
		case ePoint1X:
			Point1X = String2<int>(val);
			break;
		case ePoint1Y:
			Point1Y = String2<int>(val);
			break;
		case ePoint2X:
			Point2X = String2<int>(val);
			break;
		case ePoint2Y:
			Point2Y = String2<int>(val);
			break;
		case eRefCornerPointX:
			RefCornerPointX = String2<int>(val);
			break;
		case eRefCornerPointY:
			RefCornerPointY = String2<int>(val);
			break;
		case eRefCornerAngle:
			RefCornerAngle = String2<double>(val);
			break;
		case eRefCornerPointX2:
			RefCornerPointX2 = String2<int>(val);
			break;
		case eRefCornerPointY2:
			RefCornerPointY2 = String2<int>(val);
			break;
		case eBirateralKernel:
			BirateralKernel = String2<int>(val);
			break;
		case eSigmaSpace:
			SigmaSpace = String2<double>(val);
			break;
		case eSigmaColor:
			SigmaColor = String2<double>(val);
			break;
		case eSearchLength:
			SearchLength = String2<int>(val);
			break;
		case eSearchLine:
			SearchLine = String2<int>(val);
			break;
		case eChessCrossWidth:
			ChessCrossWidth = String2<int>(val);
			break;
		case eChessCrossHeight:
			CHessCrossHeight = String2<int>(val);
			break;
		case eChessSquareSize:
			ChessSquareSize = String2<double>(val);
			break;
		case eOffsetX:
			OffsetX = String2<int>(val);
			break;
		case eOffsetY:
			OffsetY = String2<int>(val);
			break;

		default:
			cout << "Error in Setting::setVal with argument: " << memberName << endl;
			break;
		}
	}

	//文字列から T 型へと変換するテンプレート
	template<typename T>
	T String2(const std::string& str)
	{
		std::istringstream ss(str);
		T ret;
		ss >> ret;
		return ret;
	}

	//受け取ったバイト配列から構造体を再構成する
	void ImageProcessSetting::deserialize(unsigned char* buffer)
	{
		int sepCountUpperLimit = 2;
		int sepCount = 0;
		unsigned char offset = 0;
		size_t elementLength = 0;
		unsigned char sep = ',';

		char tempCharArray[32];
		std::string tempString;

		std::string typeString;
		int structSize;

		while (sepCount < sepCountUpperLimit)
		{
			unsigned char c = buffer[offset + elementLength];
			if (c == sep)
			{
				memcpy(tempCharArray, buffer + offset, elementLength + 1);
				tempCharArray[elementLength] = '\0';
				tempString = std::string(tempCharArray);

				switch (sepCount)
				{
				case 0:
					//"StructAll"という文字列
					typeString = tempString;
					break;
				case 1:
					//構造体のサイズ
					structSize = std::stoi(tempString.c_str());
					break;
				}

				//管理用変数の更新
				offset += elementLength + 1;  //1はsepの分
				elementLength = 0;
				sepCount++;
			}
			else
			{
				//管理用変数の更新
				elementLength++;
			}
		}

		//構造体を格納
		memcpy(this, buffer + offset, sizeof(ImageProcessSetting));
	}

	//構造体の各要素の値をコンソール出力する
	void ImageProcessSetting::disp();

	//変数名とその値とを併記する
	void printer(char *name, int value) { printf("%s: %d\n", name, value); }

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(
			CEREAL_NVP(HasToUndistort),
			CEREAL_NVP(OutputWidth),
			CEREAL_NVP(OutputHeight),
			CEREAL_NVP(Threshold1),
			CEREAL_NVP(Threshold2),
			CEREAL_NVP(RhoResolution),
			CEREAL_NVP(ThetaResolution),
			CEREAL_NVP(Threshold),
			CEREAL_NVP(MinLineLength),
			CEREAL_NVP(MaxLineGap),
			CEREAL_NVP(Torelance),
			CEREAL_NVP(LowerThetaLimit),
			CEREAL_NVP(HasToMask),
			CEREAL_NVP(X1),
			CEREAL_NVP(Y1),
			CEREAL_NVP(RecWidth),
			CEREAL_NVP(RecHeight),
			CEREAL_NVP(KSizeX),
			CEREAL_NVP(KSizeY),
			CEREAL_NVP(SigmaX),
			CEREAL_NVP(SigmaY),
			CEREAL_NVP(SlopeAngleResolution),
			CEREAL_NVP(IntersectResolution),
			CEREAL_NVP(Point1X),
			CEREAL_NVP(Point1Y),
			CEREAL_NVP(Point2X),
			CEREAL_NVP(Point2Y),
			CEREAL_NVP(RefCornerPointX),
			CEREAL_NVP(RefCornerPointY),
			CEREAL_NVP(RefCornerAngle),
			CEREAL_NVP(RefCornerPointX2),
			CEREAL_NVP(RefCornerPointY2),
			CEREAL_NVP(BirateralKernel),
			CEREAL_NVP(SigmaSpace),
			CEREAL_NVP(SigmaColor),
			CEREAL_NVP(SearchLength),
			CEREAL_NVP(SearchLine),
			CEREAL_NVP(ChessCrossWidth),
			CEREAL_NVP(CHessCrossHeight),
			CEREAL_NVP(ChessSquareSize),
			CEREAL_NVP(OffsetX),
			CEREAL_NVP(OffsetY)
		);
	}
};

#endif // !SETTING_H

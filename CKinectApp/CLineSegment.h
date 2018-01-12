#ifndef CLINE_SEGMENT_H
#define CLINE_SEGMENT_H

#pragma warning(disable: 4819)
#include <opencv2/opencv.hpp>
#pragma warning(default: 4819)
#include <windows.h>
#include <debugapi.h>


///<summary>線分クラス</summary>
class CLineSegment
{
private:
	/// <summary>P1, P2 は定義済として、他のパラメータを計算する。</summary>
	void CalcParam();

public:
	/// <summary>線分(x1,y1,x2,y2)を格納したもの</summary>
	cv::Vec4i X1y1x2y2;

	/// <summary>点(x1,y1)</summary>
	cv::Point2i P1;

	/// <summary>点(x2,y2)</summary>
	cv::Point2i P2;

	/// <summary>P1,P2の中点座標 </summary>
	cv::Point2i Center;

	/// <summary>インデックス番号</summary>
	std::string Index;

	double LineLength;

	/// <summary>直線 y = ax + b の a(傾き)</summary>
	double Slope;

	/// <summary>直線 y = ax + b のb(y切片)</summary>
	double Intersect;

	/// <summary>tan(Theta) = Slope となる Theta[deg]</summary>
	double Theta;

	/// <summary>原点(0,0)からこの直線までの距離。</summary>
	double Distance;

	/// <summary>コンストラクタ</summary>
	CLineSegment();

	/// <summary>コンストラクタ。線分の座標値 ls = (x1,y1,x2,y2)を引数にとる。</summary>
	CLineSegment(cv::Vec4i ls);

	CLineSegment(cv::Vec4i ls, std::string index);

	/// <summary>デストラクタ</summary>
	~CLineSegment();
	
	/// <summary>線分の長さが一番長いものを返す</summary>
	static CLineSegment GetLongestFrom(const std::vector<CLineSegment> lineSegments);

	/// <summary>似た線分を1つにまとめて重複を除去する</summary>
	static std::vector<CLineSegment> UniqLines(std::vector<CLineSegment> lineSegments, double thetaResolution = 6, double intersectResolution = 10);

	/// <summary>以下の順に大小を比較できる時点でその真偽値を返す: Slope, Intersect, P1.x, P1.y, P2.x, P2.y</summary>
	bool operator <(CLineSegment ls);

	/// <summary> x軸(1,0)ベクトルとの角度が theta 未満である線分を抽出する</summary>
	static std::vector<CLineSegment> ExtractHorizontalLineSegments(std::vector<CLineSegment> extractedLineSegments, double theta);

	// / <summary>線分(x1,y1,x2,y2)のベクトルを CLineSegment のベクトルへと変換する</summary>
	static std::vector<CLineSegment> ConverFrom(std::vector<cv::Vec4i> x1y1x2y2List);

	/// <summary>線分の角度 Theta が、lowerLimitTheta 以上 upperLimitTheta 以下であれば true を返す。そうでなければ false を返す。</summary>
	bool hasThetaBetween(double lowerLimitTheta, double upperLimitTheta);

	/// <summary>線分の角度 Theta が、(theta-dTheta) 以上 (theta+dTheta) 以下である線分のベクトルを返す。</summary>
	static std::vector<CLineSegment> Extract(std::vector<CLineSegment> lineSegments, double theta, double dTheta = -1.0);

	/// <summary>線分を両側へ延長した直線と、点(x,y)との間の距離を返す</summary>
	double CalcDistanceFrom(cv::Point2i xy);

	/// <summary>指定点から近い直線を取得する</summary>
	static CLineSegment GetNearLine(const std::vector<CLineSegment> lines, cv::Vec2i point);

	/// <summary>線分の中点が一番下にある線分を返す。</summary>
	static CLineSegment GetLowest(const std::vector<CLineSegment> lines, cv::Vec2i point);

	/// <summary>直線と点の距離を計算する</summary>
	static double CalcPointLineDis(cv::Vec2i point, cv::Vec2i lineP1, cv::Vec2i lineP2);

	bool operator==(const CLineSegment & ls);

	/// <summary>point のx座標が 線分lsのx座標の範囲に入っている線分のうち、point との距離が最短となる線分を返す。一つも前述の条件を満たすものがない場合は、CLineSegment(-999999,-999999,999999,999999)を返す。</summary>
	static CLineSegment CLineSegment::GetNearLine(const std::vector<CLineSegment> lines, cv::Vec2i point, int& index);

	static CLineSegment GetNearLineTmp(const std::vector<CLineSegment> lines, cv::Vec2i point, int dir);

	/// <summary>線分の端点から、直線を計算 </summary>
	static CLineSegment CaleLine(cv::Point2i p1, cv::Point2i p2);

	/// <summary>似た線分を1つにまとめて重複を除去する</summary>
	static std::vector<CLineSegment> UniqLinesTmp(std::vector<CLineSegment> lineSegments, double thetaResolution = 6, double intersectResolution = 10);

	/// <summary>線分と線分が交差しているか確認する </summary>
	static bool IsCrossed(CLineSegment baseLine, CLineSegment targetLine);

	/// <summary>線分の2座標を受け取り、直線 ax+by+c=0 のパラメータ (a,b,c) を返す。</summary>
	cv::Point3d CalcAbc();
};

#endif // !CLINE_SEGMENT_H

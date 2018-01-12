#define _USE_MATH_DEFINES
#include <cmath>
#include "stdafx.h"
#include "CLineSegment.h"
#include "geo.h"
#include <algorithm>
#include <valarray>
#include "CUtil.h"

/// <summary>コンストラクタ</summary>
CLineSegment::CLineSegment()
{
	P1 = cv::Point2d(0, 0);
	P2 = cv::Point2d(1, 0);
	CalcParam();
}

/// <summary>コンストラクタ。線分の座標値ls=(x1,y1,x2,y2)を引数にとる。</summary>
CLineSegment::CLineSegment(cv::Vec4i ls)
{
	P1 = cv::Point2d(ls[0], ls[1]);
	P2 = cv::Point2d(ls[2], ls[3]);

	//x座標が小さい順にP1, P2
	if( P2.x < P1.x)
	{
		auto temp = P1;
		P1 = P2;
		P2 = temp;
	}
	else if(P1.x==P2.x && P2.y<P1.y)
	{
		//P1,P2のx座標が同じ場合は、y座標が小さい順にP1,P2とする
		auto temp = P1;
		P1 = P2;
		P2 = temp;
	}

	CalcParam();
}

CLineSegment::CLineSegment(cv::Vec4i ls, std::string index)
{
	Index = index;
	//CLineSegment{ ls };
	P1 = cv::Point2d(ls[0], ls[1]);
	P2 = cv::Point2d(ls[2], ls[3]);

	//x座標が小さい順にP1, P2
	if (P2.x < P1.x)
	{
		auto temp = P1;
		P1 = P2;
		P2 = temp;
	}
	else if (P1.x == P2.x && P2.y<P1.y)
	{
		//P1,P2のx座標が同じ場合は、y座標が小さい順にP1,P2とする
		auto temp = P1;
		P1 = P2;
		P2 = temp;
	}

	CalcParam();
}

/// <summary>デストラクタ</summary>
CLineSegment::~CLineSegment()
{
}

CLineSegment CLineSegment::GetLongestFrom(const std::vector<CLineSegment> lineSegments)
{
	std::vector<double> lengthList;
	for(auto ls: lineSegments)
	{
		lengthList.push_back(ls.LineLength);
	}

	size_t maxIndex = std::distance(lengthList.begin(), std::max_element(lengthList.begin(), lengthList.end()));
	return lineSegments.at(maxIndex);
}

/// <summary>以下の順に大小を比較できる時点でその真偽値を返す: Slope, Intersect, P1.x, P1.y, P2.x, P2.y</summary>
bool CLineSegment::operator<(CLineSegment ls)
{
	if (Slope != ls.Slope) return Slope < ls.Slope;
	if (Intersect != ls.Intersect) return Intersect < ls.Intersect;
	if (P1.x != ls.P1.x) return P1.x < ls.P1.x;
	if (P1.y != ls.P1.y) return P1.y < ls.P1.y;
	if (P2.x != ls.P2.x) return P2.x < ls.P2.x;
	return P2.x < ls.P1.x;
}


/// <summary>P1, P2 は定義済として、他のパラメータを計算する。</summary>
void CLineSegment::CalcParam()
{
	//P1, P2 は定義済として、その他のプロパティを計算する
	X1y1x2y2 = cv::Vec4i(P1.x, P1.y, P2.x, P2.y);
	Slope = double(P2.y - P1.y) / double(P2.x - P1.x);
	Intersect = double(P2.x*P1.y - P1.x*P2.y) / double(P2.x - P1.x);
	Theta = atan(Slope) * 180.0 / M_PI;// RAD2DEG;

	Distance = CalcDistanceFrom(cv::Point(0, 0));

	//中点の座標を計算
	Center.x = (int)((P1.x + P2.x) / 2.0);
	Center.y = (int)((P1.y + P2.y) / 2.0);

	LineLength = std::sqrt(std::pow(P1.x - P2.x, 2) + std::pow(P1.y - P2.y, 2));

}

/// <summary>線分の端点から、直線を計算 </summary>
/// <param name="p1">端点１</param>
/// <param name="p2">端点２</param>
/// <returns></returns>
CLineSegment CLineSegment::CaleLine(cv::Point2i p1, cv::Point2i p2)
{
	auto tmpPoints = cv::Vec4i(p1.x, p1.y, p2.x, p2.y);
	auto tmpLine = CLineSegment{ tmpPoints };

	return tmpLine;
}

/// <summary>似た線分を1つにまとめて重複を除去する</summary>
/// <param name="lineSegments">重複を除去する前の CLineSegments のベクトル</param>
/// <param name="thetaResolution">x軸から線分の角度(theta[deg])の差がこの値未満であれば、同一の直線とみなす</param>
/// <param name="intersectResolution">y切片(Intersect)の差がこの値未満であれば、同一の直線とみなす</param>
/// <returns>重複を取り除いた CLineSegment のベクトル</returns>
std::vector<CLineSegment> CLineSegment::UniqLines(std::vector<CLineSegment> lineSegments, double thetaResolution, double intersectResolution)
{
	//デバッグウィンドウで確認する場合には、ソートしたほうが見やすい
	//sort(lineSegments.begin(), lineSegments.end());

	//似た傾き・切片を持っていれば、一つの線分にまとめる
	auto ls1 = lineSegments.begin();
	while (ls1 != lineSegments.end())
	{
		auto ls2 = ls1 + 1;
		while (ls2 != lineSegments.end())
		{
			//ほぼ同じ直線なら
			if (abs((*ls1).Theta - (*ls2).Theta) < thetaResolution &&
				abs((*ls1).Distance - (*ls2).Distance) < intersectResolution)
			{
				//直線ls1, ls2を統一

				std::vector<int> xLst = { (*ls1).P1.x, (*ls1).P2.x, (*ls2).P1.x, (*ls2).P2.x };
				std::vector<int> yLst = { (*ls1).P1.y, (*ls1).P2.y, (*ls2).P1.y, (*ls2).P2.y };

				size_t minIndex = std::distance(xLst.begin(), std::min_element(xLst.begin(), xLst.end()));
				auto x1 = xLst[minIndex];
				auto y1 = yLst[minIndex];

				size_t maxIndex = std::distance(xLst.begin(), std::max_element(xLst.begin(), xLst.end()));
				auto x2 = xLst[maxIndex];
				auto y2 = yLst[maxIndex];

				auto ls1Label = (*ls1).Index;
				auto ls2Label = (*ls2).Index;

				//線分*ls1に統合した新たな線分を上書き、ls2は除去
				*ls1 = CLineSegment(cv::Vec4d(x1, y1, x2, y2));  //ls1にlを上書き
				(*ls1).Index = std::string("[") + ls1Label + "-" + ls2Label + "]";
				ls2 = lineSegments.erase(ls2);                   //ls2は消去
			}
			else
			{
				++ls2;
			}
		}

		++ls1;
	}

	return lineSegments;
}

/// <summary>似た線分を1つにまとめて重複を除去する</summary>
/// <param name="lineSegments">重複を除去する前の CLineSegments のベクトル</param>
/// <param name="thetaResolution">x軸から線分の角度(theta[deg])の差がこの値未満であれば、同一の直線とみなす</param>
/// <param name="intersectResolution">y切片(Intersect)の差がこの値未満であれば、同一の直線とみなす</param>
/// <returns>重複を取り除いた CLineSegment のベクトル</returns>
std::vector<CLineSegment> CLineSegment::UniqLinesTmp(std::vector<CLineSegment> lineSegments, double thetaResolution, double intersectResolution)
{
	//デバッグウィンドウで確認する場合には、ソートしたほうが見やすい
	//sort(lineSegments.begin(), lineSegments.end());

	//似た傾き・切片を持っていれば、一つの線分にまとめる
	auto ls1 = lineSegments.begin();
	while (ls1 != lineSegments.end())
	{
		auto ls2 = ls1 + 1;
		while (ls2 != lineSegments.end())
		{
			auto p1Center = cv::Point2i((*ls1).Center);
			auto p2Center = cv::Point2i((*ls2).Center);

			auto tmpLine = CLineSegment::CaleLine(p1Center, p2Center);
			auto ls1Theta = (*ls1).Theta;
			auto ls2Theta = (*ls2).Theta;
			auto tmpLineTheta = tmpLine.Theta;

			//if(abs(ls1Theta - tmpLineTheta) < thetaResolution && abs(ls2Theta - tmpLineTheta) < thetaResolution)
			if (abs(ls1Theta - ls2Theta) < thetaResolution && abs(tmpLine.LineLength) < intersectResolution)
			{
				//直線ls1, ls2を統一

				std::vector<int> xLst = { (*ls1).P1.x, (*ls1).P2.x, (*ls2).P1.x, (*ls2).P2.x };
				std::vector<int> yLst = { (*ls1).P1.y, (*ls1).P2.y, (*ls2).P1.y, (*ls2).P2.y };

				size_t minIndex = std::distance(xLst.begin(), std::min_element(xLst.begin(), xLst.end()));
				auto x1 = xLst[minIndex];
				auto y1 = yLst[minIndex];

				size_t maxIndex = std::distance(xLst.begin(), std::max_element(xLst.begin(), xLst.end()));
				auto x2 = xLst[maxIndex];
				auto y2 = yLst[maxIndex];

				//線分*ls1に統合した新たな線分を上書き、ls2は除去
				auto margeStr = std::string((*ls1).Index + "-" + (*ls2).Index);
				*ls1 = CLineSegment(cv::Vec4d(x1, y1, x2, y2),margeStr);  //ls1にlを上書き
				ls2 = lineSegments.erase(ls2);                   //ls2は消去
			}

			////ほぼ同じ直線なら
			//if (abs((*ls1).Theta - (*ls2).Theta) < thetaResolution &&
			//	abs((*ls1).Intersect - (*ls2).Intersect) < intersectResolution)
			//{
			//	//直線ls1, ls2を統一

			//	std::vector<int> xLst = { (*ls1).P1.x, (*ls1).P2.x, (*ls2).P1.x, (*ls2).P2.x };
			//	std::vector<int> yLst = { (*ls1).P1.y, (*ls1).P2.y, (*ls2).P1.y, (*ls2).P2.y };

			//	size_t minIndex = std::distance(xLst.begin(), std::min_element(xLst.begin(), xLst.end()));
			//	auto x1 = xLst[minIndex];
			//	auto y1 = yLst[minIndex];

			//	size_t maxIndex = std::distance(xLst.begin(), std::max_element(xLst.begin(), xLst.end()));
			//	auto x2 = xLst[maxIndex];
			//	auto y2 = yLst[maxIndex];

			//	//線分*ls1に統合した新たな線分を上書き、ls2は除去
			//	*ls1 = CLineSegment(cv::Vec4d(x1, y1, x2, y2));  //ls1にlを上書き
			//	ls2 = lineSegments.erase(ls2);                   //ls2は消去
			//}
			else
			{
				++ls2;
			}
		}

		++ls1;
	}

	return lineSegments;
}


/// <summary>x軸(1,0)ベクトルとの角度が ±theta[deg] 未満である線分を抽出する</summary>
/// <param name="extractedLineSegments"></param>
/// <param name="theta"></param>
/// <returns></returns>
std::vector<CLineSegment> CLineSegment::ExtractHorizontalLineSegments(std::vector<CLineSegment> extractedLineSegments, double theta)
{
	std::vector<CLineSegment> horizontalLineSegments;
	auto theta2 = 180.0 - theta;

	for (auto ls : extractedLineSegments)
	{
		if (ls.Theta < theta || theta2 < ls.Theta)
		{
			horizontalLineSegments.push_back(ls);
		}
	}

	return horizontalLineSegments;
}

/// <summary>線分(x1,y1,x2,y2)のベクトルを CLineSegment のベクトルへと変換する</summary>
std::vector<CLineSegment> CLineSegment::ConverFrom(std::vector<cv::Vec4i> x1y1x2y2List)
{
	std::vector<CLineSegment> ans;

	int index = 0;
	for (auto x1y1x2y2 : x1y1x2y2List)
	{
		ans.push_back(CLineSegment(x1y1x2y2, std::to_string(index++)));
	}

	return ans;
}

/// <summary>線分の角度 Theta が、lowerLimitTheta以上upperLimitTheta以下であればtrueを返す。そうでなければfalseを返す。</summary>
bool CLineSegment::hasThetaBetween(double lowerLimitTheta, double upperLimitTheta)
{
	if (Theta < lowerLimitTheta) return false;
	if (upperLimitTheta < Theta) return false;
	return true;
}

/// <summary>線分の角度Theta が、(theta-dTheta) 以上 (theta+dTheta) 以下である線分のベクトルを返す。</summary>
/// <param name="lineSegments">抽出前の線分のベクトル</param>
/// <param name="theta">抽出したい際の基準となる角度</param>
/// <param name="dTheta">角度に対する誤差。0以上の数を入力すること。-1を指定した場合は、 dTheta = 0.1*abs(theta) となる。</param>
std::vector<CLineSegment> CLineSegment::Extract(std::vector<CLineSegment> lineSegments, double theta, double dTheta)
{
	//dTheta が省略されていた場合の値を設定
	if (dTheta == -1.0) dTheta = 0.1*theta;

	//dThetaの符号を処理
	dTheta = abs(dTheta);

	//dThetaの最大値を設定
	if (90.0 < dTheta) dTheta = 90.0;

	//「dThetaは正数」として範囲を計算する
	double lowerLimitTheta = theta - dTheta;
	double upperLimitTheta = theta + dTheta;

	//角度が範囲[-90, 90]を超えた場合の変換
	double lowerLimitTheta1, upperLimitTheta1, lowerLimitTheta2, upperLimitTheta2;

	//条件に合う線分だけをansに追加する
	std::vector<CLineSegment> ans;

	// 場合分け 角度の範囲: lowerLimitTheta < -90 < +90 < upplerLimitTheta のとき
	if(lowerLimitTheta < -90.0 && +90 < upperLimitTheta)
	{
		//問答無用で全部条件を満たす
		ans.insert(ans.end(), lineSegments.begin(), lineSegments.end());
		return ans;
	}
	else if(lowerLimitTheta < -90.0)
	{
		// 場合分け 角度の範囲:  lowerLimitTheta < -90 < upperLimitTheta < +90
		lowerLimitTheta1 = 180.0 + lowerLimitTheta;
		upperLimitTheta1 = 180.0 + (-90.0);
		auto angularCenter = 0.5*(lowerLimitTheta1 + upperLimitTheta1);
		auto angularError = upperLimitTheta1 - angularCenter;
		auto res = Extract(lineSegments, angularCenter, angularError);
		ans.insert(ans.end(), res.begin(), res.end());
	}
	else if (90.0 < upperLimitTheta)
	{
		// 場合分け 角度の範囲:  -90  < lowerLimitTheta < + 90 < upperLimitTheta
		lowerLimitTheta2 = -180.0 + (+90);
		upperLimitTheta2 = -180.0 + upperLimitTheta;
		auto angularCenter = 0.5*(lowerLimitTheta2 + upperLimitTheta2);
		auto angularError = upperLimitTheta2 - angularCenter;
		auto res = Extract(lineSegments, angularCenter, angularError);
		ans.insert(ans.end(), res.begin(), res.end());
	}

	// 場合分け 角度の範囲:  -90  < lowerLimitTheta < upperLimitTheta < + 90
	for (auto ls : lineSegments)
	{
		if (ls.hasThetaBetween(lowerLimitTheta, upperLimitTheta))
		{
			ans.push_back(ls);
		}
	}

	return ans;
}

/// <summary>線分を両側へ延長した直線と、点(x,y)との間の距離を返す</summary>
double CLineSegment::CalcDistanceFrom(cv::Point2i xy)
{
	double x0 = double(xy.x);
	double y0 = double(xy.y);
	double a = Slope;
	double b = -1.0;
	double c = Intersect;

	double d = abs(a*x0 + b*y0 + c) / sqrt(a*a + b*b);
	return d;
}

/// <summary>指定点から近い直線を取得する</summary>
/// <param name="lines">検索対象の直線群</param>
/// <param name="point">指定点</param>
/// <returns></returns>
CLineSegment CLineSegment::GetNearLine(const std::vector<CLineSegment> lines, cv::Vec2i point)
{
	//pointとlinesの距離を全て計算する
	std::vector<double> lengts;

	for (auto line : lines)
	{
		auto length = abs(CalcPointLineDis(point, line.P1, line.P2)); //向きには興味ないので
		lengts.push_back(length);
	}

	auto minIT = std::min_element(lengts.begin(), lengts.end());
	auto minIndex = std::distance(lengts.begin(), minIT);

	return lines[minIndex];
}

CLineSegment CLineSegment::GetLowest(const std::vector<CLineSegment> lines, cv::Vec2i point)
{
	std::vector<int> centersY;
	for(auto i = 0; i<lines.size(); i++)
	{
		centersY.push_back(lines[i].Center.y);
	}

	std::vector<int>::iterator maxIt = std::max_element(centersY.begin(), centersY.end());
	size_t maxIndex = std::distance(centersY.begin(), maxIt);
	return lines.at(maxIndex);
}

/// <summary>指定点から近い直線を取得する</summary>
/// <param name="lines">検索対象の直線群</param>
/// <param name="point">指定点(リファレンス点)</param>
/// <param name="dir">検索方向 1:水平方向 2:垂直方向</param>
/// <returns></returns>
CLineSegment CLineSegment::GetNearLineTmp(const std::vector<CLineSegment> lines, cv::Vec2i point,int dir)
{
	//pointの垂直方向（Y軸方向）に交わる線があるか調べる
	{
		auto lineVert = cv::Vec4i(point[0], 0, point[0], 1000);
		auto dirY = CLineSegment(lineVert);
		std::vector<CLineSegment> tmpSeg;
		auto tmpY = 1000;
		auto tmpX = 1000;
		auto minIndex = -1;

		if (dir == 1)
		{
			for (auto i = 0; i < lines.size(); i++)
			{
				auto line = lines[i];

				auto flg = IsCrossed(dirY, line);
				if (flg == true)
				{
					std::stringstream ss;
					ss << "IsCrossed Detect " << "dir =" << dir << " line num =" << line.Index << std::endl;
					OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

					auto index = i;
					//水平方向検査 Ref1
					auto minY = std::min<int>(abs(point[1] - line.P1.y), abs(point[1] - line.P2.y));
					if (tmpY > minY)
					{
						tmpY = minY;
						minIndex = index;
					}
					tmpSeg.push_back(lines[minIndex]);
				}
			}
		}

		if(minIndex != -1)	return lines[minIndex];
	}

	{
		auto lineHorizon = cv::Vec4i(0, point[1], 1000, point[1]);
		auto dirX = CLineSegment(lineHorizon);
		std::vector<CLineSegment> tmpSeg;
		auto tmpY = 1000;
		auto tmpX = 1000;
		auto minIndex = -1;

		if (dir == 2)
		{
			for (auto i = 0; i < lines.size(); i++)
			{
				auto line = lines[i];

				if (IsCrossed(dirX, line) == true)
				{
					std::stringstream ss;
					ss << "IsCrossed Detect " << "dir =" << dir << " line num =" << line.Index << std::endl;
					OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

					auto index = i;

					//垂直方向検査 Ref1
					auto minX = std::min<int>(abs(point[0] - line.Center.x), abs(point[0] - line.Center.x));
					if (tmpX > minX)
					{
						tmpX = minX;
						minIndex = index;
					}

					tmpSeg.push_back(lines[minIndex]);
				}
			}
		}

		if (minIndex != -1)	return lines[minIndex];
	}

	//pointとlinesの距離を全て計算する
	std::vector<double> lengts;

	for (auto line : lines)
	{
		//auto length = abs(CalcPointLineDis(point, line.P1, line.P2)); //向きには興味ないので
		//auto p1p2Length = sqrt((line.P1.x*line.P1.x)+(line.P1.y*line.P1.y));
		auto p1p2Length = line.LineLength;
		auto p1p2PointLength = sqrt((point[0] - line.P1.x)*(point[0] - line.P1.x) + (point[1] - line.P1.y)*(point[1] - line.P1.y)) +
			sqrt((point[0] - line.P2.x)*(point[0] - line.P2.x) + (point[1] - line.P2.y)*(point[1] - line.P2.y));
		
		auto length = std::pow(p1p2PointLength,1) / std::pow(p1p2Length,1);

		lengts.push_back(length);
	}

	auto minIT = std::min_element(lengts.begin(), lengts.end());
	auto minIndex = std::distance(lengts.begin(), minIT);	//最小値のインデックス

	//pointから計算された最小値Lineの中点を結ぶ直線状に他の線がないか検査する
	
	//pointと上記検索結果のClinesegmentの中点を結ぶ線分
	auto tmpDetectLine = CLineSegment{ cv::Vec4i(point[0],point[1],lines[minIndex].Center.x,lines[minIndex].Center.y) };

	auto tmpLength = 2000.0;
	auto tmpX = std::min<int>(abs(point[0] - lines[minIndex].Center.x), abs(point[0] - lines[minIndex].Center.x));;
	auto tmpY = std::min<int>(abs(point[1] - lines[minIndex].P1.y), abs(point[1] - lines[minIndex].P2.y));;
	std::vector<CLineSegment> tmpSeg;
	for(auto i=0;i<lines.size();i++)
	{
		auto line = lines[i];
		if(tmpDetectLine == line) continue;

		if(IsCrossed(tmpDetectLine,line) == true)
		{
			//if(dir == 1)
			//{
			//	//水平方向
			//	auto P1x = 
			//}
			std::stringstream ss;
			ss << "IsCrossed Detect " <<"dir =" << dir << " line num =" << line.Index << std::endl;
			OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

			//auto result = std::find_if(lines.begin(), lines.end(), [&line](const CLineSegment& cl) {
			//	if (line == cl) return false;
			//	else return true;
			//});
			//auto index = std::distance(lines.begin(), result);
			auto index = i;
			if(dir==1)
			{
				//水平方向検査 Ref1
				auto minY = std::min<int>(abs(point[1]-line.P1.y), abs(point[1] - line.P2.y));
				if(tmpY > minY)
				{
					tmpY = minY;
					minIndex = index;
				}
			}
			else if(dir==2)
			{
				//垂直方向検査 Ref1
				auto minX = std::min<int>(abs(point[0] - line.Center.x), abs(point[0] - line.Center.x));
				if(tmpX > minX)
				{
					tmpX = minX;
					minIndex = index;
				}
			}

			tmpSeg.push_back(lines[minIndex]);


		}
	}

	if(tmpSeg.size() !=0)
	{
		if(dir == 1)
		{
			//水平方向

		}
	}

	return lines[minIndex];
}

/// <summary>線分と線分が交差しているか確認する</summary>
/// <param name="baseLine"></param>
/// <param name="targetLine"></param>
/// <returns></returns>
bool CLineSegment::IsCrossed(CLineSegment baseLine, CLineSegment targetLine)
{
	auto ax = baseLine.P1.x;
	auto ay = baseLine.P1.y;
	auto bx = baseLine.P2.x;
	auto by = baseLine.P2.y;

	auto cx = targetLine.P1.x;
	auto cy = targetLine.P1.y;
	auto dx = targetLine.P2.x;
	auto dy = targetLine.P2.y;

	long long ta = (cx - dx) * (ay - cy) + (cy - dy) * (cx - ax);
	long long tb = (cx - dx) * (by - cy) + (cy - dy) * (cx - bx);
	long long tc = (ax - bx) * (cy - ay) + (ay - by) * (ax - cx);
	long long td = (ax - bx) * (dy - ay) + (ay - by) * (ax - dx);

	auto flg= tc * td <= 0 && ta * tb <= 0;
	return flg;

	//auto p1x = baseLine.P1.x;
	//auto p1y = baseLine.P1.y;
	//auto p2x = baseLine.P2.x;
	//auto p2y = baseLine.P2.y;

	//auto p3x = targetLine.P1.x;
	//auto p3y = targetLine.P1.y;
	//auto p4x = targetLine.P2.x;
	//auto p4y = targetLine.P2.y;

	//if (((p1x - p2x) * (p3y - p1y) + (p1y - p2y) * (p1x - p3x))
	//	* ((p1x - p2x) * (p4y - p1y) + (p1y - p2y) * (p1x - p4x)) < 0) {

	//	if (((p3x - p4x) * (p1y - p3y) + (p3y - p4y) * (p3x - p1x))
	//		* ((p3x - p4x) * (p2y - p3y) + (p3y - p4y) * (p3x - p2x)) < 0) {
	//		//交差したら１を返す。				
	//		return true;
	//	}
	//}
	////交差していなかったら0を返す。
	//else return false;

}

/// <summary>point のx座標が 線分lsのx座標の範囲に入っている線分のうち、point との距離が最短となる線分を返す。一つも前述の条件を満たすものがない場合は、CLineSegment(-999999,-999999,999999,999999)を返す。</summary>
/// <param name="lines"></param>
/// <param name="point"></param>
/// <param name="iter"></param>
/// <returns></returns>
CLineSegment CLineSegment::GetNearLine(const std::vector<CLineSegment> lines, cv::Vec2i point, int& index)
{

	//pointとlinesの距離を全て計算する

	std::vector<CLineSegment> lsVec;  //条件を満たす線分のベクトル
	std::vector<double> lengths;      //条件を満たした線分から point までの距離のベクトル

	for (auto line : lines)
	{
		//以下の2条件のどちらかが成立するなら、返す線分の候補にいれる。
		// 1. line.P1.x ≦ point[0] ≦ line.P2.x
		// 1. line.P2.x ≦ point[0] ≦ line.P1.x
		if ((line.P1.x - point[0]) * (line.P2.x- point[0]) <= 0)
		{
			auto length = abs(CalcPointLineDis(point, line.P1, line.P2)); //向きには興味ないので
			lengths.push_back(length);
			lsVec.push_back(line);
		}
	}


	//返り値
	CLineSegment ans;

	//条件を満たす線分が1つ以上存在する場合は、距離が最小値となる線分を返す
	if (lengths.size() == 0)
	{
		//見つからなかった時
		index = -1;
		ans = (cv::Vec4i(-999999, -999999, +999999, 999999)); //こんなに大きなサイズの画像を操作しないでしょう
	}
	else
	{
		//1つ以上見つかった時
		auto minIT = std::min_element(lengths.begin(), lengths.end());
		auto minIndex = std::distance(lengths.begin(), minIT);

		//TODO index を lines に対するものに修正すること。現在は、lengths に対するものになっている。
		auto tmpIter = lines.begin() + minIndex;
		index = minIndex;
		ans = lsVec[minIndex];
	}

	return ans;
}

/// <summary>直線と点の距離を計算する</summary>
/// <param name="point">点(x,y)</param>
/// <param name="lineP1">直線の端点p1(p1x,p1y)</param>
/// <param name="lineP2">直線の端点p2(p2x,p2y</param>
/// <returns></returns>
double CLineSegment::CalcPointLineDis(cv::Vec2i point, cv::Vec2i lineP1, cv::Vec2i lineP2)
{
	auto p1p = point - lineP1;
	auto p2p = point - lineP2;

	//auto area = p1p.cross(p2p);

	auto area = p1p[0] * p2p[1] - p1p[1] * p2p[0]; //p1pとp2pで形成する平行四辺形の面積
	auto p12Length = norm(lineP2 - lineP1);	//p12の長さ
	auto length = area / p12Length;

	return length;
}

bool CLineSegment::operator==(const CLineSegment& ls){
	return (P1 == ls.P1 && P2 == ls.P2);


}

/// <summary>線分の2座標を受け取り、直線 ax+by+c=0 のパラメータ (a,b,c) を返す。</summary>
/// <returns>ax+by+c=0 の(a,b,c)</returns>
cv::Point3d CLineSegment::CalcAbc()
{
	auto a = +(P2.y - P1.y);
	auto b = -(P2.x - P1.x);
	auto c = P2.y*(P2.x - P1.x) - P2.x*(P2.y - P1.y);
	cv::Vec3d ans(a, b, c);
	return ans;
}
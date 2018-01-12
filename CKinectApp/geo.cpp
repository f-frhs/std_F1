#include "stdafx.h"
#include "geo.h"

#pragma warning(disable: 4819)
#include <opencv2/opencv.hpp>
#pragma warning(default: 4819)

#define PI 3.14159265

/// <summary>2次元平面上の位置関係を計算するライブラリクラス </summary>

geo::geo()
{
}

geo::~geo()
{
}

/// <summary>コーナーを検出し、そのxy座標のリストを返す </summary>
/// <param name="vec4iList">線分(x1,y1,x2,y2)のリスト</param>
/// <param name="tolerance">線分を tolerance ピクセルだけ伸長しても交点に達しないものは、交点を角とみなさない</param>
/// <returns>コーナーのxy座標を格納したVector2d のリスト</returns>
/// <summary>
std::vector<cv::Vec2i> geo::DetectCorners(std::vector<CLineSegment> lsList, int tolerance, double lowerThetaLimit)
{
	auto xyList = std::vector<cv::Vec2i>();

	for (auto i = 0; i < lsList.size(); i++)
	{
		for (auto j = i + 1; j < lsList.size(); j++)
		{
			//同一の線分の場合は、処理しない
			if (lsList[i] == lsList[j]) continue;

			cv::Vec2i xy;
			if (geo::isIntersected(lsList[i].X1y1x2y2, lsList[j].X1y1x2y2, tolerance, lowerThetaLimit, xy))
			{
				//printf("%04.0f %04.0f\n", xy(0), xy(1));
				xyList.push_back(xy);
			}
		}
	}
	return xyList;
}

/// <summary>2つの線分 ls1, ls2 が交差しているか否かを返す。また、交点のxy座標をベクトル xy に格納する。</summary>
/// <param name="ls1">線分1</param>
/// <param name="ls2">線分2</param>
/// <param name="tolerance">最大 tolerance ピクセル線分を伸長すると交点に達する場合は、「交差した」とみなす</param>
/// <param name="xy">交点の座標を格納したxy成分を持つ2次元ベクトル</param>
/// <remark>http://mf-atelier.sakura.ne.jp/mf-atelier/modules/tips/program/algorithm/a1.html</remark>
/// <returns>(線分を torelance ピクセルだけ伸長したら、)交差するか否か。交差する場合はtrue。</returns>
bool geo::isIntersected(cv::Vec4i ls1, cv::Vec4i ls2, int tolerance, double lowerThetaLimit, cv::Vec2i& xy)
{
	cv::Point3i p1(ls1[0], ls1[1], 0);
	cv::Point3i p2(ls1[2], ls1[3], 0);
	cv::Point3i p3(ls2[0], ls2[1], 0);
	cv::Point3i p4(ls2[2], ls2[3], 0);

	auto p12 = p2 - p1;
	auto p34 = p4 - p3;
	auto p12NormP34Norm = cv::norm(p12)*cv::norm(p34);

	//2ベクトル間の角度が一定以下のものは除外する。(=平行に近いとみなす -> 角ではない)
	auto sineLowerLimit = sin(lowerThetaLimit * PI / 180.0);
	auto det = abs(p12.cross(p34).z);
	if (det / p12NormP34Norm < sineLowerLimit) return false;

	//交点の座標を求める
	//交点を媒介変数s,tを使って表現する

	auto xi = (p4.y - p3.y)*(p4.x - p1.x) - (p4.x - p3.x)*(p4.y - p1.y);
	auto eta = (p2.x - p1.x)*(p4.y - p1.y) - (p2.y - p1.y)*(p4.x - p1.x);
	auto delta = (p4.y - p3.y)*(p2.x - p1.x) - (p4.x - p3.x)*(p2.y - p1.y);

	auto s = double(xi) / delta;
	auto t = double(eta) / delta;

	auto deltaS = double(tolerance) / cv::norm(p12);
	auto deltaT = double(tolerance) / cv::norm(p34);

	//(あともう少しで交差しそうな場合も含めた)交点の座標を求める
	if (-deltaS <= s && s <= (1 + deltaS) &&
		-deltaT <= t && t <= (1 + deltaT))
	{
		auto m = (1 - s)*p1 + s*p2;  // m= t*p3 + (1-t)*p4 //でもある
		xy = cv::Point2i(round(m.x), round(m.y));
		return true;
	}

	//設定した範囲外に交点があった場合は、「交差しなかった」とみなす
	return false;
}
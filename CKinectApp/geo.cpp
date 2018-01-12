#include "stdafx.h"
#include "geo.h"

#pragma warning(disable: 4819)
#include <opencv2/opencv.hpp>
#pragma warning(default: 4819)

#define PI 3.14159265

/// <summary>2�������ʏ�̈ʒu�֌W���v�Z���郉�C�u�����N���X </summary>

geo::geo()
{
}

geo::~geo()
{
}

/// <summary>�R�[�i�[�����o���A����xy���W�̃��X�g��Ԃ� </summary>
/// <param name="vec4iList">����(x1,y1,x2,y2)�̃��X�g</param>
/// <param name="tolerance">������ tolerance �s�N�Z�������L�����Ă���_�ɒB���Ȃ����̂́A��_���p�Ƃ݂Ȃ��Ȃ�</param>
/// <returns>�R�[�i�[��xy���W���i�[����Vector2d �̃��X�g</returns>
/// <summary>
std::vector<cv::Vec2i> geo::DetectCorners(std::vector<CLineSegment> lsList, int tolerance, double lowerThetaLimit)
{
	auto xyList = std::vector<cv::Vec2i>();

	for (auto i = 0; i < lsList.size(); i++)
	{
		for (auto j = i + 1; j < lsList.size(); j++)
		{
			//����̐����̏ꍇ�́A�������Ȃ�
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

/// <summary>2�̐��� ls1, ls2 ���������Ă��邩�ۂ���Ԃ��B�܂��A��_��xy���W���x�N�g�� xy �Ɋi�[����B</summary>
/// <param name="ls1">����1</param>
/// <param name="ls2">����2</param>
/// <param name="tolerance">�ő� tolerance �s�N�Z��������L������ƌ�_�ɒB����ꍇ�́A�u���������v�Ƃ݂Ȃ�</param>
/// <param name="xy">��_�̍��W���i�[����xy����������2�����x�N�g��</param>
/// <remark>http://mf-atelier.sakura.ne.jp/mf-atelier/modules/tips/program/algorithm/a1.html</remark>
/// <returns>(������ torelance �s�N�Z�������L��������A)�������邩�ۂ��B��������ꍇ��true�B</returns>
bool geo::isIntersected(cv::Vec4i ls1, cv::Vec4i ls2, int tolerance, double lowerThetaLimit, cv::Vec2i& xy)
{
	cv::Point3i p1(ls1[0], ls1[1], 0);
	cv::Point3i p2(ls1[2], ls1[3], 0);
	cv::Point3i p3(ls2[0], ls2[1], 0);
	cv::Point3i p4(ls2[2], ls2[3], 0);

	auto p12 = p2 - p1;
	auto p34 = p4 - p3;
	auto p12NormP34Norm = cv::norm(p12)*cv::norm(p34);

	//2�x�N�g���Ԃ̊p�x�����ȉ��̂��̂͏��O����B(=���s�ɋ߂��Ƃ݂Ȃ� -> �p�ł͂Ȃ�)
	auto sineLowerLimit = sin(lowerThetaLimit * PI / 180.0);
	auto det = abs(p12.cross(p34).z);
	if (det / p12NormP34Norm < sineLowerLimit) return false;

	//��_�̍��W�����߂�
	//��_��}��ϐ�s,t���g���ĕ\������

	auto xi = (p4.y - p3.y)*(p4.x - p1.x) - (p4.x - p3.x)*(p4.y - p1.y);
	auto eta = (p2.x - p1.x)*(p4.y - p1.y) - (p2.y - p1.y)*(p4.x - p1.x);
	auto delta = (p4.y - p3.y)*(p2.x - p1.x) - (p4.x - p3.x)*(p2.y - p1.y);

	auto s = double(xi) / delta;
	auto t = double(eta) / delta;

	auto deltaS = double(tolerance) / cv::norm(p12);
	auto deltaT = double(tolerance) / cv::norm(p34);

	//(���Ƃ��������Ō����������ȏꍇ���܂߂�)��_�̍��W�����߂�
	if (-deltaS <= s && s <= (1 + deltaS) &&
		-deltaT <= t && t <= (1 + deltaT))
	{
		auto m = (1 - s)*p1 + s*p2;  // m= t*p3 + (1-t)*p4 //�ł�����
		xy = cv::Point2i(round(m.x), round(m.y));
		return true;
	}

	//�ݒ肵���͈͊O�Ɍ�_���������ꍇ�́A�u�������Ȃ������v�Ƃ݂Ȃ�
	return false;
}
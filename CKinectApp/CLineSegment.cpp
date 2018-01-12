#define _USE_MATH_DEFINES
#include <cmath>
#include "stdafx.h"
#include "CLineSegment.h"
#include "geo.h"
#include <algorithm>
#include <valarray>
#include "CUtil.h"

/// <summary>�R���X�g���N�^</summary>
CLineSegment::CLineSegment()
{
	P1 = cv::Point2d(0, 0);
	P2 = cv::Point2d(1, 0);
	CalcParam();
}

/// <summary>�R���X�g���N�^�B�����̍��W�lls=(x1,y1,x2,y2)�������ɂƂ�B</summary>
CLineSegment::CLineSegment(cv::Vec4i ls)
{
	P1 = cv::Point2d(ls[0], ls[1]);
	P2 = cv::Point2d(ls[2], ls[3]);

	//x���W������������P1, P2
	if( P2.x < P1.x)
	{
		auto temp = P1;
		P1 = P2;
		P2 = temp;
	}
	else if(P1.x==P2.x && P2.y<P1.y)
	{
		//P1,P2��x���W�������ꍇ�́Ay���W������������P1,P2�Ƃ���
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

	//x���W������������P1, P2
	if (P2.x < P1.x)
	{
		auto temp = P1;
		P1 = P2;
		P2 = temp;
	}
	else if (P1.x == P2.x && P2.y<P1.y)
	{
		//P1,P2��x���W�������ꍇ�́Ay���W������������P1,P2�Ƃ���
		auto temp = P1;
		P1 = P2;
		P2 = temp;
	}

	CalcParam();
}

/// <summary>�f�X�g���N�^</summary>
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

/// <summary>�ȉ��̏��ɑ召���r�ł��鎞�_�ł��̐^�U�l��Ԃ�: Slope, Intersect, P1.x, P1.y, P2.x, P2.y</summary>
bool CLineSegment::operator<(CLineSegment ls)
{
	if (Slope != ls.Slope) return Slope < ls.Slope;
	if (Intersect != ls.Intersect) return Intersect < ls.Intersect;
	if (P1.x != ls.P1.x) return P1.x < ls.P1.x;
	if (P1.y != ls.P1.y) return P1.y < ls.P1.y;
	if (P2.x != ls.P2.x) return P2.x < ls.P2.x;
	return P2.x < ls.P1.x;
}


/// <summary>P1, P2 �͒�`�ςƂ��āA���̃p�����[�^���v�Z����B</summary>
void CLineSegment::CalcParam()
{
	//P1, P2 �͒�`�ςƂ��āA���̑��̃v���p�e�B���v�Z����
	X1y1x2y2 = cv::Vec4i(P1.x, P1.y, P2.x, P2.y);
	Slope = double(P2.y - P1.y) / double(P2.x - P1.x);
	Intersect = double(P2.x*P1.y - P1.x*P2.y) / double(P2.x - P1.x);
	Theta = atan(Slope) * 180.0 / M_PI;// RAD2DEG;

	Distance = CalcDistanceFrom(cv::Point(0, 0));

	//���_�̍��W���v�Z
	Center.x = (int)((P1.x + P2.x) / 2.0);
	Center.y = (int)((P1.y + P2.y) / 2.0);

	LineLength = std::sqrt(std::pow(P1.x - P2.x, 2) + std::pow(P1.y - P2.y, 2));

}

/// <summary>�����̒[�_����A�������v�Z </summary>
/// <param name="p1">�[�_�P</param>
/// <param name="p2">�[�_�Q</param>
/// <returns></returns>
CLineSegment CLineSegment::CaleLine(cv::Point2i p1, cv::Point2i p2)
{
	auto tmpPoints = cv::Vec4i(p1.x, p1.y, p2.x, p2.y);
	auto tmpLine = CLineSegment{ tmpPoints };

	return tmpLine;
}

/// <summary>����������1�ɂ܂Ƃ߂ďd������������</summary>
/// <param name="lineSegments">�d������������O�� CLineSegments �̃x�N�g��</param>
/// <param name="thetaResolution">x����������̊p�x(theta[deg])�̍������̒l�����ł���΁A����̒����Ƃ݂Ȃ�</param>
/// <param name="intersectResolution">y�ؕ�(Intersect)�̍������̒l�����ł���΁A����̒����Ƃ݂Ȃ�</param>
/// <returns>�d������菜���� CLineSegment �̃x�N�g��</returns>
std::vector<CLineSegment> CLineSegment::UniqLines(std::vector<CLineSegment> lineSegments, double thetaResolution, double intersectResolution)
{
	//�f�o�b�O�E�B���h�E�Ŋm�F����ꍇ�ɂ́A�\�[�g�����ق������₷��
	//sort(lineSegments.begin(), lineSegments.end());

	//�����X���E�ؕЂ������Ă���΁A��̐����ɂ܂Ƃ߂�
	auto ls1 = lineSegments.begin();
	while (ls1 != lineSegments.end())
	{
		auto ls2 = ls1 + 1;
		while (ls2 != lineSegments.end())
		{
			//�قړ��������Ȃ�
			if (abs((*ls1).Theta - (*ls2).Theta) < thetaResolution &&
				abs((*ls1).Distance - (*ls2).Distance) < intersectResolution)
			{
				//����ls1, ls2�𓝈�

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

				//����*ls1�ɓ��������V���Ȑ������㏑���Als2�͏���
				*ls1 = CLineSegment(cv::Vec4d(x1, y1, x2, y2));  //ls1��l���㏑��
				(*ls1).Index = std::string("[") + ls1Label + "-" + ls2Label + "]";
				ls2 = lineSegments.erase(ls2);                   //ls2�͏���
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

/// <summary>����������1�ɂ܂Ƃ߂ďd������������</summary>
/// <param name="lineSegments">�d������������O�� CLineSegments �̃x�N�g��</param>
/// <param name="thetaResolution">x����������̊p�x(theta[deg])�̍������̒l�����ł���΁A����̒����Ƃ݂Ȃ�</param>
/// <param name="intersectResolution">y�ؕ�(Intersect)�̍������̒l�����ł���΁A����̒����Ƃ݂Ȃ�</param>
/// <returns>�d������菜���� CLineSegment �̃x�N�g��</returns>
std::vector<CLineSegment> CLineSegment::UniqLinesTmp(std::vector<CLineSegment> lineSegments, double thetaResolution, double intersectResolution)
{
	//�f�o�b�O�E�B���h�E�Ŋm�F����ꍇ�ɂ́A�\�[�g�����ق������₷��
	//sort(lineSegments.begin(), lineSegments.end());

	//�����X���E�ؕЂ������Ă���΁A��̐����ɂ܂Ƃ߂�
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
				//����ls1, ls2�𓝈�

				std::vector<int> xLst = { (*ls1).P1.x, (*ls1).P2.x, (*ls2).P1.x, (*ls2).P2.x };
				std::vector<int> yLst = { (*ls1).P1.y, (*ls1).P2.y, (*ls2).P1.y, (*ls2).P2.y };

				size_t minIndex = std::distance(xLst.begin(), std::min_element(xLst.begin(), xLst.end()));
				auto x1 = xLst[minIndex];
				auto y1 = yLst[minIndex];

				size_t maxIndex = std::distance(xLst.begin(), std::max_element(xLst.begin(), xLst.end()));
				auto x2 = xLst[maxIndex];
				auto y2 = yLst[maxIndex];

				//����*ls1�ɓ��������V���Ȑ������㏑���Als2�͏���
				auto margeStr = std::string((*ls1).Index + "-" + (*ls2).Index);
				*ls1 = CLineSegment(cv::Vec4d(x1, y1, x2, y2),margeStr);  //ls1��l���㏑��
				ls2 = lineSegments.erase(ls2);                   //ls2�͏���
			}

			////�قړ��������Ȃ�
			//if (abs((*ls1).Theta - (*ls2).Theta) < thetaResolution &&
			//	abs((*ls1).Intersect - (*ls2).Intersect) < intersectResolution)
			//{
			//	//����ls1, ls2�𓝈�

			//	std::vector<int> xLst = { (*ls1).P1.x, (*ls1).P2.x, (*ls2).P1.x, (*ls2).P2.x };
			//	std::vector<int> yLst = { (*ls1).P1.y, (*ls1).P2.y, (*ls2).P1.y, (*ls2).P2.y };

			//	size_t minIndex = std::distance(xLst.begin(), std::min_element(xLst.begin(), xLst.end()));
			//	auto x1 = xLst[minIndex];
			//	auto y1 = yLst[minIndex];

			//	size_t maxIndex = std::distance(xLst.begin(), std::max_element(xLst.begin(), xLst.end()));
			//	auto x2 = xLst[maxIndex];
			//	auto y2 = yLst[maxIndex];

			//	//����*ls1�ɓ��������V���Ȑ������㏑���Als2�͏���
			//	*ls1 = CLineSegment(cv::Vec4d(x1, y1, x2, y2));  //ls1��l���㏑��
			//	ls2 = lineSegments.erase(ls2);                   //ls2�͏���
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


/// <summary>x��(1,0)�x�N�g���Ƃ̊p�x�� �}theta[deg] �����ł�������𒊏o����</summary>
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

/// <summary>����(x1,y1,x2,y2)�̃x�N�g���� CLineSegment �̃x�N�g���ւƕϊ�����</summary>
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

/// <summary>�����̊p�x Theta ���AlowerLimitTheta�ȏ�upperLimitTheta�ȉ��ł����true��Ԃ��B�����łȂ����false��Ԃ��B</summary>
bool CLineSegment::hasThetaBetween(double lowerLimitTheta, double upperLimitTheta)
{
	if (Theta < lowerLimitTheta) return false;
	if (upperLimitTheta < Theta) return false;
	return true;
}

/// <summary>�����̊p�xTheta ���A(theta-dTheta) �ȏ� (theta+dTheta) �ȉ��ł�������̃x�N�g����Ԃ��B</summary>
/// <param name="lineSegments">���o�O�̐����̃x�N�g��</param>
/// <param name="theta">���o�������ۂ̊�ƂȂ�p�x</param>
/// <param name="dTheta">�p�x�ɑ΂���덷�B0�ȏ�̐�����͂��邱�ƁB-1���w�肵���ꍇ�́A dTheta = 0.1*abs(theta) �ƂȂ�B</param>
std::vector<CLineSegment> CLineSegment::Extract(std::vector<CLineSegment> lineSegments, double theta, double dTheta)
{
	//dTheta ���ȗ�����Ă����ꍇ�̒l��ݒ�
	if (dTheta == -1.0) dTheta = 0.1*theta;

	//dTheta�̕���������
	dTheta = abs(dTheta);

	//dTheta�̍ő�l��ݒ�
	if (90.0 < dTheta) dTheta = 90.0;

	//�udTheta�͐����v�Ƃ��Ĕ͈͂��v�Z����
	double lowerLimitTheta = theta - dTheta;
	double upperLimitTheta = theta + dTheta;

	//�p�x���͈�[-90, 90]�𒴂����ꍇ�̕ϊ�
	double lowerLimitTheta1, upperLimitTheta1, lowerLimitTheta2, upperLimitTheta2;

	//�����ɍ�������������ans�ɒǉ�����
	std::vector<CLineSegment> ans;

	// �ꍇ���� �p�x�͈̔�: lowerLimitTheta < -90 < +90 < upplerLimitTheta �̂Ƃ�
	if(lowerLimitTheta < -90.0 && +90 < upperLimitTheta)
	{
		//�ⓚ���p�őS�������𖞂���
		ans.insert(ans.end(), lineSegments.begin(), lineSegments.end());
		return ans;
	}
	else if(lowerLimitTheta < -90.0)
	{
		// �ꍇ���� �p�x�͈̔�:  lowerLimitTheta < -90 < upperLimitTheta < +90
		lowerLimitTheta1 = 180.0 + lowerLimitTheta;
		upperLimitTheta1 = 180.0 + (-90.0);
		auto angularCenter = 0.5*(lowerLimitTheta1 + upperLimitTheta1);
		auto angularError = upperLimitTheta1 - angularCenter;
		auto res = Extract(lineSegments, angularCenter, angularError);
		ans.insert(ans.end(), res.begin(), res.end());
	}
	else if (90.0 < upperLimitTheta)
	{
		// �ꍇ���� �p�x�͈̔�:  -90  < lowerLimitTheta < + 90 < upperLimitTheta
		lowerLimitTheta2 = -180.0 + (+90);
		upperLimitTheta2 = -180.0 + upperLimitTheta;
		auto angularCenter = 0.5*(lowerLimitTheta2 + upperLimitTheta2);
		auto angularError = upperLimitTheta2 - angularCenter;
		auto res = Extract(lineSegments, angularCenter, angularError);
		ans.insert(ans.end(), res.begin(), res.end());
	}

	// �ꍇ���� �p�x�͈̔�:  -90  < lowerLimitTheta < upperLimitTheta < + 90
	for (auto ls : lineSegments)
	{
		if (ls.hasThetaBetween(lowerLimitTheta, upperLimitTheta))
		{
			ans.push_back(ls);
		}
	}

	return ans;
}

/// <summary>�����𗼑��։������������ƁA�_(x,y)�Ƃ̊Ԃ̋�����Ԃ�</summary>
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

/// <summary>�w��_����߂��������擾����</summary>
/// <param name="lines">�����Ώۂ̒����Q</param>
/// <param name="point">�w��_</param>
/// <returns></returns>
CLineSegment CLineSegment::GetNearLine(const std::vector<CLineSegment> lines, cv::Vec2i point)
{
	//point��lines�̋�����S�Čv�Z����
	std::vector<double> lengts;

	for (auto line : lines)
	{
		auto length = abs(CalcPointLineDis(point, line.P1, line.P2)); //�����ɂ͋����Ȃ��̂�
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

/// <summary>�w��_����߂��������擾����</summary>
/// <param name="lines">�����Ώۂ̒����Q</param>
/// <param name="point">�w��_(���t�@�����X�_)</param>
/// <param name="dir">�������� 1:�������� 2:��������</param>
/// <returns></returns>
CLineSegment CLineSegment::GetNearLineTmp(const std::vector<CLineSegment> lines, cv::Vec2i point,int dir)
{
	//point�̐��������iY�������j�Ɍ����������邩���ׂ�
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
					//������������ Ref1
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

					//������������ Ref1
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

	//point��lines�̋�����S�Čv�Z����
	std::vector<double> lengts;

	for (auto line : lines)
	{
		//auto length = abs(CalcPointLineDis(point, line.P1, line.P2)); //�����ɂ͋����Ȃ��̂�
		//auto p1p2Length = sqrt((line.P1.x*line.P1.x)+(line.P1.y*line.P1.y));
		auto p1p2Length = line.LineLength;
		auto p1p2PointLength = sqrt((point[0] - line.P1.x)*(point[0] - line.P1.x) + (point[1] - line.P1.y)*(point[1] - line.P1.y)) +
			sqrt((point[0] - line.P2.x)*(point[0] - line.P2.x) + (point[1] - line.P2.y)*(point[1] - line.P2.y));
		
		auto length = std::pow(p1p2PointLength,1) / std::pow(p1p2Length,1);

		lengts.push_back(length);
	}

	auto minIT = std::min_element(lengts.begin(), lengts.end());
	auto minIndex = std::distance(lengts.begin(), minIT);	//�ŏ��l�̃C���f�b�N�X

	//point����v�Z���ꂽ�ŏ��lLine�̒��_�����Ԓ�����ɑ��̐����Ȃ�����������
	
	//point�Ə�L�������ʂ�Clinesegment�̒��_�����Ԑ���
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
			//	//��������
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
				//������������ Ref1
				auto minY = std::min<int>(abs(point[1]-line.P1.y), abs(point[1] - line.P2.y));
				if(tmpY > minY)
				{
					tmpY = minY;
					minIndex = index;
				}
			}
			else if(dir==2)
			{
				//������������ Ref1
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
			//��������

		}
	}

	return lines[minIndex];
}

/// <summary>�����Ɛ������������Ă��邩�m�F����</summary>
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
	//		//����������P��Ԃ��B				
	//		return true;
	//	}
	//}
	////�������Ă��Ȃ�������0��Ԃ��B
	//else return false;

}

/// <summary>point ��x���W�� ����ls��x���W�͈̔͂ɓ����Ă�������̂����Apoint �Ƃ̋������ŒZ�ƂȂ������Ԃ��B����O�q�̏����𖞂������̂��Ȃ��ꍇ�́ACLineSegment(-999999,-999999,999999,999999)��Ԃ��B</summary>
/// <param name="lines"></param>
/// <param name="point"></param>
/// <param name="iter"></param>
/// <returns></returns>
CLineSegment CLineSegment::GetNearLine(const std::vector<CLineSegment> lines, cv::Vec2i point, int& index)
{

	//point��lines�̋�����S�Čv�Z����

	std::vector<CLineSegment> lsVec;  //�����𖞂��������̃x�N�g��
	std::vector<double> lengths;      //�����𖞂������������� point �܂ł̋����̃x�N�g��

	for (auto line : lines)
	{
		//�ȉ���2�����̂ǂ��炩����������Ȃ�A�Ԃ������̌��ɂ����B
		// 1. line.P1.x �� point[0] �� line.P2.x
		// 1. line.P2.x �� point[0] �� line.P1.x
		if ((line.P1.x - point[0]) * (line.P2.x- point[0]) <= 0)
		{
			auto length = abs(CalcPointLineDis(point, line.P1, line.P2)); //�����ɂ͋����Ȃ��̂�
			lengths.push_back(length);
			lsVec.push_back(line);
		}
	}


	//�Ԃ�l
	CLineSegment ans;

	//�����𖞂���������1�ȏ㑶�݂���ꍇ�́A�������ŏ��l�ƂȂ������Ԃ�
	if (lengths.size() == 0)
	{
		//������Ȃ�������
		index = -1;
		ans = (cv::Vec4i(-999999, -999999, +999999, 999999)); //����Ȃɑ傫�ȃT�C�Y�̉摜�𑀍삵�Ȃ��ł��傤
	}
	else
	{
		//1�ȏ㌩��������
		auto minIT = std::min_element(lengths.begin(), lengths.end());
		auto minIndex = std::distance(lengths.begin(), minIT);

		//TODO index �� lines �ɑ΂�����̂ɏC�����邱�ƁB���݂́Alengths �ɑ΂�����̂ɂȂ��Ă���B
		auto tmpIter = lines.begin() + minIndex;
		index = minIndex;
		ans = lsVec[minIndex];
	}

	return ans;
}

/// <summary>�����Ɠ_�̋������v�Z����</summary>
/// <param name="point">�_(x,y)</param>
/// <param name="lineP1">�����̒[�_p1(p1x,p1y)</param>
/// <param name="lineP2">�����̒[�_p2(p2x,p2y</param>
/// <returns></returns>
double CLineSegment::CalcPointLineDis(cv::Vec2i point, cv::Vec2i lineP1, cv::Vec2i lineP2)
{
	auto p1p = point - lineP1;
	auto p2p = point - lineP2;

	//auto area = p1p.cross(p2p);

	auto area = p1p[0] * p2p[1] - p1p[1] * p2p[0]; //p1p��p2p�Ō`�����镽�s�l�ӌ`�̖ʐ�
	auto p12Length = norm(lineP2 - lineP1);	//p12�̒���
	auto length = area / p12Length;

	return length;
}

bool CLineSegment::operator==(const CLineSegment& ls){
	return (P1 == ls.P1 && P2 == ls.P2);


}

/// <summary>������2���W���󂯎��A���� ax+by+c=0 �̃p�����[�^ (a,b,c) ��Ԃ��B</summary>
/// <returns>ax+by+c=0 ��(a,b,c)</returns>
cv::Point3d CLineSegment::CalcAbc()
{
	auto a = +(P2.y - P1.y);
	auto b = -(P2.x - P1.x);
	auto c = P2.y*(P2.x - P1.x) - P2.x*(P2.y - P1.y);
	cv::Vec3d ans(a, b, c);
	return ans;
}
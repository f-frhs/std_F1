#ifndef CLINE_SEGMENT_H
#define CLINE_SEGMENT_H

#pragma warning(disable: 4819)
#include <opencv2/opencv.hpp>
#pragma warning(default: 4819)
#include <windows.h>
#include <debugapi.h>


///<summary>�����N���X</summary>
class CLineSegment
{
private:
	/// <summary>P1, P2 �͒�`�ςƂ��āA���̃p�����[�^���v�Z����B</summary>
	void CalcParam();

public:
	/// <summary>����(x1,y1,x2,y2)���i�[��������</summary>
	cv::Vec4i X1y1x2y2;

	/// <summary>�_(x1,y1)</summary>
	cv::Point2i P1;

	/// <summary>�_(x2,y2)</summary>
	cv::Point2i P2;

	/// <summary>P1,P2�̒��_���W </summary>
	cv::Point2i Center;

	/// <summary>�C���f�b�N�X�ԍ�</summary>
	std::string Index;

	double LineLength;

	/// <summary>���� y = ax + b �� a(�X��)</summary>
	double Slope;

	/// <summary>���� y = ax + b ��b(y�ؕ�)</summary>
	double Intersect;

	/// <summary>tan(Theta) = Slope �ƂȂ� Theta[deg]</summary>
	double Theta;

	/// <summary>���_(0,0)���炱�̒����܂ł̋����B</summary>
	double Distance;

	/// <summary>�R���X�g���N�^</summary>
	CLineSegment();

	/// <summary>�R���X�g���N�^�B�����̍��W�l ls = (x1,y1,x2,y2)�������ɂƂ�B</summary>
	CLineSegment(cv::Vec4i ls);

	CLineSegment(cv::Vec4i ls, std::string index);

	/// <summary>�f�X�g���N�^</summary>
	~CLineSegment();
	
	/// <summary>�����̒�������Ԓ������̂�Ԃ�</summary>
	static CLineSegment GetLongestFrom(const std::vector<CLineSegment> lineSegments);

	/// <summary>����������1�ɂ܂Ƃ߂ďd������������</summary>
	static std::vector<CLineSegment> UniqLines(std::vector<CLineSegment> lineSegments, double thetaResolution = 6, double intersectResolution = 10);

	/// <summary>�ȉ��̏��ɑ召���r�ł��鎞�_�ł��̐^�U�l��Ԃ�: Slope, Intersect, P1.x, P1.y, P2.x, P2.y</summary>
	bool operator <(CLineSegment ls);

	/// <summary> x��(1,0)�x�N�g���Ƃ̊p�x�� theta �����ł�������𒊏o����</summary>
	static std::vector<CLineSegment> ExtractHorizontalLineSegments(std::vector<CLineSegment> extractedLineSegments, double theta);

	// / <summary>����(x1,y1,x2,y2)�̃x�N�g���� CLineSegment �̃x�N�g���ւƕϊ�����</summary>
	static std::vector<CLineSegment> ConverFrom(std::vector<cv::Vec4i> x1y1x2y2List);

	/// <summary>�����̊p�x Theta ���AlowerLimitTheta �ȏ� upperLimitTheta �ȉ��ł���� true ��Ԃ��B�����łȂ���� false ��Ԃ��B</summary>
	bool hasThetaBetween(double lowerLimitTheta, double upperLimitTheta);

	/// <summary>�����̊p�x Theta ���A(theta-dTheta) �ȏ� (theta+dTheta) �ȉ��ł�������̃x�N�g����Ԃ��B</summary>
	static std::vector<CLineSegment> Extract(std::vector<CLineSegment> lineSegments, double theta, double dTheta = -1.0);

	/// <summary>�����𗼑��։������������ƁA�_(x,y)�Ƃ̊Ԃ̋�����Ԃ�</summary>
	double CalcDistanceFrom(cv::Point2i xy);

	/// <summary>�w��_����߂��������擾����</summary>
	static CLineSegment GetNearLine(const std::vector<CLineSegment> lines, cv::Vec2i point);

	/// <summary>�����̒��_����ԉ��ɂ��������Ԃ��B</summary>
	static CLineSegment GetLowest(const std::vector<CLineSegment> lines, cv::Vec2i point);

	/// <summary>�����Ɠ_�̋������v�Z����</summary>
	static double CalcPointLineDis(cv::Vec2i point, cv::Vec2i lineP1, cv::Vec2i lineP2);

	bool operator==(const CLineSegment & ls);

	/// <summary>point ��x���W�� ����ls��x���W�͈̔͂ɓ����Ă�������̂����Apoint �Ƃ̋������ŒZ�ƂȂ������Ԃ��B����O�q�̏����𖞂������̂��Ȃ��ꍇ�́ACLineSegment(-999999,-999999,999999,999999)��Ԃ��B</summary>
	static CLineSegment CLineSegment::GetNearLine(const std::vector<CLineSegment> lines, cv::Vec2i point, int& index);

	static CLineSegment GetNearLineTmp(const std::vector<CLineSegment> lines, cv::Vec2i point, int dir);

	/// <summary>�����̒[�_����A�������v�Z </summary>
	static CLineSegment CaleLine(cv::Point2i p1, cv::Point2i p2);

	/// <summary>����������1�ɂ܂Ƃ߂ďd������������</summary>
	static std::vector<CLineSegment> UniqLinesTmp(std::vector<CLineSegment> lineSegments, double thetaResolution = 6, double intersectResolution = 10);

	/// <summary>�����Ɛ������������Ă��邩�m�F���� </summary>
	static bool IsCrossed(CLineSegment baseLine, CLineSegment targetLine);

	/// <summary>������2���W���󂯎��A���� ax+by+c=0 �̃p�����[�^ (a,b,c) ��Ԃ��B</summary>
	cv::Point3d CalcAbc();
};

#endif // !CLINE_SEGMENT_H

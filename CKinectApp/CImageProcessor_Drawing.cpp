#include "CImageProcessor.h"
#include "CLineSegment.h"


/// <summary>画像に複数ラインを描画する</summary>
/// <param name="dest">描画先画像</param>
/// <param name="lineSegments">描画するライン群</param>
/// <param name="lilneColor">線の色</param>
/// <param name="circleSize">線分端点のサークルサイズ</param>
void CImageProcessor::DrawLines(cv::Mat& dest, std::vector<CLineSegment> lineSegments, cv::Scalar lilneColor, int circleSize)
{
	for (auto lineElement : lineSegments)
	{
		Draw(lineElement, dest, lilneColor, 2, lineElement.Index, 2, RED, BLUE, 2, 2, 2);
	}
}

void CImageProcessor::Draw(cv::Point2i pos, cv::Mat& dst, int radius, int thickness, const cv::Scalar color, std::string label, const cv::Scalar labelColor, int labelSize, cv::Point offset)
{
	cv::Point p(pos);
	cv::circle(dst, p, 2, color, 2);               //中心の点
	cv::circle(dst, p, radius, color, thickness);  // 同心円を描画

	cv::putText(dst, label, p+offset, CV_FONT_HERSHEY_TRIPLEX, labelSize, labelColor);
};

void CImageProcessor::Draw(CLineSegment ls, cv::Mat& dst, cv::Scalar lineColor, int thickness, std::string label, int radius, cv::Scalar startingPointColor, cv::Scalar endingPointColor, int startingPoitCircleSize, int endingPointCircleSize, int circleThickness, const cv::Scalar labelColor, int labelSize, cv::Point labelOffset)
{
	cv::line(dst, ls.P1, ls.P2, lineColor, thickness);
	cv::circle(dst, ls.P1, startingPoitCircleSize, RED, circleThickness);
	cv::circle(dst, ls.P2, endingPointCircleSize, BLUE, circleThickness);

	cv::putText(dst, label, ls.Center + labelOffset, CV_FONT_HERSHEY_TRIPLEX, 0.8, WHITE, 3, CV_AA);
	cv::putText(dst, label, ls.Center + labelOffset, CV_FONT_HERSHEY_TRIPLEX, 0.8, BLACK, 1.5, CV_AA);
};


#ifndef GEO_H
#define GEO_H

#pragma warning(disable: 4819)
#include <opencv2/opencv.hpp>
#pragma warning(default: 4819)

#include "CLineSegment.h"


class geo
{
public:
	geo();
	~geo();
	static std::vector<cv::Vec2i> geo::DetectCorners(std::vector<CLineSegment> lsList, int tolerance, double lowerThetaLimit);
	static bool geo::isIntersected(cv::Vec4i ls1, cv::Vec4i ls2, int tolerance, double lowerThetaLimit, cv::Vec2i& xy);
};

#endif // !GEO_H

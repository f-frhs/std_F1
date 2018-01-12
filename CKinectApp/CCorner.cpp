#include "stdafx.h"
#include "CCorner.h"


CCorner::CCorner()
{
}

CCorner::CCorner(cv::Point2i inputXy)
{
	xy = inputXy;
}


CCorner::~CCorner()
{
}

std::string CCorner::ToPacket()
{
	std::stringstream ss;
	ss << xy.x << ":" << xy.y;
	return ss.str();
}

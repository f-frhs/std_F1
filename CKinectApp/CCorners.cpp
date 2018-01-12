#include "CCorners.h"

CCorners::CCorners()
{
	cornersList = {};
}

CCorners::CCorners(std::vector<CCorner> inputCornersVec)
{
	cornersList = inputCornersVec;
}

CCorners::~CCorners()
{
}

/// <summary> コーナー交点の座標列を文字列へ変換する </summary>
/// <returns>Corners,[size],[n],[x1:y1;x2:y2;...;xn:yn] 形式の文字列</returns>
/// <remarks>https://git.io/v7yM5</remarks>
std::string CCorners::ToPacket(std::string identifier, std::string sep, std::string sepBetweenCorners)
{
	//コーナー座標(1-n)の文字列を連結する
	auto cornersCount = cornersList.size();  // n のこと
	std::string corners("");
	for (auto i = 0; i < cornersCount; i++)
	{
		if (i < cornersCount - 1) corners += cornersList[i].ToPacket() + sepBetweenCorners;
		else if (i == cornersCount - 1) corners += cornersList[i].ToPacket();
	}

	//パケットとして必要な形式に整える
	auto cornersDataLength = std::to_string(corners.size());
	auto allPacketString
		= identifier + sep
		+ cornersDataLength + sep
		+ std::to_string(cornersCount) + sep
		+ corners;

	return allPacketString;
}

CCorners& CCorners::operator= (const CCorners corners){
	cornersList = corners.cornersList;
	return *this;
}

#ifndef CCORNERS_H
#define CCORNERS_H

#include "stdafx.h"
#include <vector>
#include "CCorner.h"

class CCorners
{
private:
	const std::string packetName;
	std::vector<CCorner> cornersList;

	/// <summary> パケットの識別子。 </summary>
	std::string identifier = "Corners";
	
	/// <summary> パケット内要素間の区切り文字 </summary>
	std::string sep = ",";

	/// <summary> コーナー座標間の区切り文字 </summary>
	std::string sepBetweenCorners = ";";
public:
	CCorners();
	CCorners(std::vector<CCorner> corners);
	~CCorners();
	CCorners& CCorners::operator= (const CCorners corners);

	std::string ToPacket(std::string identifier = std::string("Corners"), std::string sep = std::string(","), std::string sepBetweenCorners = std::string(";"));
};

#endif CCORNERS_H
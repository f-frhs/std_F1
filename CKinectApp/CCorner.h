#ifndef CCORNER_H
#define CCORNER_H
#include "CLineSegment.h"

/// <summary>角のクラス</summary>
class CCorner
{
public:
	/// <summary>コーナーを形成する線分1</summary>
	CLineSegment ls1;

	/// <summary>コーナーを形成する線分2</summary>
	CLineSegment ls2;

	/// <summary>コーナーの座標(x,y)</summary>
	cv::Point2i xy;

	/// <summary>コンストラクタ(引数なし)</summary>
	CCorner();

	/// <summary>コンストラクタ(交点座標)</summary>
	CCorner(cv::Point2i inputXy);

	/// <summary>デストラクタ</summary>
	~CCorner();

	/// <summary> コーナー交点の座標 "x:y" を返す </summary>
	std::string ToPacket();
};

#endif  // !CCORNER_H

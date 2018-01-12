#ifndef CCORNER_H
#define CCORNER_H
#include "CLineSegment.h"

/// <summary>�p�̃N���X</summary>
class CCorner
{
public:
	/// <summary>�R�[�i�[���`���������1</summary>
	CLineSegment ls1;

	/// <summary>�R�[�i�[���`���������2</summary>
	CLineSegment ls2;

	/// <summary>�R�[�i�[�̍��W(x,y)</summary>
	cv::Point2i xy;

	/// <summary>�R���X�g���N�^(�����Ȃ�)</summary>
	CCorner();

	/// <summary>�R���X�g���N�^(��_���W)</summary>
	CCorner(cv::Point2i inputXy);

	/// <summary>�f�X�g���N�^</summary>
	~CCorner();

	/// <summary> �R�[�i�[��_�̍��W "x:y" ��Ԃ� </summary>
	std::string ToPacket();
};

#endif  // !CCORNER_H

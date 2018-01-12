#ifndef CCAMERA_INFO_MANAGER_H
#define CCAMERA_INFO_MANAGER_H

#include "stdafx.h"
#include <string>
#include <iostream> 
#include <cctype>

class CCameraInfoManager
{
public:
	int CamId;
	CCameraInfoManager();
	~CCameraInfoManager();

	/// <summary>inputChars ���� �J����ID �𒊏o�ł������ۂ���Ԃ��B���o�ł����ꍇ�́A camId �� �J����ID �̔ԍ���������B</summary>
	bool GetCameraId(char* inputChars, int inputCharsLength, int& camId, std::string);
};

#endif // ! CCAMERA_INFO_MANAGER_H
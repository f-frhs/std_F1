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

	/// <summary>inputChars から カメラID を抽出できたか否かを返す。抽出できた場合は、 camId に カメラID の番号を代入する。</summary>
	bool GetCameraId(char* inputChars, int inputCharsLength, int& camId, std::string);
};

#endif // ! CCAMERA_INFO_MANAGER_H
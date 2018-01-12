#ifndef PROGRAM_H
#define PROGRAM_H

#include "stdafx.h"
//#include "CKinectApp.cpp"
#include "CSettingOrganizer.h"
#include "CImageProcessor.h"
#include "CUtil.h"
#include "Setting.h"
#include "CWebCam.h"
#include "CCameraInfoManager.h"
#include "CPipeServer.h"
#include <string>
#include <ctype.h>  // for islower()
#include <algorithm>

class Program
{
public:
	Program();
	~Program();
	static void Program::parseDeltaSettingPacket(unsigned char * chars, std::string &valName, std::string &valValueStr);
	static std::string Program::get1stChar(HANDLE pipeHandle);
	static bool Program::isStructAll(HANDLE pipeHandle);
	static bool Program::isImage(HANDLE pipeHandle);
	static bool Program::isStructDelta(HANDLE pipeHandle);
	static bool Program::isEmpty(HANDLE pipeHandle);
};

#endif // !PROGRAM_H

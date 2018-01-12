#include "CCameraInfoManager.h"

CCameraInfoManager::CCameraInfoManager()
{
}


CCameraInfoManager::~CCameraInfoManager()
{
}

bool CCameraInfoManager::GetCameraId(char* inputChar, int inputCharsLength, int& camIdInt, std::string clientAction)
{
	std::string allString(inputChar, inputCharsLength);
	auto camSignal = allString.substr(0, 100);
	auto camIdplace = camSignal.find(",");
	auto actionFromClient = allString.substr(0,camIdplace);
	auto camIdStr = allString.substr(camIdplace + 1, 1);
	if (actionFromClient == "ConnectCamera") {
		if (std::isdigit(static_cast<unsigned char>(camIdStr[0]))) {  //‘ÎÛ‚ğ1•¶š‚ÆŒˆ‚ß‚Ä‚¢‚é‚Ì‚Å
			camIdInt = std::stoi(camIdStr);
			clientAction = "ConnectCamera";
			return true;
		}
	}
	else if (actionFromClient == "GetCamInfo") {
		if (std::isdigit(static_cast<unsigned char>(camIdStr[0]))) {  //‘ÎÛ‚ğ1•¶š‚ÆŒˆ‚ß‚Ä‚¢‚é‚Ì‚Å
			camIdInt = std::stoi(camIdStr);
			clientAction = "GetCamInfo";
			return true;
		}
	}
	else
	{
		return false;
	}
}
#ifndef CCOMMAND_PACKET_H
#define CCOMMAND_PACKET_H

#include <string>
#include <cassert>
#include <cctype>
#include "CUtil.h"

class CCommandPacket
{

public:
	std::string OriginalMessage;
	std::string Command;
	std::string NumString;
	int Num;
	bool IsParsedSuccessfully;

	CCommandPacket();
	CCommandPacket(std::string allMessage);
	bool TryParse();
};

#endif // ! CCOMMAND_PACKET_H
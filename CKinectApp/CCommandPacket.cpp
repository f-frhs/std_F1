#include "CCommandPacket.h"

CCommandPacket::CCommandPacket()
{
}


CCommandPacket::CCommandPacket(std::string allMessage)
{
	OriginalMessage = allMessage;
}

bool CCommandPacket::TryParse()
{
	assert(OriginalMessage != std::string(""));

	auto msgList = CUtil::Split(OriginalMessage, ',');
	if (msgList.size() != 2) return false;
	
	Command = msgList[0];
	NumString = msgList[1];

	//2番目の文字列を int 型へ変換する
	//TODO: 検査方法を修正すること
	//str2nd(3文字)が数字になりうるかをチェック
	if (!std::isdigit(static_cast<unsigned char>(NumString[0]))) return false;
	if (!std::isdigit(static_cast<unsigned char>(NumString[1]))) return false;
	if (!std::isdigit(static_cast<unsigned char>(NumString[2]))) return false;
	Num = std::stoi(NumString);

	return true;
}

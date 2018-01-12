#ifndef CUTIL_H
#define CUTIL_H


#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <codecvt>


class CUtil
{
public:
	//現在時刻を YMD_HIS形式で返す
	static std::string CUtil::getDateTimeString();

	//input を delimiter で区切る
	static std::vector<std::string> Split(const std::string& input, char delimiter);

	static std::wstring CUtil::StringToWString(const std::string& messageStr);
	static std::string CUtil::WString2String(std::wstring const & src);
	static std::string utf8_to_multi_cppapi(std::string const & src);
	static std::wstring utf8_to_wide_cppapi(std::string const & src);
	static std::string wide_to_multi_capi(std::wstring const & src);
};

#endif // !CUTIL_H

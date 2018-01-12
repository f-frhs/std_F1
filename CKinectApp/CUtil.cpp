#include "CUtil.h"

//現在時刻を YMD_HIS形式で返す
std::string CUtil::getDateTimeString()
{
	time_t t = time(NULL);
	struct tm tm;
	localtime_s(&tm, &t);

	char dateTime[81];
	strftime(dateTime, sizeof(dateTime), "%Y%m%d_%H%M%S", &tm);
	return dateTime;
}

std::vector<std::string> CUtil::Split(const std::string& input, char delimiter)
{
	std::istringstream stream(input);

	std::string field;
	std::vector<std::string> result;
	while (std::getline(stream, field, delimiter))
	{
		result.push_back(field);
	}
	return result;
}

/// <summary>std::stringをstd::wstringへ変換する</summary>
/// <param name="messageStr">変換元のstd::stringへの参照</param>
/// <returns></returns>
std::wstring CUtil::StringToWString(const std::string& messageStr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
	auto wString = cv.from_bytes(messageStr);

	return wString;
}

/// <summary>文字コード変換(utf-8 -> Shift_JIS) </summary>
/// <remarks>http://nekko1119.hatenablog.com/entry/2017/01/02/054629</remarks>
std::string CUtil::utf8_to_multi_cppapi(std::string const& src)
{
	auto const wide = utf8_to_wide_cppapi(src);
	return wide_to_multi_capi(wide);
}

/// <summary>文字コード変換(std::string(utf-8) -> std::wstring)</summary>
/// <remarks>http://nekko1119.hatenablog.com/entry/2017/01/02/054629</remarks>
std::wstring CUtil::utf8_to_wide_cppapi(std::string const& src)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(src);
}

/// <summary>文字コード変換(std::wstring -> std::string(Shift_JIS))</summary>
/// <remarks>http://nekko1119.hatenablog.com/entry/2017/01/02/054629</remarks>
std::string CUtil::wide_to_multi_capi(std::wstring const& src)
{
	std::size_t converted{};
	std::vector<char> dest(src.size() * sizeof(wchar_t) + 1, '\0');
	if (::_wcstombs_s_l(&converted, dest.data(), dest.size(), src.data(), _TRUNCATE, ::_create_locale(LC_ALL, "jpn")) != 0)
	{
		throw std::system_error{ errno, std::system_category() };
	}
	return std::string(dest.begin(), dest.end());
}
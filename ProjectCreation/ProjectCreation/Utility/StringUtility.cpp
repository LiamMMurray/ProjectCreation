#include "StringUtility.h"

#include <Windows.h>

using namespace std;

namespace StringUtility
{
	std::string utf8_encode(const std::wstring &wstr)
	{
		if (wstr.empty()) return std::string();
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	// Convert an UTF8 string to a wide Unicode String
	std::wstring utf8_decode(const std::string &str)
	{
		if (str.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	std::string getExtension(const string& path)
	{

		string::size_type i = path.rfind('.', path.length());

		return path.substr(i + 1, path.length() - i);
	}

	void AppendAndReplaceExtension(string& path, string app)
	{

		string::size_type s = path.rfind('\\', path.length());
		string::size_type i = path.rfind('.', path.length());

		string name = path.substr(s + 1, i - s - 1) + app;

		if (i != string::npos)
		{
			path.replace(s + 1, name.length(), name);
		}
	}


	void ReplaceExtension(std::string& path, const std::string& ext)
	{

		string::size_type i = path.rfind('.', path.length());

		if (i != string::npos)
		{
			path.replace(i + 1, ext.length(), ext);
		}
	}

	std::string GetFileNameFromPath(const string& path)
	{
		string::size_type s = path.rfind('\\', path.length());
		string::size_type i = path.rfind('.', path.length());

		return path.substr(s + 1, i - s - 1);
	}

}
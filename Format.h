#pragma once
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <string>

class Format
{
public:
	static std::string format(const char* msg, ...)
	{
		va_list  arg_ptr;
		va_start(arg_ptr, msg);

		int len = _vscprintf(msg, arg_ptr) + 1;
		std::string strBuf(len, '\0');
		vsprintf_s(&strBuf[0], len, msg, arg_ptr);

		va_end(arg_ptr);
		strBuf.pop_back();

		return strBuf;
	}

	template<unsigned int BUF_SIZE = 512>
	static const char* format_str(const char* msg, ...)
	{
		thread_local char buffer[BUF_SIZE];
		memset(buffer, 0, BUF_SIZE);

		va_list  arg_ptr;
		va_start(arg_ptr, msg);

		int len = _vscprintf(msg, arg_ptr) + 1;
		if (len > BUF_SIZE)
		{
			return "buffer overflow";
		}

		vsprintf_s(buffer, len, msg, arg_ptr);

		va_end(arg_ptr);

		return buffer;
	}

	static inline bool isSpace(const wchar_t& ch)
	{
		return ch == '\n' || ch == '\r' || ch == '\t' || ch == ' ';
	}

	static void ltrim(std::string& s)
	{
		int sz = (int)s.size();
		int iPos = 0;
		for (; iPos < sz; ++iPos)
		{
			if (!isSpace(s[iPos]))
				break;
		}

		if (iPos > 0)
		{
			s = s.substr(iPos);
		}
	}

	static void rtrim(std::string& s)
	{
		int sz = (int)s.size();
		int iPos = sz - 1;
		for (; iPos >= 0; --iPos)
		{
			if (!isSpace(s[iPos]))
				break;
		}

		if (iPos < sz - 1)
		{
			s = s.substr(0, iPos + 1);
		}
	}

	static void trim(std::string& s)
	{
		ltrim(s);
		rtrim(s);
	}

};
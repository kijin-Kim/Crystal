#pragma once
#include <debugapi.h>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>
#include <stdexcept>


//https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
template<typename ... Args>
std::string string_format(const std::string fmt_str, ...) {
	int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
	std::unique_ptr<char[]> formatted;
	va_list ap;
	while (true) {
		formatted = std::make_unique<char[]>(n); /* Wrap the plain char array into the unique_ptr */
		strcpy(&formatted[0], fmt_str.c_str());
		va_start(ap, fmt_str);
		final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	return std::string(formatted.get());
}

namespace Crystal {
	inline void Log(const char* prefixMsg, const char* msg)
	{
		OutputDebugStringA(prefixMsg);
		OutputDebugStringA(msg);
		OutputDebugStringA("\n");
	}
}

#define CS_FORMAT(...) string_format(__VA_ARGS__)
#define CS_LOG(...) Crystal::Log("Crystal Log : ", CS_FORMAT(__VA_ARGS__).c_str())
#define CS_ERROR(...) Crystal::Log("Crystal Error : ", CS_FORMAT(__VA_ARGS__).c_str())

#ifdef CS_DEBUG
#define CS_ASSERT(x, ...) if(!(x)) \
{ \
	std::string formatted = CS_FORMAT(__VA_ARGS__);\
	Crystal::Log("Crystal Error : ", formatted.c_str());\
	MessageBoxA(NULL, formatted.c_str(), "Assertion Failed", MB_OK);\
	__debugbreak();\
}
#elif defined(CS_RELEASE)
#define CS_ASSERT(x, ...) if(!(x)) { CS_ERROR(__VA_ARGS__);}
#endif

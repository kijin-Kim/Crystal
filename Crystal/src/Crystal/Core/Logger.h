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
		if (fmt_str.empty()) return std::string();
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

/*
* DEBUG_INFO : Debug 모드에서 출력되는 메시지
* INFO : Debug, Release 모드에서 출력되는 메시지
* WARN : Debug, Relase 모드에서 출력되는 메시지. 프로그램이 동작될 순 있으나 잠재적인 위험이 있음.
* FATAL : Debug, Relase 모드에서 출력되는 메시지. 프로그램에 심각한 오류로 프로그램이 정상적으로 동작될 수 없음. (Assertion)
*/

#define CS_FORMAT(...) string_format(__VA_ARGS__)

#define CS_INFO(...) Crystal::Log("[Crystal Info] : ", CS_FORMAT(__VA_ARGS__).c_str())
#define CS_WARN(...) Crystal::Log("[Crystal Warn] : ", CS_FORMAT(__VA_ARGS__).c_str())

#ifdef CS_DEBUG
#define CS_DEBUG_INFO(...) Crystal::Log("[Crystal Debug] : ", CS_FORMAT(__VA_ARGS__).c_str())
#define CS_FATAL(x, ...) if(!(x)) { \
	std::string formatted = CS_FORMAT(__VA_ARGS__).c_str();\
	Crystal::Log("[Crystal Fatal]: ", formatted.c_str());\
	MessageBoxA(NULL, formatted.c_str(), "Assertion Failed", MB_OK);\
	__debugbreak();\
}
#elif defined(CS_RELEASE)
#define CS_DEBUG_INFO(...)
#define CS_FATAL(x, ...) if(!(x)) { \
	std::string formatted = CS_FORMAT(__VA_ARGS__);\
	Crystal::Log("[Crystal Fatal] : ", formatted.c_str());\
	MessageBoxA(NULL, formatted.c_str(), "Assertion Failed", MB_OK);\
}
#endif

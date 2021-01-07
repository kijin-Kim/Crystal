#pragma once
#include <debugapi.h>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>
#include <stdexcept>


//https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	int size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	if (size <= 0) { throw std::runtime_error("Error during formatting."); }
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

namespace Crystal {
	inline void Log(const char* prefixMsg, const char* msg)
	{
		OutputDebugStringA(prefixMsg);
		OutputDebugStringA(msg);
	}
}

#define CS_FORMAT(...) string_format(__VA_ARGS__)
#define CS_LOG(...) Crystal::Log("Crystal Log : ",__VA_ARGS__)
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

#pragma once
#include <debugapi.h>
#include <cstdarg>
#include <cstdio>

namespace Crystal {
	class Logger
	{
	public:
		static void Log(const char* fmt, ...)
		{
			char buffer[1000] = {};

			va_list ap;
			va_start(ap, fmt);
			vsprintf_s(buffer, fmt, ap);
			va_end(ap);

			char output[1000] = {};
			sprintf_s(output, "[ Crystal Log : %s ]\n", buffer);

			OutputDebugStringA(output);
		}

		static void Error(const char* fmt, ...)
		{
			char buffer[1000] = {};

			va_list ap;
			va_start(ap, fmt);
			vsprintf_s(buffer, fmt, ap);
			va_end(ap);

			char output[1000] = {};
			sprintf_s(output, "[ Crystal Error : %s ]\n", buffer);

			OutputDebugStringA(output);
		}

		static void FormatOutput(char* const outBuffer, const char* fmt, ...)
		{
			char buffer[1000] = {};

			va_list ap;
			va_start(ap, fmt);
			vsprintf_s(buffer, fmt, ap);
			va_end(ap);

			sprintf(outBuffer, "[ Crystal Error : %s ]\n", buffer);
		}

	private:
		Logger() {};
	};
}


#define CS_LOG(...) Crystal::Logger::Log(__VA_ARGS__)
#define CS_ERROR(...) Crystal::Logger::Error(__VA_ARGS__)

#ifdef CS_DEBUG
#define CS_ASSERT(x, ...) if(!(x)) \
{ \
	CS_ERROR(__VA_ARGS__); \
	char buffer[1000] = {}; \
	Crystal::Logger::FormatOutput(buffer, __VA_ARGS__);\
	 MessageBoxA(NULL, buffer, "Assertion Failed", MB_OK); \
	__debugbreak();\
}
#elif defined(CS_RELEASE)
#define CS_ASSERT(x, ...) if(!(x)) { CS_ERROR(__VA_ARGS__);}
#endif

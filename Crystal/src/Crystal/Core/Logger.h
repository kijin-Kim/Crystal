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
			char buffer[200] = {};

			va_list ap;
			va_start(ap, fmt);
			vsprintf_s(buffer, fmt, ap);
			va_end(ap);

			char output[200] = {};
			sprintf_s(output, "[ Crystal Log : %s ]\n", buffer);

			OutputDebugStringA(output);
		}

		static void Error(const char* fmt, ...)
		{
			char buffer[200] = {};

			va_list ap;
			va_start(ap, fmt);
			vsprintf_s(buffer, fmt, ap);
			va_end(ap);

			char output[200] = {};
			sprintf_s(output, "[ Crystal Error : %s ]\n", buffer);

			OutputDebugStringA(output);
		}

	private:
		Logger() {};
	};
}


#define CS_LOG(...) Crystal::Logger::Log(__VA_ARGS__)
#define CS_ERROR(...) Crystal::Logger::Error(__VA_ARGS__)

#ifdef CS_DEBUG
#define CS_ASSERT(x, ...) if(!(x)) { CS_ERROR("Assertion Failed : %s", __VA_ARGS__); __debugbreak();}
#elif defined(CS_RELEASE)
#define CS_ASSERT(x, ...) if(!(x)) { CS_ERROR("Assertion Failed : %s", __VA_ARGS__);}
#endif

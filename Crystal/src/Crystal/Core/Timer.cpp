#include "cspch.h"
#include "Timer.h"

namespace Crystal {
	Timer::Timer()
	{
		LARGE_INTEGER f;
		QueryPerformanceFrequency(&f);
		m_InvFreq = 1000.0 / double(f.QuadPart);
	}

	void Timer::Tick()
	{
		double current = getCurrent();

		if(m_bShouldReset)
		{
			m_LastTime = current;
			m_bShouldReset = false;
		}

		m_DeltaTime = (current - m_LastTime) * m_InvFreq * (1 / 1000.0);
		m_ElapsedTime += m_DeltaTime;

		m_LastTime = current;
	}

	void Timer::Reset()
	{
		m_bShouldReset = true;
		m_ElapsedTime = 0.0;
	}

	double Timer::getCurrent()
	{
		LARGE_INTEGER c;
		QueryPerformanceCounter(&c);
		return c.QuadPart;
	}

}

#include "cspch.h"
#include "Timer.h"

namespace Crystal {
	Timer::Timer()
	{
		UINT64 countsPerSec = 0;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);

		m_SecondsPerCount = 1.0 / (double)countsPerSec;
		
	}

	void Timer::Tick()
	{
		if (m_bIsStopped)
		{
			m_DeltaTime = 0.0;
			return;
		}
		
		UINT64 currentTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
		m_CurrentTime = (double)currentTime;

		// Ÿ�̸Ӵ� ������ ùƽ�� 0.0�� ��ŸŸ�ӿ��� �����մϴ�
		if (m_LastTime != 0.0)
			m_DeltaTime = (m_CurrentTime - m_LastTime) * m_SecondsPerCount;

		m_LastTime = m_CurrentTime;

		if (m_DeltaTime < 0.0)
		{
			m_DeltaTime = 0.0;
		}

		m_ElapsedTime += m_DeltaTime;
	}

	void Timer::Reset()
	{
		m_ElapsedTime = 0.0f;
	}

}
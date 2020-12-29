#pragma once

namespace Crystal {
	class Timer final
	{
	public:
		Timer();
		~Timer() = default;

		void Tick();

		float DeltaTime() const { return (float)m_DeltaTime; }

	private:
		double m_SecondsPerCount = 0.0;

		double m_CurrentTime = 0.0;
		double m_DeltaTime = 0.0;
		double m_LastTime = 0.0;

		bool m_bIsStopped = false;
	};
}

#pragma once
#include <chrono>


namespace Crystal {

	class Timer final
	{
		
	public:
		Timer();
		~Timer() = default;



		void Tick();
		void Reset();


		float GetDeltaSeconds() const { return (float)m_DeltaTime; }
		float GetElapsedTime() const { return (float)m_ElapsedTime; }

		void SetElapsedTime(float elapsedTime) { m_ElapsedTime = (double)elapsedTime; }

	private:
		double getCurrent();

	private:
		double m_ElapsedTime = 0.0;
		double m_DeltaTime = 0.0;
		double m_LastTime = 0.0;
		double m_InvFreq = 0.0;

		bool m_bShouldReset = true;
	};
}



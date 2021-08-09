#pragma once

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


		void Pause();
		void Resume();

			

	private:
		double m_SecondsPerCount = 0.0;

		double m_CurrentTime = 0.0;
		double m_DeltaTime = 0.0;
		double m_ElapsedTime = 0.0;
		double m_LastTime = 0.0;

		bool m_bIsStopped = false;

		double m_Rate = 0.0;
		bool m_bShouldLoop = false;	
	};
}

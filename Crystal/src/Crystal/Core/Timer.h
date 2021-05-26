#pragma once
#include <functional>

#include "Core.h"

namespace Crystal {

#define CS_TIMER_FN(fn) std::bind(&fn, this)

	using TimerCallback = std::function<void()>;


	class Timer final
	{
		friend class TimerManager;
	public:
		Timer();
		~Timer() = default;

		void Tick();
		void Reset();

		void HandleCallback();

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
		
		TimerCallback m_Callback = nullptr;
	};


	class TimerManager final
	{
	public:
		static TimerManager& Instance()
		{
			static TimerManager instance;
			return instance;
		}

		void Update()
		{
			for (auto& pair : m_Timers)
			{
				auto& timer = pair.second;
				timer->Tick();				
			}
		}

		Shared<Timer> SetTimer(const std::string& timerName, float rate, bool bLoop)
		{
			auto it = m_Timers.find(timerName);
			if (it != m_Timers.end())
			{
				CS_WARN("%s 이름을 가진 타이머가 이미 존재합니다", timerName.c_str());
				return {};
			}

			Shared<Timer> newTimer = CreateShared<Timer>();
			newTimer->m_Rate = rate;
			newTimer->m_bShouldLoop = bLoop;

			return m_Timers.insert({ timerName, newTimer }).first->second;
		}


		Shared<Timer> SetTimer(const std::string& timerName, TimerCallback callback, float rate, bool bLoop)
		{
			auto it = m_Timers.find(timerName);
			if (it != m_Timers.end())
			{
				CS_WARN("%s 이름을 가진 타이머가 이미 존재합니다", timerName.c_str());
				return {};
			}

			Shared<Timer> newTimer = CreateShared<Timer>();
			newTimer->m_Callback = callback;
			newTimer->m_Rate = rate;
			newTimer->m_bShouldLoop = bLoop;

			return m_Timers.insert({timerName, newTimer}).first->second;
		}


	private:
		TimerManager() = default;
		~TimerManager() = default;

	private:
		std::unordered_map<std::string, Shared<Timer>> m_Timers;
	};
}

#include "cspch.h"
#include "State.h"

namespace Crystal {
	StateStack::~StateStack()
	{
		for (State* state : m_States)
			delete state;
	}

	void StateStack::PushState(State* state)
	{
		if (!m_States.empty())
			m_States.back()->Exit();
		state->Enter();
		m_States.emplace_back(state);
	}

	void StateStack::PopState(State* state)
	{
		auto it = std::find(m_States.begin(), m_States.end(), state);
		if (it != m_States.end())
		{
			(*it)->Exit();
			m_States.erase(it);
			if (!m_States.empty())
				m_States.back()->Enter();
		}
		else
			CS_ERROR("레이어가 존재하지 않습니다");
	}
}
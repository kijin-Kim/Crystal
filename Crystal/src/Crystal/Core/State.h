#pragma once
#include <vector>

namespace Crystal {
	class State
	{
	public:
		State() = default;
		virtual ~State() = default;

		virtual void Enter() {}
		virtual void Exit() {}
		virtual void Update(float deltatTime) {}
	};

	class StateStack final
	{
	public:
		StateStack() = default;
		~StateStack();

		void PushState(State* state);
		void PopState(State* state);

		std::vector<State*>::iterator begin() { return m_States.begin(); }
		std::vector<State*>::iterator end() { return m_States.end(); }

	private:
		std::vector<State*> m_States;
	};
}

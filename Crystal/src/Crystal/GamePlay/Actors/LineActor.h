#pragma once
#include "Actor.h"
#include "Crystal\GamePlay\Components\CollisionComponent.h"

namespace Crystal {

	class LineActor : public Actor
	{
	public:
		LineActor(Object* parent) : Actor(parent)
		{
			m_DebugLineComponent = CreateComponent<RayComponent>("DebugLineComponent");
			m_MainComponent = m_DebugLineComponent;
		}

		~LineActor() override = default;

		void Begin() override 
		{
		}
		void End() override 
		{
		}
		void Update(const float deltaTime) override {}

		RayComponent* GetLineComponent() const { return m_DebugLineComponent; }

	private:
		RayComponent* m_DebugLineComponent = nullptr;
	};

}

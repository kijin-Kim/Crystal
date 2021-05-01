#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"

namespace Crystal {
	class LineActor : public Actor
	{
	public:
		LineActor() = default;
		~LineActor() override = default;

		void Initialize() override
		{
			Actor::Initialize();

			m_DebugLineComponent = CreateComponent<RayComponent>("DebugLineComponent");
			m_MainComponent = m_DebugLineComponent;
		}

		std::shared_ptr<RayComponent> GetLineComponent() const { return m_DebugLineComponent; }


		STATIC_TYPE_IMPLE(LineActor)
	private:
		std::shared_ptr<RayComponent> m_DebugLineComponent = nullptr;
	};
}

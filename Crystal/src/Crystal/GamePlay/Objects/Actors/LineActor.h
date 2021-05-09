#pragma once
#include "Actor.h"
#include "Crystal/Core/Timer.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"

namespace Crystal {
	class LineActor : public Actor
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<Actor>(*this);
			ar & *m_DebugLineComponent;
		}

	public:
		LineActor() = default;
		~LineActor() override = default;

		void Initialize() override
		{
			Actor::Initialize();

			m_DebugLineComponent = CreateComponent<RayComponent>("DebugLineComponent");
			m_MainComponent = m_DebugLineComponent;
		}

		void Update(float deltaTime) override;
		void SetLifeTime(float lifeTime) { m_LifeTime = lifeTime;  }

		std::shared_ptr<RayComponent> GetLineComponent() const { return m_DebugLineComponent; }


		STATIC_TYPE_IMPLE(LineActor)
	private:
		std::shared_ptr<RayComponent> m_DebugLineComponent = nullptr;
		float m_LifeTime = 0.0f;
		Timer timer;
	};
}

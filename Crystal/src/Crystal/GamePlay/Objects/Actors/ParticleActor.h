#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/ParticleComponent.h"


namespace Crystal {
	class ParticleActor : public Actor
	{


	public:
		ParticleActor() = default;
		~ParticleActor() override = default;

		void Initialize() override;
		void Update(const float deltaTime) override;


		STATIC_TYPE_IMPLE(ParticleActor)

	private:
		Timer m_Timer;
	};
}

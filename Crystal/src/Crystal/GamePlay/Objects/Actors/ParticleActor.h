#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/ParticleComponent.h"


namespace Crystal {
	class ParticleActor : public Actor
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<Actor>(*this);
		}

	public:
		ParticleActor() = default;
		~ParticleActor() override = default;

		void Initialize() override;


		STATIC_TYPE_IMPLE(ParticleActor)
	};
}

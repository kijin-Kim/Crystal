#pragma once
#include "PrimitiveComponent.h"


namespace Crystal {
	class ParticleComponent : public PrimitiveComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<PrimitiveComponent>(*this);
		}

	public:
		ParticleComponent() = default;
		~ParticleComponent() override = default;

		STATIC_TYPE_IMPLE(ParticleComponent)
	};
}

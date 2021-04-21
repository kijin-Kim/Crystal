#pragma once
#include "PrimitiveComponent.h"


namespace Crystal {
	class ParticleComponent : public PrimitiveComponent
	{
	public:
		ParticleComponent() = default;
		~ParticleComponent() override = default;
		
		STATIC_TYPE_IMPLE(ParticleComponent)
	};
}



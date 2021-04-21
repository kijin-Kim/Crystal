#pragma once
#include "PlaneQuadActor.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {

class TonemappingActor : public PlaneQuadActor
{
public:
	TonemappingActor()
	{
		auto& resourceManager = ResourceManager::Instance();

	

		


		

	}

	~TonemappingActor() override = default;


	STATIC_TYPE_IMPLE(TonemappingActor)
};
}

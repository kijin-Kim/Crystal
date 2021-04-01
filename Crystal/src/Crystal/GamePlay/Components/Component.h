#pragma once
#include <DirectXMath.h>
#include "Crystal/Math/Math.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "../../Core/Logger.h"
#include "Crystal/Resources/Renderable.h"

namespace Crystal {
	class Component : public Updatable
	{
	public:
		enum ComponentOwnerType
		{
			Owner_Component = 0,
			Owner_Actor = 1
		};
	public:
		Component() = default;
		~Component() override = default;

		STATIC_TYPE_IMPLE(Component)
	};
}

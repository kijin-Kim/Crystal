#pragma once
#include <DirectXMath.h>
#include "Crystal/Math/Math.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "../../Core/Logger.h"
#include "Crystal/Resources/Renderable.h"

namespace Crystal {
	class Component : public Object
	{
	public:
		Component() = default;
		~Component() override = default;

		virtual void RegisterComponent() {}

		virtual bool CanBeRendered() const { return false; }
		virtual bool IsCollisionEnabled() const { return false; }

		STATIC_TYPE_IMPLE(Component)
	};
}

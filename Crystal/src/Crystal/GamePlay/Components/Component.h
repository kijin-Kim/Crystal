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
		Component() = default;
		~Component() override = default;

		void SetOwner(Actor* owner) { m_Owner = owner; }
		Actor* GetOwner() const { if (!m_Owner) CS_WARN("Owner가 nullptr입니다"); return m_Owner; }

		STATIC_TYPE_IMPLE(Component)

	private:
		/*OwnerShip을 가지고 있지 않음*/
		Actor* m_Owner = nullptr;
	};
}

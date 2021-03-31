#pragma once
#include <DirectXMath.h>
#include "Crystal/Math/Math.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "../../Core/Logger.h"
#include "Crystal/Resources/Renderable.h"

namespace Crystal {
	class Component
	{
	public:
		explicit Component(const std::string& name) : m_Name(name) {}
		virtual ~Component() = default;

		virtual void OnCreate() {};

		virtual void Update(const float deltaTime) {}

		void SetOwner(Actor* owner) { m_Owner = owner; }

		Actor* GetOwner() const { if (!m_Owner) CS_WARN("Owner가 nullptr입니다"); return m_Owner; }
		const std::string& GetName() const { return m_Name; }

	private:
		/*OwnerShip을 가지고 있지 않음*/
		Actor* m_Owner = nullptr;
		std::string m_Name;
	};
}

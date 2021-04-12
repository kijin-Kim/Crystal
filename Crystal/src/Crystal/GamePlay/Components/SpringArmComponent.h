#pragma once
#include "TransformComponent.h"
#include <corecrt_math.h>

namespace Crystal {
	class SpringArmComponent final : public TransformComponent
	{
	public:
		SpringArmComponent() = default;
		~SpringArmComponent() override = default;

		void Update(const float deltaTime) override;

		void SetSpringConstant(const float springConstant);
		void SetRestLength(const float restLength);
		void SetOffsetPosition(const DirectX::XMFLOAT3& offset);

		STATIC_TYPE_IMPLE(SpringArmComponent)

	private:
		float m_SpirngConstant = 30.0f;
		float m_RestLength = 0.0f;
		DirectX::XMFLOAT3 m_OffsetPosition = Vector3::Zero;
		DirectX::XMFLOAT3 m_WorldLastPosition = Vector3::Zero;
	};
}

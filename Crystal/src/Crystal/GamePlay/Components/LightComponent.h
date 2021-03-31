#pragma once
#include "TransformComponent.h"

namespace Crystal {
	class LightComponent : public TransformComponent
	{
	public:
		LightComponent() = default;
		~LightComponent() override = default;

		void SetLightColor(const DirectX::XMFLOAT3& color) { m_LightColor = color; }
		const DirectX::XMFLOAT3& GetLightColor() const { return m_LightColor; }


		STATIC_TYPE_IMPLE(LightComponent)
	private:
		DirectX::XMFLOAT3 m_LightColor = Vector3::White;
	};

	class DirectionalLightComponent : public LightComponent
	{
	public:
		DirectionalLightComponent() = default;
		~DirectionalLightComponent() override = default;


		STATIC_TYPE_IMPLE(DirectionalLightComponent)
	};
}

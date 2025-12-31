#pragma once
#include "TransformComponent.h"

namespace Crystal {
	class LightComponent : public TransformComponent
	{
	public:
		LightComponent() = default;
		~LightComponent() override = default;

		void RegisterComponent() override;

		void Update(float deltaTime) override;
		void SetLightColor(const DirectX::XMFLOAT3& color) { m_LightColor = color; }
		const DirectX::XMFLOAT3& GetLightColor() const { return m_LightColor; }

		void SetLightIntensity(const float intensity) { m_Intensity = intensity; }
		float GetLightIntensity() const { return m_Intensity; }

		void SetCastShadow(bool bCastShadow) { m_bCastShadow = bCastShadow; }
		bool GetCastShadow() const { return m_bCastShadow; }

		void SetAffectsWorld(bool bAffectsWorld) { m_bAffectsWorld = bAffectsWorld; }
		bool GetAffectsWorld() const { return m_bAffectsWorld; }

		const DirectX::XMFLOAT4X4& GetLightViewProjection() const { return m_LightViewProjection; }


		

		STATIC_TYPE_IMPLE(LightComponent)
	private:
		bool m_bCastShadow = false;
		DirectX::XMFLOAT4X4 m_LightViewProjection = Matrix4x4::Identity();
		DirectX::XMFLOAT3 m_LightColor = Vector3::White;
		float m_Intensity = 1.0f;

		bool m_bAffectsWorld = true;
	};


	class DirectionalLightComponent : public LightComponent
	{

	public:
		DirectionalLightComponent() = default;
		~DirectionalLightComponent() override = default;


		STATIC_TYPE_IMPLE(DirectionalLightComponent)
	};

	class PointLightComponent : public LightComponent
	{

	public:
		PointLightComponent() = default;
		~PointLightComponent() override = default;

		void SetAttenuationRadius(float radius) { m_AttenuationRadius = radius; }
		float GetAttenuationRadius() const { return m_AttenuationRadius; }

		STATIC_TYPE_IMPLE(PointLightComponent)

	private:
		float m_AttenuationRadius = 0.0f;

	};
}



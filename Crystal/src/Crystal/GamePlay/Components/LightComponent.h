#pragma once
#include "TransformComponent.h"

namespace Crystal {
	class LightComponent : public TransformComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<TransformComponent>(*this);
			ar & m_LightColor;
			ar & m_Intensity;
			ar & m_bCastShadow;
			ar & m_LightViewProjection;
		}

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

		const DirectX::XMFLOAT4X4& GetLightViewProjection() const { return m_LightViewProjection; }


		STATIC_TYPE_IMPLE(LightComponent)
	private:
		bool m_bCastShadow = false;
		DirectX::XMFLOAT4X4 m_LightViewProjection = Matrix4x4::Identity();
		DirectX::XMFLOAT3 m_LightColor = Vector3::White;
		float m_Intensity = 1.0f;
	};

	class LocalLightComponent : public LightComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<LightComponent>(*this);
		}


	public:
		LocalLightComponent() = default;
		~LocalLightComponent() override = default;


		STATIC_TYPE_IMPLE(LocalLightComponent)
	};

	class DirectionalLightComponent : public LightComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<LightComponent>(*this);
		}

	public:
		DirectionalLightComponent() = default;
		~DirectionalLightComponent() override = default;


		STATIC_TYPE_IMPLE(DirectionalLightComponent)
	};
}
